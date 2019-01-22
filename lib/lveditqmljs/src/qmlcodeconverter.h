#ifndef LVQMLCODECONVERTER_H
#define LVQMLCODECONVERTER_H

#include <QObject>
#include <QJSValue>
#include <QVariant>

#include "live/lveditqmljsglobal.h"

namespace lv{

class QmlEditFragment;

/// \private
class QmlCodeConverter : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue whenBinding READ whenBinding WRITE setWhenBinding NOTIFY whenBindingChanged)

public:
    explicit QmlCodeConverter(QmlEditFragment* edit, QObject *parent = nullptr);
    ~QmlCodeConverter();

    const QJSValue& whenBinding() const;
    void setWhenBinding(const QJSValue& whenBinding);

public slots:
    void writeProperties(const QJSValue& properties);
    void write(const QJSValue options);
    QVariant parse();

    void updateValue();
    void updateBindings();

signals:
    void whenBindingChanged();

private:
    QString buildCode(const QJSValue& value);

    QmlEditFragment* m_edit;
    QJSValue         m_whenBinding;
};

inline const QJSValue& QmlCodeConverter::whenBinding() const{
    return m_whenBinding;
}

inline void QmlCodeConverter::setWhenBinding(const QJSValue& whenBinding){
    m_whenBinding = whenBinding;
    emit whenBindingChanged();
}

}// namespace

#endif // LVQMLCODECONVERTER_H
