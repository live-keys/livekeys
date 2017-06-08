/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef QLIVECVSETTINGS_H
#define QLIVECVSETTINGS_H

#include <QObject>
#include "qeditorsettings.h"

namespace lcv{

class QLiveCVSettings : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QEditorSettings*   editor  READ editor      CONSTANT)
    Q_PROPERTY(bool previewMode                READ previewMode CONSTANT)
    Q_PROPERTY(QString path                    READ path        CONSTANT)

public:
    ~QLiveCVSettings();

    lcv::QEditorSettings*   editor();

    static QLiveCVSettings* initialize(const QString& path, QObject* parent = 0);

    bool previewMode() const;
    void setPreviewMode(bool previewMode);

    const QString& path() const;

public slots:
    QObject* custom(const QString& key);

private:
    QLiveCVSettings(const QString& path, QObject* parent = 0);

private:
    QString           m_path;
    QEditorSettings*  m_editor;
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

inline const QString &QLiveCVSettings::path() const{
    return m_path;
}

}// namespace

#endif // QLIVECVSETTINGS_H
