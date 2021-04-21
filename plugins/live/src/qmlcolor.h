#ifndef QMLCOLOR_H
#define QMLCOLOR_H

#include <QObject>
#include <QColor>

namespace lv {

class QmlColor : public QObject{

    Q_OBJECT

public:
    explicit QmlColor(QObject *parent = nullptr);

public slots:
    QColor RgbToHsv(QColor rgb);
    QString toHex(QColor rgb);
};

}

#endif // QMLCOLOR_H
