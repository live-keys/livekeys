#ifndef LVTIMELINESETTINGS_H
#define LVTIMELINESETTINGS_H

#include <QObject>
#include <QMap>

#include "live/mlnode.h"

namespace lv{

/// \private
class TimelineSettings : public QObject{

    Q_OBJECT

public:
    explicit TimelineSettings(const QString& settingsPath, QObject *parent = nullptr);
    ~TimelineSettings();

    void addLoader(const QString& key, const QString& value);
    const QMap<QString, QString>& loaders() const;

private:
    void readFile();
    void readFileContent(const MLNode& root);

    QString                m_path;
    QMap<QString, QString> m_loaders;
};

inline const QMap<QString, QString> &TimelineSettings::loaders() const{
    return m_loaders;
}

}// namespace

#endif // LVTIMELINESETTINGS_H
