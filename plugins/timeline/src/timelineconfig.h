#ifndef LVTIMELINECONFIG_H
#define LVTIMELINECONFIG_H

#include <QObject>
#include <QJSValue>
#include "timelinesettings.h"

namespace lv{

/// \private
class TimelineConfig : public QObject{

    Q_OBJECT

public:
    explicit TimelineConfig(QObject *parent = nullptr);
    ~TimelineConfig();

public slots:
    void addLoader(const QString& key, const QString& path);
    QJSValue loaders() const;

    QJSValue trackMenu(QObject* track);

private:
    TimelineSettings* m_settings;
};

}// namespace

#endif // LVTIMELINECONFIG_H
