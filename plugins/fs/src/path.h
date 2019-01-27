#ifndef LVPATH_H
#define LVPATH_H

#include <QObject>

namespace lv{

class Path : public QObject{

    Q_OBJECT

public:
    explicit Path(QObject *parent = nullptr);

public slots:
    QString name(const QString& path);

};

}// namespace

#endif // LVPATH_H
