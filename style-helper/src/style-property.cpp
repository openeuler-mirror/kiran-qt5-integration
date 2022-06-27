#include "style-property.h"
#include <QProgressBar>
#include <QPushButton>
#include <QVariant>

/// pushbutton
#define KIRAN_STYLE_PROPERTY_BUTTON_TYPE "_kiran_button_type"
/// spinbox
#define KIRAN_STYLE_PROPERTY_SPINBOX_POSITION "_kiran_spinbox_position"
/// progress bar
#define KIRAN_STYLE_PROPERTY_PROGRESSBAR_TEXT_POSITION "_kiran_progressbar_text_position"

namespace Kiran
{
namespace StylePropertyHelper
{
Kiran::ButtonType getButtonType(const QPushButton *btn)
{
    ButtonType buttonType = BUTTON_Normal;

    QVariant var = btn->property(KIRAN_STYLE_PROPERTY_BUTTON_TYPE);
    if (var.isValid())
    {
        bool toInt = false;
        auto temp = static_cast<ButtonType>(var.toInt(&toInt));
        if (toInt)
        {
            buttonType = temp;
        }
    }

    return buttonType;
}

void setButtonType(QPushButton *btn,
                                          Kiran::ButtonType type)
{
    btn->setProperty(KIRAN_STYLE_PROPERTY_BUTTON_TYPE, type);
}

void setProgressBarTextPosition(QProgressBar *progressBar,
                                                       Kiran::ProgressBarTextPosition textPosition)
{
    progressBar->setProperty(KIRAN_STYLE_PROPERTY_PROGRESSBAR_TEXT_POSITION, textPosition);
}

Kiran::ProgressBarTextPosition getProgressBarTextPosition(const QProgressBar *progressBar)
{
    ProgressBarTextPosition progressBarTextPosition = PROGRESS_TEXT_RIGHT;

    QVariant var = progressBar->property(KIRAN_STYLE_PROPERTY_PROGRESSBAR_TEXT_POSITION);
    if (var.isValid())
    {
        bool toInt = false;
        auto temp = static_cast<ProgressBarTextPosition>(var.toInt(&toInt));
        if (toInt)
        {
            progressBarTextPosition = temp;
        }
    }

    return progressBarTextPosition;
}

}  // namespace StylePropertyHelper
}  // namespace Kiran