#ifndef QPROJECTDOCUMENT_H
#define QPROJECTDOCUMENT_H

#include <QObject>
#include <QDateTime>
#include <QLinkedList>
#include <QAbstractUndoItem>
#include <QTextBlockUserData>
#include "qlcveditorglobal.h"

#include <QQmlProperty>

namespace lcv{

class QProject;
class QProjectFile;
class QProjectDocument;
class QProjectDocumentBlockData;
class QDocumentCodeInterface;

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


class Q_LCVEDITOR_EXPORT QProjectDocumentBinding : public QObject{

    Q_OBJECT

public:
    QProjectDocumentBinding(QProjectDocument* parent = 0);
    ~QProjectDocumentBinding();

    int propertyPosition;
    int propertyLength;

    int valuePositionOffset;
    int valueLength;

    bool modifiedByEngine;

    int length() const{ return propertyLength + valuePositionOffset + valueLength; }

    QStringList                propertyChain;
    QProjectDocumentBlockData* parentBlock;

public slots:
    void updateValue();

private:
    QProjectDocument* m_document;

};

class Q_LCVEDITOR_EXPORT QProjectDocumentBlockData : public QTextBlockUserData{

public:
    QProjectDocumentBlockData() : exceededBindingLength(0){}
    ~QProjectDocumentBlockData();

    void addBinding(QProjectDocumentBinding* binding);
    void removeBinding(QProjectDocumentBinding* binding);

    QLinkedList<QProjectDocumentBinding*> m_bindings;
    QList<int> bracketPositions;
    QString    blockIdentifier;

    int exceededBindingLength;
};

class Q_LCVEDITOR_EXPORT QProjectDocument : public QObject{

    Q_OBJECT
    Q_PROPERTY(lcv::QProjectFile* file    READ file        CONSTANT)
    Q_PROPERTY(QString            content READ content     NOTIFY contentChanged)
    Q_ENUMS(OpenMode)

public:
    typedef QLinkedList<QProjectDocumentBinding*>::iterator BindingIterator;

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

    const QDateTime& lastModified() const;
    void setLastModified(const QDateTime& lastModified);

    QProject* parentAsProject();

    void assignEditingDocument(QTextDocument* doc, QDocumentCodeInterface* handler);
    QTextDocument* editingDocument();
    QProjectDocumentBinding* addNewBinding(int position, int length, const QStringList& propertyChain);
    void documentContentsChanged(int position, int charsRemoved, const QString& addedText = "");
    void documentContentsSilentChanged(int position, int charsRemoved, const QString& addedText = "");

    BindingIterator bindingsBegin();
    BindingIterator bindingsEnd();
    int  totalBindings() const;
    bool hasBindings() const;
    QProjectDocumentBinding* bindingAt(int position);
    bool removeBindingAt(int position);

    void updateBindingValue(QProjectDocumentBinding* binding, const QString &value);

    bool isActive() const;

public slots:
    void dumpContent(const QString& content);
    void readContent();
    bool save();
    bool saveAs(const QString& path);
    bool saveAs(const QUrl& url);

signals:
    void contentChanged();

private:
    void updateBindings(int position, int charsRemoved, const QString& addedText);
    void updateBindingBlocks(int position, const QString &addedText);

    QProjectFile* m_file;
    QString       m_content;
    QDateTime     m_lastModified;

    QTextDocument*          m_editingDocument;
    QDocumentCodeInterface* m_editingDocumentHandler;

    QLinkedList<QProjectDocumentBinding*> m_bindings;

    QLinkedList<QProjectDocumentAction>   m_changes;
};

inline QProjectFile *QProjectDocument::file() const{
    return m_file;
}

inline const QString &QProjectDocument::content() const{
    return m_content;
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
