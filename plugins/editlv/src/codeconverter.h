#ifndef LVCODECONVERTER_H
#define LVCODECONVERTER_H

#include <QObject>
#include <QJSValue>
#include <QVariant>
#include "live/elements/parseddocument.h"

namespace lv{

class LvEditFragment;
//class DocumentQmlScope;
class CodeCompletionModel;

/// \private
class CodeConverter : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue whenBinding READ whenBinding WRITE setWhenBinding NOTIFY whenBindingChanged)

public:
    explicit CodeConverter(LvEditFragment* edit, QObject *parent = nullptr);
    ~CodeConverter();

    QJSValue& whenBinding();
    void setWhenBinding(const QJSValue& whenBinding);

    LvEditFragment* editingFragment();

    static QObject* create(
        const el::ParsedDocument& pd,
        const QString& declaration,
        const QString& path,
        QObject* parent = nullptr
    );

public slots:
    void writeProperties(const QJSValue& properties);
    void write(const QJSValue options);
    QVariant parse();
    void updateBindings();
    void suggestionsForExpression(const QString& expression, lv::CodeCompletionModel* model);
    bool bindExpression(const QString& expression);

    void updateFromPalette();

signals:
    void whenBindingChanged();

private:
    QString buildCode(const QJSValue& value);

    LvEditFragment*  m_edit;
    QJSValue         m_whenBinding;
};

inline QJSValue& CodeConverter::whenBinding(){
    return m_whenBinding;
}

inline void CodeConverter::setWhenBinding(const QJSValue& whenBinding){
    m_whenBinding = whenBinding;
    emit whenBindingChanged();
}

inline LvEditFragment *CodeConverter::editingFragment(){
    return m_edit;
}

}// namespace

#endif // LVQMLCODECONVERTER_H
