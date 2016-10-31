#ifndef QLIVECVMAIN_HPP
#define QLIVECVMAIN_HPP

#include <QObject>
#include <QVariantMap>

class QLiveCVMain : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariantMap options READ options WRITE setOptions NOTIFY optionsChanged)


public:
    QLiveCVMain(QObject* parent = 0);
    ~QLiveCVMain();

    QVariantMap options() const;

public slots:
    void setOptions(QVariantMap options);

signals:
    void optionsChanged();
    void run();

private:
    QVariantMap m_options;
};

inline QVariantMap QLiveCVMain::options() const{
    return m_options;
}

inline void QLiveCVMain::setOptions(QVariantMap options){
    if (m_options == options)
        return;

    m_options = options;
    emit optionsChanged();
}

#endif // QLIVECVMAIN_HPP
