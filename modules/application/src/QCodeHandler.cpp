#include "QCodeHandler.hpp"
#include "QCodeJSHighlighter_p.hpp"

#include <QQuickTextDocument>

QCodeHandler::QCodeHandler(QQuickItem *parent)
    : QQuickItem(parent)
    , m_target(0)
    , m_highlighter(0){

}

QCodeHandler::~QCodeHandler(){
    // m_target and m_highlighter are not owned by this document
}

void QCodeHandler::setTarget(QQuickTextDocument *target){

    m_target      = target;
    m_highlighter = new QCodeJSHighlighter(m_target->textDocument());

    emit targetChanged();
}
