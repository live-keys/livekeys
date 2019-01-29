#ifndef TEXTSEARCH_H
#define TEXTSEARCH_H

#include <QObject>

namespace lv{

class TextSearch : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString text      READ text   WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString value     READ value  WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QList<int> output READ output NOTIFY outputChanged)

public:
    explicit TextSearch(QObject *parent = nullptr);
    virtual ~TextSearch();

    const QString& text() const;
    const QList<int>& output() const;
    const QString& value() const;

    void process();

signals:
    void textChanged();
    void outputChanged();
    void valueChanged();

public slots:
    void setText(QString text);
    void setValue(const QString& value);

private:
    QString    m_text;
    QString    m_value;
    QList<int> m_output;
};

inline const QString &TextSearch::text() const{
    return m_text;
}

inline const QString &TextSearch::value() const{
    return m_value;
}

inline const QList<int>& TextSearch::output() const{
    return m_output;
}

inline void TextSearch::setText(QString text){
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged();

    process();
}

inline void TextSearch::setValue(const QString &value){
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged();

    process();
}

}// namespace

#endif // TEXTSEARCH_H
