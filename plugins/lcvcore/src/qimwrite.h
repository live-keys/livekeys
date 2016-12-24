#ifndef QIMWRITE_H
#define QIMWRITE_H

#include <QObject>
#include <QJSValue>
#include <vector>
#include "qmat.h"

class QImWrite : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue params READ params WRITE setParams NOTIFY paramsChanged)

public:
    QImWrite(QObject* parent = 0);
    ~QImWrite();

    const QJSValue& params() const;
    void setParams(const QJSValue& params);

public slots:
    bool saveImage(const QString& file, QMat* image);

signals:
    void paramsChanged();

private:
    QJSValue m_params;
    std::vector<int> m_convertedParams;
};

inline const QJSValue &QImWrite::params() const{
    return m_params;
}



#endif // QIMWRITE_H
