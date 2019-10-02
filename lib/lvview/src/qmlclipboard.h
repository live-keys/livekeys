#ifndef LVQMLCLIPBOARD_H
#define LVQMLCLIPBOARD_H

#include <QObject>
#include <QClipboard>
#include <QJSValue>

namespace lv{

/// \private
class QmlClipboard : public QObject{

    Q_OBJECT

public:
    explicit QmlClipboard(QObject *parent = 0);

public slots:
    void setText(const QString& text);
    QString asText() const;
    bool hasText() const;

    void setUrls(const QJSValue& value);
    void setPaths(const QJSValue& value);
    bool hasUrls() const;
    QJSValue asUrls() const;

private:
    QClipboard* m_clipboard;
};

}// namespace

#endif // LVQMLCLIPBOARD_H
