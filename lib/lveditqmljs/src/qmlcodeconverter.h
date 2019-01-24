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

    QJSValue& whenBinding();
    void setWhenBinding(const QJSValue& whenBinding);

    QmlEditFragment* editingFragment();

public slots:
    void writeProperties(const QJSValue& properties);
    void write(const QJSValue options);
    QVariant parse();
    void updateBindings();

    void updateFromPalette();

signals:
    void whenBindingChanged();

private:
    QString buildCode(const QJSValue& value);

    QmlEditFragment* m_edit;
    QJSValue         m_whenBinding;
};

inline QJSValue& QmlCodeConverter::whenBinding(){
    return m_whenBinding;
}

inline void QmlCodeConverter::setWhenBinding(const QJSValue& whenBinding){
    m_whenBinding = whenBinding;
    emit whenBindingChanged();
}

inline QmlEditFragment *QmlCodeConverter::editingFragment(){
    return m_edit;
}

}// namespace

#endif // LVQMLCODECONVERTER_H
