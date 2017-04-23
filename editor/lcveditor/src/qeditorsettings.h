#ifndef QEDITORSETTINGS_H
#define QEDITORSETTINGS_H

#include <QObject>
#include "qlcveditorglobal.h"

namespace lcv{

class QProjectDocument;
class Q_LCVEDITOR_EXPORT QEditorSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QString path READ path     CONSTANT)

public:
    explicit QEditorSettings(const QString& path, QObject *parent = 0);
    ~QEditorSettings();

    int fontSize() const;

    const QString& path() const;

    void fromJson(const QJsonObject& root);
    QJsonObject toJson() const;

public slots:
    void reparse();
    void init(const QByteArray& data);
    const QByteArray& content() const;
    void documentOpened(lcv::QProjectDocument* document);
    void documentIsDirtyChanged();

signals:
    void initError(const QString& errorString);
    void fontSizeChanged(int fontSize);

private:
    int        m_fontSize;
    QString    m_path;
    QByteArray m_content;
};

inline int QEditorSettings::fontSize() const{
    return m_fontSize;
}

inline const QString &QEditorSettings::path() const{
    return m_path;
}

inline const QByteArray &QEditorSettings::content() const{
    return m_content;
}

}// namespace

#endif // QEDITORSETTINGS_H
