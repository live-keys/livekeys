#ifndef QLIVECVSETTINGS_H
#define QLIVECVSETTINGS_H

#include <QObject>
#include "qeditorsettings.h"

namespace lcv{

class QLicenseSettings;
class QLicenseContainer;
class QLiveCVSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QEditorSettings*   editor  READ editor  CONSTANT)
    Q_PROPERTY(lcv::QLicenseContainer* license READ license CONSTANT)
    Q_PROPERTY(bool previewMode                READ previewMode CONSTANT)

public:
    ~QLiveCVSettings();

    lcv::QEditorSettings*   editor();
    lcv::QLicenseContainer* license();

    static QLiveCVSettings* initialize(const QString& path, QObject* parent = 0);

    bool previewMode() const;
    void setPreviewMode(bool previewMode);

private:
    QLiveCVSettings(const QString& path, QObject* parent = 0);

private:
    QString           m_path;
    QEditorSettings*  m_editor;
    QLicenseSettings* m_license;
    bool              m_previewMode;
};

inline QEditorSettings *QLiveCVSettings::editor(){
    return m_editor;
}

inline bool QLiveCVSettings::previewMode() const{
    return m_previewMode;
}

inline void QLiveCVSettings::setPreviewMode(bool previewMode){
    m_previewMode = previewMode;
}

}// namespace

#endif // QLIVECVSETTINGS_H
