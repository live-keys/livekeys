#ifndef TEXTSEARCH_H
#define TEXTSEARCH_H

#include <QObject>
#include <QVariant>

namespace lv{

class TextSearch : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString text        READ text    WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString value       READ value   WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool asLines        READ asLines WRITE setAsLines NOTIFY asLinesChanged)
    Q_PROPERTY(QVariantList output READ output  NOTIFY outputChanged)

public:
    explicit TextSearch(QObject *parent = nullptr);
    virtual ~TextSearch();

    const QString& text() const;
    const QVariantList& output() const;
    bool asLines() const;
    const QString& value() const;

    void process();


signals:
    void textChanged();
    void outputChanged();
    void valueChanged();

    void asLinesChanged();

public slots:
    void setText(QString text);
    void setValue(const QString& value);

    void setAsLines(bool asLines);

private:
    QString      m_text;
    QString      m_value;
    bool         m_asLines;
    QVariantList m_output;
};

inline const QString &TextSearch::text() const{
    return m_text;
}

inline const QString &TextSearch::value() const{
    return m_value;
}

inline bool TextSearch::asLines() const{
    return m_asLines;
}

inline const QVariantList &TextSearch::output() const{
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

inline void TextSearch::setAsLines(bool asLines){
    if (m_asLines == asLines)
        return;

    m_asLines = asLines;
    emit asLinesChanged();
}

}// namespace

#endif // TEXTSEARCH_H
