#ifndef LINECONTROL_H
#define LINECONTROL_H

#include <QObject>

class LineControl : public QObject
{
    Q_OBJECT
public:
    explicit LineControl(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LINECONTROL_H