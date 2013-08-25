#ifndef QMATSOURCE_HPP
#define QMATSOURCE_HPP

#include <QQuickItem>
#include "QMat.hpp"
#include "QMatDisplay.hpp"

class QMatSource : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)

public:
    explicit QMatSource(QQuickItem *parent = 0);
    ~QMatSource();

    const QString& file() const;
    void setFile(const QString& file);
    
signals:
    void fileChanged();

private:
    QString m_file;
    
};

inline const QString &QMatSource::file() const{
    return m_file;
}

#endif // QMATSOURCE_HPP
