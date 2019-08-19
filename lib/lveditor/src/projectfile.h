/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef LVPROJECTFILE_H
#define LVPROJECTFILE_H

#include "live/lveditorglobal.h"
#include "live/projectentry.h"

#include <QObject>

namespace lv{

class ProjectDocument;

class LV_EDITOR_EXPORT ProjectFile : public ProjectEntry{

    Q_OBJECT
    Q_PROPERTY(lv::ProjectDocument* document READ document NOTIFY documentChanged)

public:
    explicit ProjectFile(const QString& path, ProjectEntry *parent = 0);
    ProjectFile(const QString &path, const QString& name, ProjectEntry *parent);
    ~ProjectFile();

    ProjectDocument* document() const;
    void setDocument(lv::ProjectDocument* document);
    bool isOpen() const;

public slots:
    bool exists() const;

signals:
    /** Signals that the document associated with the file has changed */
    void documentChanged();

private:
    ProjectDocument* m_document;
};

/** Document getter */
inline ProjectDocument *ProjectFile::document() const{
    return m_document;
}

/** Shows if the file is opened */
inline bool ProjectFile::isOpen() const{
    return m_document != 0;
}

/**
 * \brief Returns true if the file exists or false is a temporarry file
 */
inline bool ProjectFile::exists() const{
    if ( name().startsWith(":>") )
        return false;
    return true;
}

}// namespace

#endif // LVPROJECTFILE_H
