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
    Q_PROPERTY(lv::ProjectFile* file  READ file        NOTIFY fileChanged)
    Q_PROPERTY(QByteArray content     READ content     NOTIFY contentChanged)
    Q_PROPERTY(bool isMonitored       READ isMonitored NOTIFY isMonitoredChanged)
    Q_PROPERTY(bool isDirty           READ isDirty     WRITE  setIsDirty     NOTIFY isDirtyChanged)
    Q_ENUMS(OpenMode)

public:
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
    explicit Document(ProjectFile* file, bool isMonitored, Project *parent);
    virtual ~Document();

    virtual QByteArray content();

    void setIsDirty(bool isDirty);
    bool isDirty() const;
    void setIsMonitored(bool isMonitored);
    /** Shows if the document is monitored */
    bool isMonitored() const;

    ProjectFile* file() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    Project* parentAsProject();

signals:
    /** shows dirty state changed */
    void isDirtyChanged();
    /** shows if monitoring state changed */
    void isMonitoredChanged();
    /** triggered when the file changed */
    void fileChanged();
    /** triggered when the document was saved */
    void saved();
    /** shows if the document content changed */
    void contentChanged();

public slots:
    virtual void setContent(const QByteArray& content);
    virtual void readContent();
    virtual int contentLength();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

protected:
    Document(ProjectFile* file, Project* parent);

private:

    Q_DISABLE_COPY(Document)

    QByteArray   m_content;
    ProjectFile* m_file;
    QDateTime    m_lastModified;
    bool         m_isMonitored;
    bool         m_isDirty;
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

/**
 * \brief File getter
 */
inline ProjectFile *Document::file() const{
    return m_file;
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

}// namespace

#endif // LVDOCUMENT_H
