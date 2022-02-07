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

#ifndef LVDOCUMENT_H
#define LVDOCUMENT_H

#include "live/lveditorglobal.h"

#include <QObject>
#include <QDateTime>

namespace lv{

class Project;
class ProjectFile;

class LV_EDITOR_EXPORT Document : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString path           READ path        NOTIFY pathChanged)
    Q_PROPERTY(QByteArray content     READ content     NOTIFY contentChanged)
    Q_PROPERTY(QString formatType     READ formatType  NOTIFY formatTypeChanged)
    Q_PROPERTY(bool isMonitored       READ isMonitored NOTIFY isMonitoredChanged)
    Q_PROPERTY(bool isDirty           READ isDirty     WRITE  setIsDirty     NOTIFY isDirtyChanged)
    Q_ENUMS(OpenMode)

public:
    friend class Project;
    friend class ProjectDocumentModel;

    class LV_EDITOR_EXPORT Location{
    public:
        Location(int line = -1, int column = 0) : m_line(line), m_column(column){}

        bool isValid() const{ return m_line >= 0; }
        int line() const{ return m_line; }
        int column() const{ return m_column; }
    private:
        int m_line;
        int m_column;
    };

public:
    /** Enum containing possible modes of opening documents */
    enum OpenMode{
        /** The file open in the editor */
        Edit = 0,
        /** Read-only, but any external change will be reflected */
        Monitor,
        /** If not opened, will be open for editing. If already monitored, it will not be available for editing. */
        EditIfNotOpen
    };

public:
    explicit Document(const QString &filePath, const QString& formatType, bool isMonitored, Project *parent);
    virtual ~Document();

    virtual QByteArray content();

    void setIsDirty(bool isDirty);
    bool isDirty() const;
    void setIsMonitored(bool isMonitored);
    /** Shows if the document is monitored */
    bool isMonitored() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    Project* parentAsProject();

    const QString& formatType() const;

    const QString& path() const;
    QString pathHash() const;

signals:
    /** shows dirty state changed */
    void isDirtyChanged();
    /** shows if monitoring state changed */
    void isMonitoredChanged();
    /** triggered when the file changed */
    void fileChanged();
    /** triggered when the formatType changed */
    void formatTypeChanged();
    /** triggered when the document was saved */
    void saved();
    /** shows if the document content changed */
    void contentChanged();
    /** triggered when path changed */
    void pathChanged();
    /** triggered before the document would close */
    void aboutToClose();

public slots:
    virtual void setContent(const QByteArray& content);
    virtual void readContent();
    virtual int contentLength();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

    QString fileName() const;
    bool isOnDisk() const;

protected:
    void setFormatType(const QString& formatType);
    Document(const QString &filePath, Project* parent);

private:
    void beforeClose();
    void setPath(const QString& path);

    Q_DISABLE_COPY(Document)

    QByteArray   m_content;
    QString      m_formatType;
    QDateTime    m_lastModified;
    bool         m_isMonitored;
    bool         m_isDirty;
    QString      m_fileName;
    QString      m_path;
};

inline QByteArray Document::content(){
    return m_content;
}

inline void Document::setContent(const QByteArray &content){
    m_content = content;
    emit contentChanged();
}

inline bool Document::isMonitored() const{
    return m_isMonitored;
}

inline void Document::setIsDirty(bool isDirty){
    if ( m_isDirty == isDirty )
        return;

    m_isDirty = isDirty;
    isDirtyChanged();
}

inline bool Document::isDirty() const{
    return m_isDirty;
}

/**
 * \brief Sets the indicator for monitoring
 */
inline void Document::setIsMonitored(bool isMonitored){
    if ( m_isMonitored == isMonitored )
        return;

    m_isMonitored = isMonitored;
    emit isMonitoredChanged();
}

/**
 * \brief Returns the timestamp of last modification
 */
inline const QDateTime &Document::lastModified() const{
    return m_lastModified;
}

/**
 * \brief Sets the timestamp of latest modification
 */
inline void Document::setLastModified(const QDateTime &lastModified){
    m_lastModified = lastModified;
}

inline const QString &Document::formatType() const{
    return m_formatType;
}

/**
 * \brief Returns the document path
 */
inline const QString &Document::path() const{
    return m_path;
}

}// namespace

#endif // LVDOCUMENT_H
