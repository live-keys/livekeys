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

#ifndef LVPROJECTDOCUMENT_H
#define LVPROJECTDOCUMENT_H

#include <QObject>
#include <QDateTime>
#include <QLinkedList>
#include <QAbstractUndoItem>
#include <QTextBlockUserData>

#include <functional>

#include "live/lveditorglobal.h"

#include <QDebug>

namespace lv{

class Project;
class ProjectFile;
class ProjectDocument;
class ProjectDocumentBlockData;
class DocumentHandler;

class LV_EDITOR_EXPORT ProjectDocumentMarker{

public:
    friend class ProjectDocument;
    /** Shared pointer to the marker */
    typedef QSharedPointer<ProjectDocumentMarker>       Ptr;
    /** Const shared pointer to the marker */
    typedef QSharedPointer<const ProjectDocumentMarker> ConstPtr;

public:
    int position() const{ return m_position; }
    bool isValid() const{ return m_position != -1; }
    /** Create a ProjectDocumentMarker */
    static Ptr create(){ return ProjectDocumentMarker::Ptr(new ProjectDocumentMarker); }
    ~ProjectDocumentMarker(){}

private:
    ProjectDocumentMarker(int position = -1) : m_position(position){}
    void invalidate(){m_position = -1;}

private:
    int m_position;
};

class LV_EDITOR_EXPORT ProjectDocumentSection{

public:
    friend class ProjectDocument;
    friend class ProjectDocumentBlockData;
    /** Shared pointer to a section */
    typedef QSharedPointer<ProjectDocumentSection>       Ptr;
    /** Const shared pointer to a section */
    typedef QSharedPointer<const ProjectDocumentSection> ConstPtr;

public:
    int position() const{ return m_position; }
    int length() const{ return m_length; }
    int type() const{ return m_type; }
    bool isValid() const{ return m_position != -1; }
    void resize(int newLength){ m_length = newLength; }
    ~ProjectDocumentSection();

    void setUserData(void* data){ m_userData = data; }
    void* userData(){ return m_userData; }

    ProjectDocument* document(){ return m_document; }
    ProjectDocumentBlockData* parentBlock(){ return m_parentBlock; }

    void onTextChanged(std::function<void(ProjectDocumentSection::Ptr, int, int, const QString &)> handler);

    /** Create a ProjectDocumentMarker with given parameters */
    static Ptr create(int type, int position = -1, int length = 0){
        return ProjectDocumentSection::Ptr(new ProjectDocumentSection(0, type, position, length));
    }

private:
    ProjectDocumentSection(ProjectDocument* document, int type, int position = -1, int length = 0)
        : m_document(document), m_type(type), m_position(position), m_length(length), m_userData(0), m_parentBlock(0)
    {}
    static Ptr create(ProjectDocument* document, int type, int position = -1, int length = 0){
        return ProjectDocumentSection::Ptr(new ProjectDocumentSection(document, type, position, length));
    }
    void invalidate();

private:
    ProjectDocument* m_document;
    int              m_type;
    int              m_position;
    int              m_length;
    void*            m_userData;
    ProjectDocumentBlockData* m_parentBlock;
    std::function<void(ProjectDocumentSection::Ptr, int, int, const QString&)> m_textChangedHandler;
};

/**
  \private
*/
class LV_EDITOR_EXPORT ProjectDocumentAction : public QAbstractUndoItem{

public:
    /** Default constructor */
    ProjectDocumentAction(
            ProjectDocument* pParent,
            int pPosition,
            const QString& pCharsAdded,
            const QString& pCharsRemoved,
            bool pCommited)
        : parent(pParent)
        , charsAdded(pCharsAdded)
        , charsRemoved(pCharsRemoved)
        , position(pPosition)
        , commited(pCommited)
    {
    }

    void undo();
    void redo();

    ProjectDocument* parent;

    QString charsAdded;
    QString charsRemoved;
    int     position;
    bool    commited;
};

typedef std::function<void(const QTextBlock& tb, int& numLines, QString& replacement)> CollapseFunctionType;

/**
  \private
*/
class LV_EDITOR_EXPORT ProjectDocumentBlockData : public QTextBlockUserData{

public:

    ProjectDocumentBlockData();
    ~ProjectDocumentBlockData();

    void addSection(ProjectDocumentSection::Ptr section);
    void removeSection(ProjectDocumentSection::Ptr section);
    void removeSection(ProjectDocumentSection* section);

    QLinkedList<ProjectDocumentSection::Ptr> m_sections;
    QLinkedList<ProjectDocumentSection::Ptr> m_exceededSections;
    QList<int> bracketPositions;
    QString    blockIdentifier;

    void setCollapse(CollapseFunctionType func);
    void setReplacementString(QString& string);
    QString &replacementString();
    void setNumOfCollapsedLines(int num);
    int numOfCollapsedLines();
    CollapseFunctionType onCollapse();
    void setStateChangeFlag(bool value) {m_stateChangeFlag = value; }
    bool stateChangeFlag() {return m_stateChangeFlag; }
    bool isCollapsible() { return m_collapsable; }
    void setCollapsible(bool col) { m_collapsable = col; }
    void resetCollapseParams();

private:
    QString m_replacementString;
    int m_numOfCollapsedLines;
    CollapseFunctionType m_onCollapse;
    bool m_stateChangeFlag;
    bool m_collapsable;
};


class LV_EDITOR_EXPORT ProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::ProjectFile* file  READ file        NOTIFY fileChanged)
    Q_PROPERTY(QString content        READ content     NOTIFY contentChanged)
    Q_PROPERTY(bool isMonitored       READ isMonitored NOTIFY isMonitoredChanged)
    Q_PROPERTY(bool isDirty           READ isDirty     WRITE  setIsDirty     NOTIFY isDirtyChanged)
    Q_ENUMS(OpenMode)

public:
    /** Iterator through sections */
    typedef QLinkedList<ProjectDocumentSection::Ptr>::iterator       SectionIterator;
    /** Const iterator through sections */
    typedef QLinkedList<ProjectDocumentSection::Ptr>::const_iterator SectionConstIterator;

    friend class ProjectDocumentAction;
    friend class ProjectDocumentMarker;
    friend class ProjectDocumentSection;

    /** Enum containing possible modes of opening documents */
    enum OpenMode{
        /** The file open in the editor */
        Edit = 0,
        /** Read-only, but any external change will be reflected */
        Monitor,
        /** If not opened, will be open for editing. If already monitored, it will not be available for editing. */
        EditIfNotOpen
    };

    /** Editing states of an opened document */
    enum EditingState{
        /**     0 : coming from the user */
        Manual   = 0,
        /**     1 : coming from a code completion assistant */
        Assisted = 1,
        /**    10 : does not trigger a recompile */
        Silent   = 2,
        /**   110 : also silent (when a palette edits a section) */
        Palette  = 6,
        /**  1010 : also silent (comming from a runtime binding) */
        Runtime  = 10,
        /** 10000 : populate from file, does not signal anything */
        Read     = 16
    };

public:
    explicit ProjectDocument(ProjectFile* file, bool isMonitored, Project *parent);

    ~ProjectDocument();

    /** \brief File getter */
    lv::ProjectFile* file() const;

    /**
     * \brief Returns document content
     */
    QString content() const;

    void setIsDirty(bool isDirty);

    bool isDirty() const;

    void setIsMonitored(bool isMonitored);
    /**
     * \brief Shows if the document is monitored
     */
    bool isMonitored() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    Project* parentAsProject();

    QTextDocument* textDocument();

    ProjectDocumentMarker::Ptr addMarker(int position);
    void removeMarker(ProjectDocumentMarker::Ptr marker);

    ProjectDocumentSection::Ptr createSection(int type, int position, int length);
    SectionIterator sectionsBegin();
    SectionIterator sectionsEnd();
    SectionConstIterator sectionsBegin() const;
    SectionConstIterator sectionsEnd() const;
    int totalSections() const;
    bool hasSections() const;
    ProjectDocumentSection::Ptr sectionAt(int position);
    bool removeSectionAt(int position);
    void removeSection(ProjectDocumentSection::Ptr section);

    bool isActive() const;

    QString peekContent(int position) const;

    void addEditingState(EditingState type);
    void removeEditingState(EditingState state);
    bool editingStateIs(int flag) const;
    void resetEditingState();

public slots:
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void setContent(const QString& content);
    void readContent();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

signals:
    /** shows dirty state changed */
    void isDirtyChanged();
    /** shows if monitoring state changed */
    void isMonitoredChanged();
    /** shows if the file changed */
    void fileChanged();
    /** shows if the document content changed */
    void contentChanged();
    /** shows if the format changed */
    void formatChanged(int position, int length);

    void contentsChange(int pos, int removed, int added);

private:
    void syncContent() const;
    void resetSync() const;
    void updateSections(int position, int charsRemoved, const QString& addedText);
    void updateMarkers(int position, int charsRemoved, int addedText);
    void updateSectionBlocks(int position, const QString& addedText);
    QString getCharsRemoved(int position, int count);

    ProjectFile*    m_file;
    QDateTime       m_lastModified;

    QTextDocument*   m_textDocument;

    QLinkedList<ProjectDocumentSection::Ptr> m_sections;
    QLinkedList<ProjectDocumentMarker::Ptr>  m_markers;

    QLinkedList<ProjectDocumentSection::Ptr> m_sectionsToRemove;
    bool                                     m_iteratingSections;

    QLinkedList<ProjectDocumentAction>      m_changes;
    mutable QLinkedList<ProjectDocumentAction>::iterator m_lastChange;

    mutable int   m_editingState;
    bool          m_isDirty;
    mutable bool  m_isSynced;
    bool          m_isMonitored;
};

/**
 * \brief File getter
 */
inline ProjectFile *ProjectDocument::file() const{
    return m_file;
}

inline QString ProjectDocument::content() const{
    syncContent();
    return m_textDocument->toPlainText();
}

/**
 * \brief Sets the "dirty" indicator
 */
inline void ProjectDocument::setIsDirty(bool isDirty){
    if ( m_isDirty == isDirty )
        return;

    m_isDirty = isDirty;
    isDirtyChanged();
}

inline bool ProjectDocument::isDirty() const{
    return m_isDirty;
}

/**
 * \brief Sets the indicator for monitoring
 */
inline void ProjectDocument::setIsMonitored(bool isMonitored){
    if ( m_isMonitored == isMonitored )
        return;

    m_isMonitored = isMonitored;
    emit isMonitoredChanged();
}


inline bool ProjectDocument::isMonitored() const{
    return m_isMonitored;
}

/**
 * \brief Returns the timestamp of last modification
 */
inline const QDateTime &ProjectDocument::lastModified() const{
    return m_lastModified;
}

/**
 * \brief Sets the timestamp of latest modification
 */
inline void ProjectDocument::setLastModified(const QDateTime &lastModified){
    m_lastModified = lastModified;
}

/**
 * \brief Begin-iterator of the sections
 */
inline ProjectDocument::SectionIterator ProjectDocument::sectionsBegin(){
    return m_sections.begin();
}

/**
 * \brief End-iterator of the sections
 */
inline ProjectDocument::SectionIterator ProjectDocument::sectionsEnd(){
    return m_sections.end();
}


/**
 * \brief Const begin-iterator of the sections
 */
inline ProjectDocument::SectionConstIterator ProjectDocument::sectionsBegin() const{
    return m_sections.begin();
}


/**
 * \brief Const end-iterator of the sections
 */
inline ProjectDocument::SectionConstIterator ProjectDocument::sectionsEnd() const{
    return m_sections.end();
}

/**
 * \brief Number of sections
 */
inline int ProjectDocument::totalSections() const{
    return m_sections.size();
}

/**
 * \brief Shows if the object has any sections
 */
inline bool ProjectDocument::hasSections() const{
    return totalSections() > 0;
}

inline void ProjectDocument::resetSync() const{
    m_isSynced = false;
}

/**
 * \brief Text document which is wrapped inside the ProjectDocument
 */
inline QTextDocument *ProjectDocument::textDocument(){
    return m_textDocument;
}

/**
 * \brief Adds editing state flag
 */
inline void ProjectDocument::addEditingState(EditingState state){
    m_editingState |= state;
}

/**
 * \brief Removes the given editing state flag
 */
inline void ProjectDocument::removeEditingState(EditingState state){
    if ( m_editingState & state ){
        bool restoreSilent = editingStateIs(ProjectDocument::Palette | ProjectDocument::Runtime);
        m_editingState = m_editingState & ~state;
        if ( restoreSilent ){
            m_editingState |= ProjectDocument::Silent;
        }
    }
}

/**
 * \brief Shows if the editing state includes the given flags
 */
inline bool ProjectDocument::editingStateIs(int flag) const{
    return (flag & m_editingState) == flag;
}

/**
 * \brief Resets all of the editing state flags
 */
inline void ProjectDocument::resetEditingState(){
    m_editingState = 0;
}

}// namespace

#endif // LVPROJECTDOCUMENT_H
