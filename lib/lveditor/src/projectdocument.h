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

#ifndef LVPROJECTDOCUMENT_H
#define LVPROJECTDOCUMENT_H

#include <QObject>
#include <QDateTime>
#include <QLinkedList>
#include <QAbstractUndoItem>
#include <QTextBlockUserData>

#include "live/lveditorglobal.h"
#include "live/codedeclaration.h"

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
    void invalidate(){ m_position = -1; }

private:
    int m_position;
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

class LV_EDITOR_EXPORT ProjectDocumentBlockData : public QTextBlockUserData{

public:
    ProjectDocumentBlockData() : exceededBindingLength(0){}
    ~ProjectDocumentBlockData();

    void addBinding(CodeRuntimeBinding* binding);
    void removeBinding(CodeRuntimeBinding* binding);

    QLinkedList<CodeRuntimeBinding*> m_bindings;
    QList<int> bracketPositions;
    QString    blockIdentifier;

    int exceededBindingLength;
};

class LV_EDITOR_EXPORT ProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::ProjectFile* file  READ file        NOTIFY fileChanged)
    Q_PROPERTY(QString content        READ content     NOTIFY contentChanged)
    Q_PROPERTY(bool isMonitored       READ isMonitored NOTIFY isMonitoredChanged)
    Q_PROPERTY(bool isDirty           READ isDirty     WRITE  setIsDirty     NOTIFY isDirtyChanged)
    Q_ENUMS(OpenMode)

public:
    typedef QLinkedList<CodeRuntimeBinding*>::iterator BindingIterator;

    friend class ProjectDocumentAction;
    friend class ProjectDocumentMarker;

    enum OpenMode{
        Edit = 0,
        Monitor,
        EditIfNotOpen
    };

public:
    explicit ProjectDocument(ProjectFile* file, bool isMonitored, Project *parent);
    ~ProjectDocument();

    lv::ProjectFile* file() const;

    const QString& content() const;

    void setIsDirty(bool isDirty);
    bool isDirty() const;

    void setIsMonitored(bool isMonitored);
    bool isMonitored() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    Project* parentAsProject();

    void assignEditingDocument(QTextDocument* doc, DocumentHandler* handler);
    QTextDocument* editingDocument();
    CodeRuntimeBinding* addNewBinding(CodeDeclaration::Ptr declaration);

    void documentContentsChanged(
        DocumentHandler* author,
        int position,
        int charsRemoved,
        const QString& addedText = "");
    void documentContentsSilentChanged(
        DocumentHandler* author,
        int position,
        int charsRemoved,
        const QString& addedText = ""
    );

    ProjectDocumentMarker::Ptr addMarker(int position);
    void removeMarker(ProjectDocumentMarker::Ptr marker);

    BindingIterator bindingsBegin();
    BindingIterator bindingsEnd();
    int  totalBindings() const;
    bool hasBindings() const;
    CodeRuntimeBinding* bindingAt(int position);
    bool removeBindingAt(int position);

    void updateBindingValue(CodeRuntimeBinding* binding, const QString &value);

    bool isActive() const;

public slots:
    void dumpContent(const QString& content);
    void readContent();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

signals:
    void isDirtyChanged();
    void isMonitoredChanged();
    void fileChanged();
    void contentChanged(QObject* author);

private:
    void syncContent() const;
    void resetSync() const;
    void updateBindings(int position, int charsRemoved, const QString& addedText);
    void updateMarkers(int position, int charsRemoved, int addedText);
    void updateBindingBlocks(int position, const QString &addedText);
    QString getCharsRemoved(int position, int count);

    ProjectFile*    m_file;
    mutable QString m_content;
    QDateTime       m_lastModified;

    QTextDocument*   m_editingDocument;
    DocumentHandler* m_editingDocumentHandler;

    QLinkedList<CodeRuntimeBinding*>        m_bindings;
    QLinkedList<ProjectDocumentMarker::Ptr> m_markers;

    QLinkedList<ProjectDocumentAction>      m_changes;
    mutable QLinkedList<ProjectDocumentAction>::iterator m_lastChange;

    bool          m_isDirty;
    mutable bool  m_isSynced;
    bool          m_isMonitored;
};

inline ProjectFile *ProjectDocument::file() const{
    return m_file;
}

inline const QString &ProjectDocument::content() const{
    syncContent();
    return m_content;
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

inline void ProjectDocument::resetSync() const{
    m_isSynced = false;
}

inline QTextDocument *ProjectDocument::editingDocument(){
    return m_editingDocument;
}

}// namespace

#endif // LVPROJECTDOCUMENT_H
