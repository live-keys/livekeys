/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#ifndef LVDOCUMENTATION_H
#define LVDOCUMENTATION_H

#include <QQuickItem>

namespace lv{

class ViewEngine;
class PackageGraph;
class DocumentationLoader;
class Documentation : public QObject{

    Q_OBJECT

public:
    Documentation(const PackageGraph* pg, QObject* parent = nullptr);
    ~Documentation();

    QJSValue load(const QString& language, const QString& path, const QString& detail);

public slots:
    QJSValue load(const QString& path);

private:
    DocumentationLoader* createLoader(ViewEngine *engine, const QString& path);

    const PackageGraph*  m_packageGraph;
    DocumentationLoader* m_defaultLoader;
};

}// namespace

#endif // DOCUMENTATION_H
