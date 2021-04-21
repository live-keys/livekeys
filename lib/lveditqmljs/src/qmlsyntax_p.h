#ifndef LVQMLSYNTAX_H
#define LVQMLSYNTAX_H

#include <QObject>
#include <QJSValue>

#include "documentqmlinfo.h"

namespace lv{

class ViewEngine;
class QmlSyntax : public QObject{

    Q_OBJECT

public:
    explicit QmlSyntax(QObject *parent = nullptr);
    ~QmlSyntax() override;

public slots:
    QJSValue parseObjectDeclarations(const QString& content);
    QJSValue parsePathDeclaration(const QString& content);

private:
    QJSValue recurseObjectDeclarations(DocumentQmlValueObjects::RangeObject* object, const QString& content);

private:
    ViewEngine* m_engine;
};

}// namespace

#endif // LVQMLSYNTAX_H
