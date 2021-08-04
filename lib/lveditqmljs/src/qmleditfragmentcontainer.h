#ifndef LVQMLEDITFRAGMENTCONTAINER_H
#define LVQMLEDITFRAGMENTCONTAINER_H

#include <QObject>
#include <QLinkedList>
#include <QJSValue>

#include "lveditqmljsglobal.h"

namespace lv{

class QmlEditFragment;
class LanguageQmlHandler;

class LV_EDITQMLJS_EXPORT QmlEditFragmentContainer : public QObject{

    Q_OBJECT
    Q_PROPERTY(int editCount READ editCount NOTIFY editCountChanged)

public:
    friend class LanguageQmlHandler;

public:
    explicit QmlEditFragmentContainer(LanguageQmlHandler *parent = nullptr);
    ~QmlEditFragmentContainer() override;

    int editCount() const;

public slots:
    bool addEdit(lv::QmlEditFragment* edit);
    void derefEdit(lv::QmlEditFragment* edit);
    void removeEdit(lv::QmlEditFragment* edit);
    QJSValue allEdits();
    void clearAllFragments();

    lv::QmlEditFragment* topEditAtPosition(int position);

    //TOMOVE
    lv::QmlEditFragment* findObjectFragmentByPosition(int position);
    lv::QmlEditFragment* findFragmentByPosition(int position);

signals:
    void editCountChanged();

private:
    QLinkedList<QmlEditFragment*> m_edits; // opened fragments
    LanguageQmlHandler*               m_codeHandler;
};

inline int QmlEditFragmentContainer::editCount() const{
    return m_edits.size();
}

}// namespace

#endif // LVQMLEDITFRAGMENTCONTAINER_H
