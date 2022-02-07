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

#ifndef QVIDEOWRITER_H
#define QVIDEOWRITER_H

#include <QQuickItem>
#include <QQmlParserStatus>
#include <QJSValue>
#include "qmat.h"
#include "live/qmlwritablestream.h"

class QVideoWriterThread;
/// \private
class QVideoWriter : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue options              READ options       WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(lv::QmlWritableStream* stream READ stream        CONSTANT)
    Q_PROPERTY(int framesWritten             READ framesWritten NOTIFY framesWrittenChanged)

    friend class QVideoWriterThread;

public:
    explicit QVideoWriter(QObject *parent = 0);
    virtual ~QVideoWriter();

    QJSValue options() const;
    int framesWritten() const;
    lv::QmlWritableStream* stream() const;

    static void onInStream(QObject* that, const QJSValue& val);

signals:
    void framesWrittenChanged();
    void optionsChanged();

public slots:
    void save();
    void write(QMat* image);
    void setOptions(QJSValue options);

private:
    QVideoWriterThread* createThread(const QString& filename, int fourcc, double fps, const cv::Size frameSize, bool isColor);

    QVideoWriterThread*    m_thread;
    QJSValue               m_options;
    lv::QmlWritableStream* m_stream;
};

inline QJSValue QVideoWriter::options() const{
    return m_options;
}

inline lv::QmlWritableStream *QVideoWriter::stream() const{
    return m_stream;
}

#endif // QVIDEOWRITER_H
