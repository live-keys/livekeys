#include "qmlclipboard.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"

#include <QGuiApplication>
#include <QMimeData>
#include <QJSValueIterator>
#include <QUrl>
#include <QQmlEngine>

namespace lv{

QmlClipboard::QmlClipboard(QObject *parent)
    : QObject(parent)
{
    m_clipboard = QGuiApplication::clipboard();
}

void QmlClipboard::setText(const QString &text){
    m_clipboard->setText(text);
}

QString QmlClipboard::asText() const{
    return m_clipboard->text();
}

bool QmlClipboard::hasText() const{
    return m_clipboard->mimeData()->hasText();
}

void QmlClipboard::setUrls(const QJSValue &value){
    QList<QUrl> urls;

    QJSValueIterator it(value);
    while ( it.next() ){
        if ( it.name() != "length" ){
            urls.append(QUrl(it.value().toString()));
        }
    }

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setUrls(urls);
    m_clipboard->setMimeData(newMimeData);
}

void QmlClipboard::setPaths(const QJSValue &value){
    QList<QUrl> urls;

    QJSValueIterator it(value);
    while ( it.next() ){
        if ( it.name() != "length" ){
            urls.append(QUrl::fromLocalFile(it.value().toString()));
        }
    }

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setUrls(urls);
    m_clipboard->setMimeData(newMimeData);
}

bool QmlClipboard::hasUrls() const{
    return m_clipboard->mimeData()->hasUrls();
}

QJSValue QmlClipboard::asUrls() const{
    QQmlEngine* engine = lv::ViewContext::instance().engine()->engine();
    if ( !hasUrls() )
        return QJSValue();

    QList<QUrl> urls = m_clipboard->mimeData()->urls();

    QJSValue result = engine->newArray(urls.size());
    for ( int i = 0; i < urls.size(); ++i ){
        result.setProperty(i, urls[i].toString());
    }

    return result;
}

} // namespace
