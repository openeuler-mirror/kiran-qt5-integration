#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ColorsCard;
}
QT_END_NAMESPACE

class ColorsCard : public QDialog
{
    Q_OBJECT

public:
    explicit ColorsCard(QDialog *parent = nullptr);
    ~ColorsCard() override;

    void renderColorsCard();
    void printAllColors();

public slots:
    void start_view();

private:
    Ui::ColorsCard *ui;
    void init();
};
