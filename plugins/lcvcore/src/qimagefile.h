/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef QIMAGEFILE_H
#define QIMAGEFILE_H

#include <QObject>
#include "qmat.h"
#include "qmatdisplay.h"

class QFileSystemWatcher;
/// \private
class QImageFile : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QString source   READ source  WRITE setSource  NOTIFY sourceChanged)
    Q_PROPERTY(int     iscolor  READ iscolor WRITE setIscolor NOTIFY iscolorChanged)
    Q_PROPERTY(bool    monitor  READ monitor WRITE setMonitor NOTIFY monitorChanged)

    Q_ENUMS(Load)

public:
    enum Load{
        CV_LOAD_IMAGE_UNCHANGED  = -1,
        CV_LOAD_IMAGE_GRAYSCALE  =  0,
        CV_LOAD_IMAGE_COLOR      =  1,
        CV_LOAD_IMAGE_ANYDEPTH   =  2,
        CV_LOAD_IMAGE_ANYCOLOR   =  4
    };

public:
    explicit QImageFile(QQuickItem*parent = nullptr);
    ~QImageFile();

    const QString& source() const;
    void setSource(const QString& source);

    int iscolor() const;
    void setIscolor(int iscolor);

    bool monitor() const;
    void setMonitor(bool monitor);

public slots:
    void systemFileChanged(const QString& file);
    void open(const QString& file);

signals:
    void iscolorChanged();
    void sourceChanged();
    void monitorChanged();
    void init();

protected:
    void componentComplete();

private:
    void loadImage();

    QString m_source;
    int     m_iscolor;
    bool    m_monitor;

    QFileSystemWatcher* m_watcher;
};

inline const QString &QImageFile::source() const{
    return m_source;
}

inline bool QImageFile::monitor() const{
    return m_monitor;
}

inline int QImageFile::iscolor() const{
    return m_iscolor;
}

#endif // QIMAGEFILE_H
