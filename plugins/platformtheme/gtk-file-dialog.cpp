/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#include "gtk-file-dialog.h"
#include <private/qguiapplication_p.h>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QGuiApplication>
#include <QSharedPointer>
#include <QUrl>
#include <QWindow>
#include <QX11Info>
#include "cmake-macros.h"

#undef signals
#include <gdk/gdk.h>
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif
#include <gtk/gtk.h>

// 比较GTK版本是否>=3.10
#define IS_GTK_VERSION_AT_LEAST_3_10 \
    ((GTK_MAJOR_VERSION > 3) || (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION >= 10))

#if GTK_CHECK_VERSION(3, 10, 0)
// qt5-qtranslations
#define STOCK_CANCEL QCoreApplication::translate("GtkFileDialogHelper", "(_C)Cancel").toUtf8().constData()
#define STOCK_OK QCoreApplication::translate("GtkFileDialogHelper", "(_O)OK").toUtf8().constData()
#define STOCK_OPEN QCoreApplication::translate("GtkFileDialogHelper", "(_O)Open").toUtf8().constData()
#define STOCK_SAVE QCoreApplication::translate("GtkFileDialogHelper", "(_S)Save").toUtf8().constData()
#else
// Deprecated: 3.10
#define STOCK_CANCEL GTK_STOCK_CANCEL
#define STOCK_OK GTK_STOCK_OK
#define STOCK_OPEN GTK_STOCK_OPEN
#define STOCK_SAVE GTK_STOCK_SAVE
#endif

#define PREVIEW_WIDTH 256
#define PREVIEW_HEIGHT 512

namespace Kiran
{
namespace Platformtheme
{
// 包装GTK对话框，封装基础功能
// 若有其他需求，可以在此基础上拿到扩展Dialog句柄，进行进一步操作。
class GtkDialogWrapper : public QWindow
{
    Q_OBJECT
public:
    GtkDialogWrapper(GtkWidget *gtkWidget);
    ~GtkDialogWrapper();

    GtkDialog *gtkDialog() const;

    void exec();
    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent);
    void hide();

Q_SIGNALS:
    void accept();
    void reject();

protected:
    static void onDialogResponse(GtkDialogWrapper *dialog, int responseID);

private slots:
    void onParentWindowDestroyed();

private:
    GtkWidget *m_gtkWidget;
};

GtkDialogWrapper::GtkDialogWrapper(GtkWidget *gtkWidget)
    : m_gtkWidget(gtkWidget)
{
    g_signal_connect_swapped(G_OBJECT(m_gtkWidget), "response", G_CALLBACK(onDialogResponse), this);
    g_signal_connect(G_OBJECT(m_gtkWidget), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
}

GtkDialogWrapper::~GtkDialogWrapper()
{
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    gtk_widget_destroy(m_gtkWidget);
}

GtkDialog *GtkDialogWrapper::gtkDialog() const
{
    return GTK_DIALOG(m_gtkWidget);
}

void GtkDialogWrapper::exec()
{
    if (modality() == Qt::ApplicationModal)
    {
        // 阻塞整个应用程序的输入，包括其他GTK对话框
        gtk_dialog_run(gtkDialog());
    }
    else
    {
        // 阻塞窗口输入，允许其他GTK对话框接受输入
        QEventLoop loop;
        connect(this, SIGNAL(accept()), &loop, SLOT(quit()));
        connect(this, SIGNAL(reject()), &loop, SLOT(quit()));
        loop.exec();
    }
}

bool GtkDialogWrapper::show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent)
{
    if (parent)
    {
        connect(parent, &QWindow::destroyed, this, &GtkDialogWrapper::onParentWindowDestroyed, Qt::UniqueConnection);
    }

    setParent(parent);
    setFlags(flags);
    setModality(modality);

    // 对话框创建窗口句柄，并设置瞬时父窗口
    gtk_widget_realize(m_gtkWidget);
    GdkWindow *gdkWindow = gtk_widget_get_window(m_gtkWidget);
#ifdef GDK_WINDOWING_X11
    if (parent)
    {
        GdkDisplay *gdkDisplay = gdk_window_get_display(gdkWindow);
        if (GDK_IS_X11_DISPLAY(gdkDisplay))
        {
            // 设置对话框的瞬时父窗口
            XSetTransientForHint(gdk_x11_display_get_xdisplay(gdkDisplay), gdk_x11_window_get_xid(gdkWindow), parent->winId());
        }
    }
#endif

    // 设置对话框的模态性
    if (modality != Qt::NonModal)
    {
        gdk_window_set_modal_hint(gdkWindow, true);
        // 注册ModalWindow,避免其他窗口可以接受输入
        QGuiApplicationPrivate::showModalWindow(this);
    }

    gtk_widget_show(m_gtkWidget);

    // 同步 Qt 的时间戳到 GTK，避免窗口管理器因为时间戳落后而将对话框置于底层
    guint32 timestamp = GDK_CURRENT_TIME;
    if (QGuiApplication::platformName() == QLatin1String("xcb"))
    {
        timestamp = QX11Info::appTime();
    }
    gdk_window_focus(gdkWindow, timestamp);

    return true;
}

void GtkDialogWrapper::hide()
{
    QGuiApplicationPrivate::hideModalWindow(this);
    gtk_widget_hide(m_gtkWidget);
}

void GtkDialogWrapper::onDialogResponse(GtkDialogWrapper *dialog, int responseID)
{
    if (responseID == GTK_RESPONSE_OK || responseID == GTK_RESPONSE_ACCEPT)
        emit dialog->accept();
    else
        emit dialog->reject();
}

void GtkDialogWrapper::onParentWindowDestroyed()
{
    // 解除与父窗口的关联,避免父窗口被销毁时,对话框也被销毁
    setParent(nullptr);
}

// GtkFileDialog 实现
GtkFileDialogHelper::GtkFileDialogHelper()
    : m_parent(nullptr), m_previewWidget(nullptr)
{
    m_dialogWrapper.reset(new GtkDialogWrapper(gtk_file_chooser_dialog_new("", 0,
                                                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                                                           STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                                           STOCK_OK, GTK_RESPONSE_OK,
                                                                           NULL)));
    connect(m_dialogWrapper.data(), SIGNAL(accept()), this, SLOT(onAccepted()));
    connect(m_dialogWrapper.data(), SIGNAL(reject()), this, SIGNAL(reject()));

    auto fileChooser = GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog());
    g_signal_connect(fileChooser, "selection-changed", G_CALLBACK(onSelectionChanged), this);
    g_signal_connect_swapped(fileChooser, "current-folder-changed", G_CALLBACK(onCurrentFolderChanged), this);
    g_signal_connect(fileChooser, "update-preview", G_CALLBACK(onUpdatePreview), this);

    m_previewWidget = gtk_image_new();
    gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog()), m_previewWidget);
}

GtkFileDialogHelper::~GtkFileDialogHelper()
{
    // m_dialogWrapper 由 QScopedPointer 自动管理，会在析构时先销毁
    // GtkDialogWrapper 析构时会销毁 GTK widget，GTK 会自动断开所有信号连接
    // m_previewWidget 由 GTK 管理，会在 dialog 销毁时自动释放
}

GtkImage *GtkFileDialogHelper::previewImage() const
{
    return GTK_IMAGE(m_previewWidget);
}

void GtkFileDialogHelper::exec()
{
    m_dialogWrapper->exec();
}

bool GtkFileDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    m_dir.clear();
    m_files.clear();
    updateDialogOptions();
    return m_dialogWrapper->show(windowFlags, windowModality, parent);
}

void GtkFileDialogHelper::hide()
{
    m_dir = directory();
    m_files = selectedFiles();
    m_dialogWrapper->hide();
}

bool GtkFileDialogHelper::defaultNameFilterDisables() const
{
    return false;
}

void GtkFileDialogHelper::setDirectory(const QUrl &directory)
{
    auto fileChooser = GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog());
    QString path = directory.toLocalFile();
    if (path.isEmpty())
        path = directory.path();
    if (!path.isEmpty())
    {
        gtk_file_chooser_set_current_folder(fileChooser, path.toUtf8().constData());
    }
}

QUrl GtkFileDialogHelper::directory() const
{
    auto fileChooser = GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog());
    gchar *folder = gtk_file_chooser_get_current_folder(fileChooser);
    if (!folder)
        return QUrl();

    QUrl url = QUrl::fromLocalFile(QString::fromUtf8(folder));
    g_free(folder);
    return url;
}

void GtkFileDialogHelper::selectFile(const QUrl &filename)
{
    auto fileChooser = GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog());
    QString path = filename.toLocalFile();
    if (path.isEmpty())
        path = filename.path();

    if (options()->acceptMode() == QFileDialogOptions::AcceptSave)
    {
        QFileInfo fi(filename.toLocalFile());
        gtk_file_chooser_set_current_folder(fileChooser, qUtf8Printable(fi.path()));
        gtk_file_chooser_set_current_name(fileChooser, qUtf8Printable(fi.fileName()));
    }
    else
    {
        gtk_file_chooser_select_filename(fileChooser, qUtf8Printable(filename.toLocalFile()));
    }
}

QList<QUrl> GtkFileDialogHelper::selectedFiles() const
{
    QList<QUrl> files;
    auto fileChooser = GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog());
    GSList *filenames = gtk_file_chooser_get_filenames(fileChooser);
    for (GSList *iter = filenames; iter; iter = iter->next)
    {
        gchar *filename = static_cast<gchar *>(iter->data);
        files.append(QUrl::fromLocalFile(QString::fromUtf8(filename)));
        g_free(filename);
    }
    g_slist_free(filenames);
    return files;
}

void GtkFileDialogHelper::setFilter()
{
    updateDialogOptions();
}

void GtkFileDialogHelper::selectNameFilter(const QString &filter)
{
    GtkFileFilter *gtkFilter = m_nameFilters.value(filter);
    if (gtkFilter)
    {
        GtkDialog *gtkDialog = m_dialogWrapper->gtkDialog();
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(gtkDialog), gtkFilter);
    }
}

QString GtkFileDialogHelper::selectedNameFilter() const
{
    GtkDialog *gtkDialog = m_dialogWrapper->gtkDialog();
    GtkFileFilter *filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(gtkDialog));
    if (!filter)
        return QString();
    return m_filterNames.value(filter, QString());
}

void GtkFileDialogHelper::ensureGtkInitialized()
{
    static bool initialized = false;
    if (!initialized)
    {
        if (!gtk_init_check(nullptr, nullptr))
        {
            qWarning() << "Failed to initialize GTK";
        }
        initialized = true;
    }
}

// 根据QFileDialogOptions中fileMode及acceptMode确定GTK_FILE_CHOOSER_ACTION_SAVE
GtkFileChooserAction getFileChooserAction(QSharedPointer<QFileDialogOptions> options)
{
    const auto fileMode = options->fileMode();
    const auto acceptMode = options->acceptMode();

    if (fileMode == QFileDialogOptions::FileMode::AnyFile ||
        fileMode == QFileDialogOptions::FileMode::ExistingFile ||
        fileMode == QFileDialogOptions::FileMode::ExistingFiles)
    {
        if (acceptMode == QFileDialogOptions::AcceptMode::AcceptOpen)
        {
            return GTK_FILE_CHOOSER_ACTION_OPEN;
        }
        else
        {
            return GTK_FILE_CHOOSER_ACTION_SAVE;
        }
    }
    else
    {
        if (acceptMode == QFileDialogOptions::AcceptMode::AcceptOpen)
        {
            return GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
        }
        else
        {
            return GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;
        }
    }
}

// 根据QFileDialogOptions选项同步设置GtkFileChooser
void GtkFileDialogHelper::updateDialogOptions()
{
    auto fileChooser = GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog());
    QSharedPointer<QFileDialogOptions> options = this->options();
    if (!options)
        return;

    gtk_file_chooser_set_local_only(fileChooser, TRUE);
    gtk_window_set_title(GTK_WINDOW(m_dialogWrapper->gtkDialog()), options->windowTitle().toUtf8().constData());
    gtk_file_chooser_set_action(fileChooser, getFileChooserAction(options));

    const auto selectMultiple = options->fileMode() == QFileDialogOptions::FileMode::ExistingFiles;
    gtk_file_chooser_set_select_multiple(fileChooser, selectMultiple);

    const auto confirmOverwrite = !options->testOption(QFileDialogOptions::DontConfirmOverwrite);
    gtk_file_chooser_set_do_overwrite_confirmation(fileChooser, confirmOverwrite);

    const auto readonly = options->testOption(QFileDialogOptions::ReadOnly);
    gtk_file_chooser_set_create_folders(fileChooser, !readonly);

    const auto nameFilter = options->nameFilters();
    if (!nameFilter.isEmpty())
    {
        setNameFilters(nameFilter);
    }

    const auto initialDirectory = options->initialDirectory();
    if (initialDirectory.isLocalFile())
    {
        setDirectory(initialDirectory);
    }

    const auto initiallySelectedFiles = options->initiallySelectedFiles();
    for (const auto &file : initiallySelectedFiles)
    {
        selectFile(file);
    }

    const QString initialNameFilter = options->initiallySelectedNameFilter();
    if (!initialNameFilter.isEmpty())
    {
        selectNameFilter(initialNameFilter);
    }

    GtkWidget *acceptButton = gtk_dialog_get_widget_for_response(m_dialogWrapper->gtkDialog(), GTK_RESPONSE_OK);
    if (acceptButton)
    {
        if (options->isLabelExplicitlySet(QFileDialogOptions::Accept))
            gtk_button_set_label(GTK_BUTTON(acceptButton), options->labelText(QFileDialogOptions::Accept).toUtf8());
        else if (options->acceptMode() == QFileDialogOptions::AcceptOpen)
            gtk_button_set_label(GTK_BUTTON(acceptButton), STOCK_OPEN);
        else
            gtk_button_set_label(GTK_BUTTON(acceptButton), STOCK_SAVE);
    }

    GtkWidget *rejectButton = gtk_dialog_get_widget_for_response(m_dialogWrapper->gtkDialog(), GTK_RESPONSE_CANCEL);
    if (rejectButton)
    {
        if (options->isLabelExplicitlySet(QFileDialogOptions::Reject))
            gtk_button_set_label(GTK_BUTTON(rejectButton), options->labelText(QFileDialogOptions::Reject).toUtf8());
        else
            gtk_button_set_label(GTK_BUTTON(rejectButton), STOCK_CANCEL);
    }
}

// 更新过滤器列表同步到GTK文件选择框，并缓存过滤器名称映射
void GtkFileDialogHelper::setNameFilters(const QStringList &filters)
{
    auto fileChooser = GTK_FILE_CHOOSER(m_dialogWrapper->gtkDialog());
    QSharedPointer<QFileDialogOptions> options = this->options();
    if (!options)
        return;

    // 清除现有过滤器
    GSList *gtkFilters = gtk_file_chooser_list_filters(fileChooser);
    for (GSList *iter = gtkFilters; iter; iter = iter->next)
    {
        gtk_file_chooser_remove_filter(fileChooser, GTK_FILE_FILTER(iter->data));
    }
    g_slist_free(gtkFilters);

    m_nameFilters.clear();
    m_filterNames.clear();

    // 添加新的过滤器
    for (const QString &nameFilter : filters)
    {
        GtkFileFilter *filter = gtk_file_filter_new();
        const QString name = nameFilter.left(nameFilter.indexOf(QLatin1Char('(')));
        const QStringList extensions = cleanFilterList(nameFilter);

        const QString filterDesc = name.isEmpty() ? extensions.join(QLatin1Char(',')) : name;
        gtk_file_filter_set_name(filter, filterDesc.toUtf8().constData());

        for (const QString &extension : extensions)
        {
            gtk_file_filter_add_pattern(filter, extension.toUtf8().constData());
        }
        gtk_file_chooser_add_filter(fileChooser, filter);
        m_nameFilters.insert(nameFilter, filter);
        m_filterNames.insert(filter, nameFilter);
    }
}
void GtkFileDialogHelper::onAccepted()
{
    emit accept();
}

void GtkFileDialogHelper::onRejected()
{
    emit reject();
}

void GtkFileDialogHelper::onSelectionChanged(GtkDialog *gtkDialog, GtkFileDialogHelper *helper)
{
    QString selection;
    auto fileChooser = GTK_FILE_CHOOSER(gtkDialog);
    gchar *filename = gtk_file_chooser_get_filename(fileChooser);
    if (filename)
    {
        selection = QString::fromUtf8(filename);
        g_free(filename);
    }
    emit helper->currentChanged(QUrl::fromLocalFile(selection));
}

void GtkFileDialogHelper::onCurrentFolderChanged(GtkFileDialogHelper *helper)
{
    emit helper->directoryEntered(helper->directory());
}

void GtkFileDialogHelper::onUpdatePreview(GtkDialog *dialog, GtkFileDialogHelper *helper)
{
    auto fileChooser = GTK_FILE_CHOOSER(helper->m_dialogWrapper->gtkDialog());
    gchar *filename = gtk_file_chooser_get_preview_filename(fileChooser);
    if (!filename)
    {
        gtk_file_chooser_set_preview_widget_active(fileChooser, false);
        return;
    }

    QString path = QString::fromUtf8(filename);
    g_free(filename);

    // 检查文件是否存在
    QFileInfo fileinfo(path);
    if (!fileinfo.exists() || !fileinfo.isFile())
    {
        gtk_file_chooser_set_preview_widget_active(fileChooser, false);
        return;
    }

    // 保持图像的宽高比
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size(path.toUtf8().constData(), PREVIEW_WIDTH, PREVIEW_HEIGHT, 0);
    if (pixbuf)
    {
        gtk_image_set_from_pixbuf(helper->previewImage(), pixbuf);
        g_object_unref(pixbuf);
    }

    gtk_file_chooser_set_preview_widget_active(fileChooser, pixbuf ? true : false);
}
}  // namespace Platformtheme
}  // namespace Kiran
#include "gtk-file-dialog.moc"