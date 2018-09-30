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
class CodeConverter;
class CodeRuntimeBinding;

class LV_EDITOR_EXPORT ProjectDocumentMarker{

public:
    friend class ProjectDocument;
    typedef QSharedPointer<ProjectDocumentMarker>       Ptr;
    typedef QSharedPointer<const ProjectDocumentMarker> ConstPtr;

public:
    int position() const{ return m_position; }
    bool isValid() const{ return m_position != -1; }
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
    typedef QSharedPointer<ProjectDocumentSection>       Ptr;
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

class LV_EDITOR_EXPORT ProjectDocumentAction : public QAbstractUndoItem{

public:
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

class LV_EDITOR_EXPORT ProjectDocumentBlockData : public QTextBlockUserData{

public:
    enum CollapseState {NoCollapse, Collapse, Expand};

    ProjectDocumentBlockData();
    ~ProjectDocumentBlockData();

    void addSection(ProjectDocumentSection::Ptr section);
    void removeSection(ProjectDocumentSection::Ptr section);
    void removeSection(ProjectDocumentSection* section);

    QLinkedList<ProjectDocumentSection::Ptr> m_sections;
    QLinkedList<ProjectDocumentSection::Ptr> m_exceededSections;
    QList<int> bracketPositions;
    QString    blockIdentifier;

    void setCollapse(CollapseState state, CollapseFunctionType func);
    CollapseState collapseState();
    void setReplacementString(QString& string);
    QString &replacementString();
    void setNumOfCollapsedLines(int num);
    int numOfCollapsedLines();
    CollapseFunctionType onCollapse();
    void setStateChangeFlag(bool value) {m_stateChangeFlag = value; }
    bool stateChangeFlag() {return m_stateChangeFlag; }
    void collapse() {m_collapseState = Expand; m_stateChangeFlag = true; }
    void expand() { m_collapseState = Collapse; m_stateChangeFlag = true; }

    void resetCollapseParams();

private:
    CollapseState m_collapseState;
    QString m_replacementString;
    int m_numOfCollapsedLines;
    CollapseFunctionType m_onCollapse;
    bool m_stateChangeFlag;
};


class LV_EDITOR_EXPORT ProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::ProjectFile* file  READ file        NOTIFY fileChanged)
    Q_PROPERTY(QString content        READ content     NOTIFY contentChanged)
    Q_PROPERTY(bool isMonitored       READ isMonitored NOTIFY isMonitoredChanged)
    Q_PROPERTY(bool isDirty           READ isDirty     WRITE  setIsDirty     NOTIFY isDirtyChanged)
    Q_ENUMS(OpenMode)

public:
    typedef QLinkedList<CodeRuntimeBinding*>::iterator               BindingIterator;
    typedef QLinkedList<ProjectDocumentSection::Ptr>::iterator       SectionIterator;
    typedef QLinkedList<ProjectDocumentSection::Ptr>::const_iterator SectionConstIterator;

    friend class ProjectDocumentAction;
    friend class ProjectDocumentMarker;
    friend class ProjectDocumentSection;

    enum OpenMode{
        Edit = 0,
        Monitor,
        EditIfNotOpen
    };

    enum EditingState{
        Manual   = 0, //     0 : coming from the user
        Assisted = 1, //     1 : coming from a code completion assistant
        Silent   = 2, //    10 : does not trigger a recompile
        Palette  = 6, //   110 : also silent (when a palette edits a section)
        Runtime  = 10,//  1010 : also silent (comming from a runtime binding)
        Read     = 16 // 10000 : populate from file, does not signal anything
    };

public:
    explicit ProjectDocument(ProjectFile* file, bool isMonitored, Project *parent);
    ~ProjectDocument();

    lv::ProjectFile* file() const;

    QString content() const;

    void setIsDirty(bool isDirty);
    bool isDirty() const;

    void setIsMonitored(bool isMonitored);
    bool isMonitored() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    Project* parentAsProject();

    void assignDocumentHandler(DocumentHandler* handler);
    QTextDocument* textDocument();

    ProjectDocumentMarker::Ptr addMarker(int position);
    void removeMarker(ProjectDocumentMarker::Ptr marker);

    bool addNewBinding(CodeRuntimeBinding* binding);
    BindingIterator bindingsBegin();
    BindingIterator bindingsEnd();
    int  totalBindings() const;
    bool hasBindings() const;
    CodeRuntimeBinding* bindingAt(int position);
    bool removeBindingAt(int position);
    void updateBindingValue(CodeRuntimeBinding* binding, const QString &value);

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

    void addEditingState(EditingState type);
    void removeEditingState(EditingState state);
    bool editingStateIs(int flag) const;
    void resetEditingState();

public slots:
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void resetContent(const QString& content);
    void readContent();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

signals:
    void isDirtyChanged();
    void isMonitoredChanged();
    void fileChanged();
    void contentChanged();

private:
    void syncContent() const;
    void resetSync() const;
    void updateSections(int position, int charsRemoved, const QString& addedText);
    void updateMarkers(int position, int charsRemoved, int addedText);
    void updateSectionBlocks(int position, const QString& addedText);
    QString getCharsRemoved(int position, int count);

    ProjectFile*    m_file;
    // mutable QString m_content;
    QDateTime       m_lastModified;

    QTextDocument*   m_textDocument;
    DocumentHandler* m_editingDocumentHandler;

    QLinkedList<CodeRuntimeBinding*>         m_bindings;
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

inline ProjectFile *ProjectDocument::file() const{
    return m_file;
}

inline QString ProjectDocument::content() const{
    syncContent();
    return m_textDocument->toPlainText();
}

inline void ProjectDocument::setIsDirty(bool isDirty){
    if ( m_isDirty == isDirty )
        return;

    m_isDirty = isDirty;
    isDirtyChanged();
}

inline bool ProjectDocument::isDirty() const{
    return m_isDirty;
}

inline void ProjectDocument::setIsMonitored(bool isMonitored){
    if ( m_isMonitored == isMonitored )
        return;

    m_isMonitored = isMonitored;
    emit isMonitoredChanged();
}

inline bool ProjectDocument::isMonitored() const{
    return m_isMonitored;
}

inline const QDateTime &ProjectDocument::lastModified() const{
    return m_lastModified;
}

inline void ProjectDocument::setLastModified(const QDateTime &lastModified){
    m_lastModified = lastModified;
}

inline ProjectDocument::BindingIterator ProjectDocument::bindingsBegin(){
    return m_bindings.begin();
}

inline ProjectDocument::BindingIterator ProjectDocument::bindingsEnd(){
    return m_bindings.end();
}

inline int ProjectDocument::totalBindings() const{
    return m_bindings.size();
}

inline bool ProjectDocument::hasBindings() const{
    return !m_bindings.isEmpty();
}

inline ProjectDocument::SectionIterator ProjectDocument::sectionsBegin(){
    return m_sections.begin();
}

inline ProjectDocument::SectionIterator ProjectDocument::sectionsEnd(){
    return m_sections.end();
}

inline ProjectDocument::SectionConstIterator ProjectDocument::sectionsBegin() const{
    return m_sections.begin();
}

inline ProjectDocument::SectionConstIterator ProjectDocument::sectionsEnd() const{
    return m_sections.end();
}

inline int ProjectDocument::totalSections() const{
    return m_sections.size();
}

inline bool ProjectDocument::hasSections() const{
    return totalSections() > 0;
}

inline void ProjectDocument::resetSync() const{
    m_isSynced = false;
}

inline QTextDocument *ProjectDocument::textDocument(){
    return m_textDocument;
}

inline void ProjectDocument::addEditingState(EditingState state){
    m_editingState |= state;
}

inline void ProjectDocument::removeEditingState(EditingState state){
    if ( m_editingState & state ){
        bool restoreSilent = editingStateIs(ProjectDocument::Palette | ProjectDocument::Runtime);
        m_editingState = m_editingState & ~state;
        if ( restoreSilent ){
            m_editingState |= ProjectDocument::Silent;
        }
    }
}

inline bool ProjectDocument::editingStateIs(int flag) const{
    return (flag & m_editingState) == flag;
}

inline void ProjectDocument::resetEditingState(){
    m_editingState = 0;
}

}// namespace

#endif // LVPROJECTDOCUMENT_H
