#pragma once

#include <QObject>
//KiranStyle提供的更便捷的方法取出KiranStyle里相关颜色的类
class KiranPalette : public QObject
{
    Q_OBJECT
public:
    enum ColorType
    {
        NoType,
        ObviousBackground,
        FrameBorder,
        PlaceHolderText,
        NColorTypes
    };
    Q_ENUM(ColorType);

    explicit KiranPalette(QObject *parent=0);
    ~KiranPalette();
};
