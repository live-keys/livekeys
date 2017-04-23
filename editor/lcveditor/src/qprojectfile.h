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

#ifndef QPROJECTFILE_H
#define QPROJECTFILE_H

#include "qlcveditorglobal.h"

#include <QObject>
#include "qprojectentry.h"

namespace lcv{

class QProjectDocument;

class Q_LCVEDITOR_EXPORT QProjectFile : public QProjectEntry{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectDocument* document READ document NOTIFY documentChanged)

public:
    explicit QProjectFile(const QString& path, QProjectEntry *parent = 0);
    QProjectFile(const QString &path, const QString& name, QProjectEntry *parent);
    ~QProjectFile();

    lcv::QProjectDocument* document() const;
    void setDocument(lcv::QProjectDocument* document);
    bool isOpen() const;

signals:
    void documentChanged();

private:
    lcv::QProjectDocument* m_document;
};

inline QProjectDocument *QProjectFile::document() const{
    return m_document;
}

inline bool QProjectFile::isOpen() const{
    return m_document != 0;
}

}// namespace

#endif // QPROJECTFILE_H
