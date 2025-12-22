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
#pragma once
#include <qpa/qplatformdialoghelper.h>
#include <QDialog>
#include <QScopedPointer>
#include <QWindow>
#include <QMap>

typedef struct _GtkFileFilter GtkFileFilter;
typedef struct _GtkImage GtkImage;
typedef struct _GtkDialog GtkDialog;
typedef struct _GtkWidget GtkWidget;

namespace Kiran
{
namespace Platformtheme
{
class GtkDialogWrapper;
class GtkFileDialogHelper : public QPlatformFileDialogHelper
{
    Q_OBJECT
public:
    explicit GtkFileDialogHelper();
    ~GtkFileDialogHelper() override;

    GtkImage* previewImage() const;

    void exec() override;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) override;
    void hide() override;

    bool defaultNameFilterDisables() const override;
    void setDirectory(const QUrl &directory) override;
    QUrl directory() const override;
    void selectFile(const QUrl &filename) override;
    QList<QUrl> selectedFiles() const override;
    void setFilter() override;
    void selectNameFilter(const QString &filter) override;
    QString selectedNameFilter() const override;
    static void ensureGtkInitialized();
    
private Q_SLOTS:
    void onAccepted();
    void onRejected();

private:
    void updateDialogOptions();
    void setNameFilters(const QStringList &filters);
    static void onSelectionChanged(GtkDialog *gtkDialog, GtkFileDialogHelper *helper);
    static void onCurrentFolderChanged(GtkFileDialogHelper *helper);
    static void onUpdatePreview(GtkDialog *dialog, GtkFileDialogHelper *helper);

private:
    QWindow *m_parent;
    QUrl m_dir;
    QList<QUrl> m_files;
    QMap<QString, GtkFileFilter *> m_nameFilters;
    QMap<GtkFileFilter*, QString> m_filterNames;
    QScopedPointer<GtkDialogWrapper> m_dialogWrapper;
    GtkWidget *m_previewWidget;
};
}  // namespace Platformtheme
}  // namespace Kiran