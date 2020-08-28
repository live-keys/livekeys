#ifndef LVQMLTOKENIZER_H
#define LVQMLTOKENIZER_H

#include <QObject>
#include <QJSValue>

namespace lv{

class ViewEngine;
class QmlTokenizer : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue tokenKind  READ tokenKind CONSTANT)
    Q_PROPERTY(QJSValue tokenState READ tokenState CONSTANT)

public:
    QmlTokenizer(QObject* parent = nullptr);
    ~QmlTokenizer();

    QJSValue tokenKind() const;
    QJSValue tokenState() const;

public slots:
    QJSValue scan(const QString& text);

private:
    QJSValue    m_tokenKind;
    QJSValue    m_tokenState;

    ViewEngine* m_engine;
};

inline QJSValue QmlTokenizer::tokenKind() const{
    return m_tokenKind;
}

inline QJSValue QmlTokenizer::tokenState() const{
    return m_tokenState;
}

}// namespace

#endif // LVQMLTOKENIZER_H
