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

#ifndef QLIVECVARGUMENTS_HPP
#define QLIVECVARGUMENTS_HPP

#include <QObject>
#include "qliveglobal.h"

namespace lcv{

class QLiveCVCommandLineParser;
class Q_LIVE_EXPORT QLiveCVArguments : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool previewFlag READ previewFlag CONSTANT)

public:
    explicit QLiveCVArguments(
        const QString& header,
        int argc,
        const char* const argv[],
        QObject *parent = 0
    );
    ~QLiveCVArguments();

    bool previewFlag() const;
    bool consoleFlag() const;
    bool fileLogFlag() const;

    bool pluginInfoFlag() const;
    const QString& pluginInfoImport() const;

    bool helpFlag() const;
    bool versionFlag() const;
    QString helpString() const;

    const QStringList& monitoredFiles() const;

    QLiveCVCommandLineParser* parser();

public slots:
    const QString& script() const;
    QString at(int number) const;
    int length() const;
    QString option(const QString& key) const;
    bool isOptionSet(const QString& key) const;

private:
    void initialize(int argc, const char* const argv[]);

    QLiveCVCommandLineParser* m_parser;

    bool m_fileLogFlag;
    bool m_consoleFlag;
    bool m_previewFlag;

    bool m_pluginInfoFlag;
    QString m_pluginInfoImport;

    QStringList m_monitoredFiles;

    QString m_script;

};

inline bool QLiveCVArguments::previewFlag() const{
    return m_previewFlag;
}

inline bool QLiveCVArguments::consoleFlag() const{
    return m_consoleFlag;
}

inline bool QLiveCVArguments::fileLogFlag() const{
    return m_fileLogFlag;
}

inline const QString &QLiveCVArguments::pluginInfoImport() const{
    return m_pluginInfoImport;
}

inline const QStringList &QLiveCVArguments::monitoredFiles() const{
    return m_monitoredFiles;
}

inline QLiveCVCommandLineParser *QLiveCVArguments::parser(){
    return m_parser;
}

inline const QString&QLiveCVArguments::script() const{
    return m_script;
}

}// namespace

#endif // QLIVECVARGUMENTS_HPP
