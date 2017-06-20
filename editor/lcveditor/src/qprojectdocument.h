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

#ifndef QPROJECTDOCUMENT_H
#define QPROJECTDOCUMENT_H

#include <QObject>
#include <QDateTime>
#include <QLinkedList>
#include <QAbstractUndoItem>
#include <QTextBlockUserData>
#include "qlcveditorglobal.h"
#include "qcodedeclaration.h"

namespace lcv{

class QProject;
class QProjectFile;
class QProjectDocument;
class QProjectDocumentBlockData;
class QDocumentHandler;
class QCodeConverter;
class QCodeRuntimeBinding;

class Q_LCVEDITOR_EXPORT QProjectDocumentAction : public QAbstractUndoItem{

public:
    QProjectDocumentAction(int pPosition, const QString& pCharsAdded, const QString& pCharsRemoved, bool pCommited)
        : charsAdded(pCharsAdded)
        , charsRemoved(pCharsRemoved)
        , position(pPosition)
        , commited(pCommited)
    {
    }

    void undo();
    void redo();

    QString charsAdded;
    QString charsRemoved;
    int     position;
    bool    commited;
};

class Q_LCVEDITOR_EXPORT QProjectDocumentBlockData : public QTextBlockUserData{

public:
    QProjectDocumentBlockData() : exceededBindingLength(0){}
    ~QProjectDocumentBlockData();

    void addBinding(QCodeRuntimeBinding* binding);
    void removeBinding(QCodeRuntimeBinding* binding);

    QLinkedList<QCodeRuntimeBinding*> m_bindings;
    QList<int> bracketPositions;
    QString    blockIdentifier;

    int exceededBindingLength;
};

class Q_LCVEDITOR_EXPORT QProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectFile* file    READ file        NOTIFY fileChanged)
    Q_PROPERTY(QString            content READ content     NOTIFY contentChanged)
    Q_PROPERTY(bool isMonitored           READ isMonitored NOTIFY isMonitoredChanged)
    Q_PROPERTY(bool isDirty               READ isDirty     WRITE setIsDirty     NOTIFY isDirtyChanged)
    Q_ENUMS(OpenMode)

public:
    typedef QLinkedList<QCodeRuntimeBinding*>::iterator BindingIterator;

    enum OpenMode{
        Edit = 0,
        Monitor,
        EditIfNotOpen
    };

public:
    explicit QProjectDocument(QProjectFile* file, bool isMonitored, QProject *parent);
    ~QProjectDocument();

    lcv::QProjectFile* file() const;

    const QString& content() const;

    void setIsDirty(bool isDirty);
    bool isDirty() const;

    void setIsMonitored(bool isMonitored);
    bool isMonitored() const;

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    QProject* parentAsProject();

    void assignEditingDocument(QTextDocument* doc, QDocumentHandler* handler);
    QTextDocument* editingDocument();
    QCodeRuntimeBinding* addNewBinding(QCodeDeclaration::Ptr declaration);
    void documentContentsChanged(int position, int charsRemoved, const QString& addedText = "");
    void documentContentsSilentChanged(int position, int charsRemoved, const QString& addedText = "");

    BindingIterator bindingsBegin();
    BindingIterator bindingsEnd();
    int  totalBindings() const;
    bool hasBindings() const;
    QCodeRuntimeBinding* bindingAt(int position);
    bool removeBindingAt(int position);

    void updateBindingValue(QCodeRuntimeBinding* binding, const QString &value);

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
    void contentChanged();

private:
    void updateBindings(int position, int charsRemoved, const QString& addedText);
    void updateBindingBlocks(int position, const QString &addedText);

    QProjectFile* m_file;
    QString       m_content;
    QDateTime     m_lastModified;

    QTextDocument*    m_editingDocument;
    QDocumentHandler* m_editingDocumentHandler;

    QLinkedList<QCodeRuntimeBinding*> m_bindings;

    QLinkedList<QProjectDocumentAction>   m_changes;

    bool          m_isDirty;
    bool          m_isMonitored;
};

inline QProjectFile *QProjectDocument::file() const{
    return m_file;
}

inline const QString &QProjectDocument::content() const{
    return m_content;
}

inline void QProjectDocument::setIsDirty(bool isDirty){
    if ( m_isDirty == isDirty )
        return;

    m_isDirty = isDirty;
    isDirtyChanged();
}

inline bool QProjectDocument::isDirty() const{
    return m_isDirty;
}

inline void QProjectDocument::setIsMonitored(bool isMonitored){
    if ( m_isMonitored == isMonitored )
        return;

    m_isMonitored = isMonitored;
    emit isMonitoredChanged();
}

inline bool QProjectDocument::isMonitored() const{
    return m_isMonitored;
}

inline const QDateTime &QProjectDocument::lastModified() const{
    return m_lastModified;
}

inline void QProjectDocument::setLastModified(const QDateTime &lastModified){
    m_lastModified = lastModified;
}

inline QProjectDocument::BindingIterator QProjectDocument::bindingsBegin(){
    return m_bindings.begin();
}

inline QProjectDocument::BindingIterator QProjectDocument::bindingsEnd(){
    return m_bindings.end();
}

inline int QProjectDocument::totalBindings() const{
    return m_bindings.size();
}

inline bool QProjectDocument::hasBindings() const{
    return !m_bindings.isEmpty();
}

inline QTextDocument *QProjectDocument::editingDocument(){
    return m_editingDocument;
}

}// namespace

#endif // QPROJECTDOCUMENT_H
