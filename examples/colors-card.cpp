#include "colors-card.h"
#include "ui_colors-card.h"

#include <QDebug>
#include <QMenu>
#include <QStyleFactory>
#include "examples/ui_widget-factory.h"
#include "lib/theme/style.h"
#include "lib/theme/palette.h"
#include <QMetaEnum>

ColorsCard::ColorsCard(QDialog *parent) : QDialog(parent), ui(new Ui::ColorsCard)
{
    ui->setupUi(this);
    init();
}

void ColorsCard::init()
{
    this->setWindowTitle("ColorsCard");
}

ColorsCard::~ColorsCard()
{
    delete ui;
}

void ColorsCard::renderColorsCard()
{
    // 在渲染之前先清空 ui->m_infoSpace1 ui->m_infoSpace2
    // 清空 ui->m_infoSpace1
    QLayoutItem* item1;
    while ((item1 = ui->m_infoSpace1->takeAt(0)) != nullptr) {
        delete item1->widget();
        delete item1;
    }
    // 清空 ui->m_infoSpace2
    QLayoutItem* item2;
    while ((item2 = ui->m_infoSpace2->takeAt(0)) != nullptr) {
        delete item2->widget();
        delete item2;
    }

    // 清空 ui->m_baseColors
    QLayoutItem* item3;
    while ((item3 = ui->m_baseColors->takeAt(0)) != nullptr) {
        delete item3->widget();
        delete item3;
    }

    Kiran::Theme::Palette::BaseColors baseColors = Kiran::Theme::DEFAULT_PALETTE()->getBaseColors();
    auto baseColorsLayout = this->ui->m_baseColors;
    auto baseGroupBox = new QGroupBox("baseColors", this);
    auto baseGroupLayout = new QVBoxLayout;
    auto spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    struct ColorData {
        QString name;
        QString tooltip;
        QColor color;
    };

    // 定义颜色数据数组
    ColorData colors[] = {
        {"baseBackground", "用于窗口背景、输入框背景和图标，即底色", baseColors.baseBackground},
        {"baseForeground", "用于文字和图标", baseColors.baseForeground},
        {"containerBackground", "用于容器背景，例如色块", baseColors.containerBackground},
        {"widgetBackground", "用于大部分控件的默认背景色", baseColors.widgetBackground},
        {"widgetBorder", "用于大部分控件的边框色、Switch关、滑块背景", baseColors.widgetBorder},
        {"widgetSelection", "用于控件选中背景、Switch开、滑块值、check/radio按钮选中", baseColors.widgetSelection},
        {"widgetMain", "用于文字和图标", baseColors.widgetMain},
        {"widgetWarning", "用于危险提示文字、图标和输入框错误描边", baseColors.widgetWarning}
    };

    // 遍历颜色数据数组并添加到布局中
    for (const auto& colorData : colors) {
        auto hexColorStr = QString("%1: \t%2").arg(colorData.name, colorData.color.name());
        auto colorLabel = new QLabel(hexColorStr);
        colorLabel->setFixedSize(350, 20);
        colorLabel->setToolTip(colorData.tooltip);

        auto colorPanel = new QLabel();
        colorPanel->setFixedSize(100, 20);
        colorPanel->setAutoFillBackground(true);
        QPalette palette = colorLabel->palette();
        palette.setColor(QPalette::Window, colorData.color);
        colorPanel->setPalette(palette);

        auto layout = new QHBoxLayout;
        layout->addWidget(colorLabel);
        layout->addWidget(colorPanel);
        baseGroupLayout->addLayout(layout);
    }

    baseGroupLayout->addSpacerItem(spacer);
    baseGroupBox->setLayout(baseGroupLayout);
    baseColorsLayout->addWidget(baseGroupBox);

    for (int i = 0; i < Kiran::Theme::Palette::N_COLOR_GROUPS; i++) {
        auto groupBox = new QGroupBox(QMetaEnum::fromType<Kiran::Theme::Palette::ColorGroup>().valueToKey(i), this);
        auto groupLayout = new QVBoxLayout;
        for (int j = 0; j < Kiran::Theme::Palette::N_COLOR_ROLES; j++) {
            auto colorGroup = static_cast<Kiran::Theme::Palette::ColorGroup>(i);
            auto colorRole = static_cast<Kiran::Theme::Palette::ColorRole>(j);
            QColor color = Kiran::Theme::DEFAULT_PALETTE()->getColor(colorGroup,colorRole);
            QString colorValue = QString("0x%1%2%3")
                .arg(color.red(), 2, 16, QLatin1Char('0'))
                .arg(color.green(), 2, 16, QLatin1Char('0'))
                .arg(color.blue(), 2, 16, QLatin1Char('0'));

            QString colorInfo = QString("%1+%2 \tis %3, Alpha is %4")
                .arg(QMetaEnum::fromType<Kiran::Theme::Palette::ColorGroup>().valueToKey(i))
                .arg(QMetaEnum::fromType<Kiran::Theme::Palette::ColorRole>().valueToKey(j))
                .arg(colorValue)
                .arg(color.alphaF());

            auto colorLabel = new QLabel(colorInfo);
            colorLabel->setFixedSize(350, 20);
            auto colorPanel = new QLabel();
            colorPanel->setFixedSize(100, 20);
            colorPanel->setAutoFillBackground(true);
            QPalette palette = colorLabel->palette();
            palette.setColor(QPalette::Window, color);
            colorPanel->setPalette(palette);

            auto layout = new QHBoxLayout;
            layout->addWidget(colorLabel);
            layout->addWidget(colorPanel);
            groupLayout->addLayout(layout);
        }
        groupBox->setLayout(groupLayout);
        auto infoSpace = i < 3 ? ui->m_infoSpace1 : ui->m_infoSpace2;
        infoSpace->addWidget(groupBox);
    }
}

void ColorsCard::start_view()
{
    this->show();
    renderColorsCard();
}
// 遍历 ColorGroup 和 ColorRole 结构体，并输出颜色
void ColorsCard::printAllColors()
{
    Kiran::Theme::Palette* kiranPalette = Kiran::Theme::DEFAULT_PALETTE();
    const Kiran::Theme::Palette::BaseColors& baseColors = kiranPalette->getBaseColors();

    for (int group = 0; group < Kiran::Theme::Palette::N_COLOR_GROUPS; group++)
    {
        for (int role = 0; role < Kiran::Theme::Palette::N_COLOR_ROLES; role++)
        {
            auto colorGroup = static_cast<Kiran::Theme::Palette::ColorGroup>(group);
            auto colorRole = static_cast<Kiran::Theme::Palette::ColorRole>(role);

            QColor color = kiranPalette->getColor(colorGroup, colorRole);

            qDebug() << "Color for Group" << colorGroup << "Role" << colorRole << ":"
                     << "RGB(" << color.red() << "," << color.green() << "," << color.blue() << ")";
        }
    }
}
