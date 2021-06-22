#ifndef LVTIMELINESETTINGS_H
#define LVTIMELINESETTINGS_H

#include <QObject>
#include <QJSValue>
#include <QMap>

#include "live/lvtimelineglobal.h"
#include "live/mlnode.h"

namespace lv{

class ViewEngine;
class Settings;

class LV_TIMELINE_EXPORT TimelineSettings : public QObject{

    Q_OBJECT

public:
    class TrackType{
    public:
        TrackType() : extension(nullptr){}

        QString  label;
        QString  typeReference;
        QString  factory;
        QString  extensionPath;
        QObject* extension;
        bool    enabled;
    };

public:
    explicit TimelineSettings(const QString& settingsPath, QObject *parent = nullptr);
    ~TimelineSettings();

    void addTrackType(
            const QString& typeReference,
            const QString& label,
            const QString& path,
            bool enabled = true,
            const QString &extensionPath = QString());
    const QMap<QString, TrackType> trackTypes() const;
    TrackType trackType(const QString& typeReference) const;
    QJSValue trackMenu(const QString& typeReference, ViewEngine* engine);

    void addLoader(const QString& key, const QString& value);
    const QMap<QString, QString>& loaders() const;

    static TimelineSettings* grabFrom(Settings* settings);

private:
    void readFile();
    void readFileContent(const MLNode& root);

    QString                  m_path;
    QMap<QString, TrackType> m_trackTypes;
    QMap<QString, QString>   m_loaders;
};

inline const QMap<QString, QString> &TimelineSettings::loaders() const{
    return m_loaders;
}

}// namespace

#endif // LVTIMELINESETTINGS_H
