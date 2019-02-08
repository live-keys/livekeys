2/****************************************************************************
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

#ifndef QVIDEOWRITER_H
#define QVIDEOWRITER_H

#include <QQuickItem>
#include <QQmlParserStatus>
#include <QJSValue>
#include "qmat.h"

class QVideoWriterThread;
/// \private
class QVideoWriter : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* input       READ input         WRITE  setInput      NOTIFY inputChanged)
    Q_PROPERTY(int framesWritten READ framesWritten NOTIFY framesWrittenChanged)

    friend class QVideoWriterThread;

public:
    explicit QVideoWriter(QQuickItem *parent = 0);
    virtual ~QVideoWriter();

    QMat* input() const;
    int framesWritten() const;

    QString getKey(const QString& filename, int fourcc, double fps, const cv::Size frameSize) const;

protected:
    void componentComplete();

signals:
    void inputChanged();
    void framesWrittenChanged();

public slots:
    void staticLoad(const QJSValue& params);
    void setInput(QMat* input);
    void save();

    void write(QMat* image);

private:
    QVideoWriterThread* createThread(const QString& filename, int fourcc, double fps, const cv::Size frameSize, bool isColor);

    QMat*    m_input;
    QVideoWriterThread* m_thread;
};

inline QMat *QVideoWriter::input() const{
    return m_input;
}

inline void QVideoWriter::setInput(QMat *input){
    m_input = input;
    emit inputChanged();
    write(input);
}



#endif // QVIDEOWRITER_H
