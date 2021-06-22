#include "qmlcolor.h"
#include <QColor>
#include <QDebug>

namespace lv {

QmlColor::QmlColor(QObject *parent) : QObject(parent)
{

}

QColor QmlColor::RgbToHsv(QColor rgb)
{
    return rgb.toHsv();
}

QString QmlColor::toHex(QColor rgb){
    return rgb.name(QColor::HexRgb);
}

}
