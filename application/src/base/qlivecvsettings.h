#ifndef QLIVECVSETTINGS_H
#define QLIVECVSETTINGS_H

#include <QObject>
#include "qliveglobal.h"
#include "qeditorsettings.h"

namespace lcv{

class QLicenseSettings;
class QLicenseContainer;
class QLiveCVSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QEditorSettings*   editor  READ editor  CONSTANT)
    Q_PROPERTY(lcv::QLicenseContainer* license READ license CONSTANT)

public:
    ~QLiveCVSettings();

    lcv::QEditorSettings*   editor();
    lcv::QLicenseContainer* license();

    static QLiveCVSettings* initialize(const QString& path, QObject* parent = 0);

private:
    QLiveCVSettings(const QString& path, QObject* parent = 0);

private:
    QString           m_path;
    QEditorSettings*  m_editor;
    QLicenseSettings* m_license;
};

inline QEditorSettings *QLiveCVSettings::editor(){
    return m_editor;
}

}// namespace

#endif // QLIVECVSETTINGS_H
