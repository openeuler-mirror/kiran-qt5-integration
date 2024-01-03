//
// Created by liuxinhao on 2022/6/20.
//
#pragma once

#include <QGuiApplication>
#include <QSignalMapper>
#include <QWidget>

#include "font-size-manager.h"

namespace Kiran
{
class FontSizeManagerPrivate : public QObject
{
    Q_OBJECT
public:
    explicit FontSizeManagerPrivate(FontSizeManager* ptr)
    {
        // 先缓存该指针，尽量避免访问该指针以及调用方法，因KiranFontBindingManager还为构造完
        q_ptr = ptr;
    };

    ~FontSizeManagerPrivate() override{};

    void init()
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        connect(qGuiApp, &QGuiApplication::fontChanged, this, &FontSizeManagerPrivate::handleGuiAppFontChanged);
#else
        connect(qGuiApp, &QGuiApplication::fontDatabaseChanged, [this](){
            this->handleGuiAppFontChanged(qGuiApp->font());
        });
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        connect(&m_destorySignalMapper, &QSignalMapper::mappedWidget, this, &FontSizeManagerPrivate::handleBindWidgetDestroyed);
#else
        connect(&m_destorySignalMapper, QOverload<QWidget *>::of(&QSignalMapper::mapped), this, &FontSizeManagerPrivate::handleBindWidgetDestroyed);
#endif
    }

    void updateWidgetFont()
    {
        for (int i = 0; i < FontSizeManager::FONT_LEVEL_LAST; i++)
        {
            const auto& binderList = m_binderMap[i];
            for (auto& binderWidget : binderList)
            {
                binderWidget->setFont(q_ptr->getFont(static_cast<FontSizeManager::FontLevelEnum>(i), binderWidget->font()));
            }
        }
    }

public slots:

    void handleBindWidgetDestroyed(QWidget* widget)
    {
        q_ptr->unbind(widget);
    };

    void handleGuiAppFontChanged(const QFont& font)
    {
        q_ptr->setUniversalFontSize(FontSizeManager::fontPixelSize(font));
    };

private:
    FontSizeManager* q_ptr;
    Q_DECLARE_PUBLIC(FontSizeManager);

    // 系统标准字号分级-px
    quint16 m_fontSizes[FontSizeManager::FONT_LEVEL_LAST] = {27, 20, 15, 13, 12, 11, 10};
    // m_fontSizes标准字号分级差值
    quint16 m_fontSizeDiff = 0;

    // 控件绑定关系存储
    QList<QWidget*> m_binderMap[FontSizeManager::FONT_LEVEL_LAST];

    QSignalMapper m_destorySignalMapper;
};
}  // namespace Kiran
