#ifndef QEDITORSETTINGS_H
#define QEDITORSETTINGS_H

#include <QObject>
#include "qlcveditorglobal.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QEditorSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)

public:
    explicit QEditorSettings(const QString& path, QObject *parent = 0);
    ~QEditorSettings();

    int fontSize() const;

public slots:
    void reparse();
    void save();
    void init(const QByteArray& data);
    const QByteArray& content() const;

signals:
    void fontSizeChanged(int fontSize);

private:
    int        m_fontSize;
    QString    m_path;
    QByteArray m_content;
};

inline int QEditorSettings::fontSize() const{
    return m_fontSize;
}

inline const QByteArray &QEditorSettings::content() const{
    return m_content;
}

}// namespace

#endif // QEDITORSETTINGS_H
