#include "qmleditfragmentcontainer.h"
#include "languageqmlhandler.h"

namespace lv{

QmlEditFragmentContainer::QmlEditFragmentContainer(LanguageQmlHandler *parent)
    : QObject(parent)
    , m_codeHandler(parent)
{

}

QmlEditFragmentContainer::~QmlEditFragmentContainer(){

}

bool QmlEditFragmentContainer::addEdit(QmlEditFragment *edit){
    auto it = m_edits.begin();
    while ( it != m_edits.end() ){
        QmlEditFragment* itEdit = *it;

        if ( itEdit->declaration()->position() < edit->declaration()->position() ){
            break;

        } else if ( itEdit->declaration()->position() == edit->declaration()->position() ){
            qWarning("Multiple editing fragments at the same position: %d", edit->position());
            return false;
        }
        ++it;
    }

    m_edits.insert(it, edit);

    emit editCountChanged();
    return true;
}

void QmlEditFragmentContainer::derefEdit(QmlEditFragment *edit){
    edit->decrementRefCount();

    if (edit->refCount() == 0){
        QObject* parent = edit->parent();
        if ( parent == m_codeHandler ){
            auto it = m_edits.begin();
            while( it != m_edits.end() ){
                QmlEditFragment* itEdit = *it;

                if ( itEdit->declaration()->position() < edit->declaration()->position() )
                    break;

                if ( itEdit == edit ){
                    m_edits.erase(it);

                    for (auto child: edit->childFragments())
                        derefEdit(child);

                    emit editCountChanged();

                    //TOWORKAROUND
                    if ( m_codeHandler->m_editFragment == edit ){
                        m_codeHandler->m_document->removeEditingState(ProjectDocument::Overlay);
                        m_codeHandler->m_editFragment = nullptr;
                    }
                    edit->emitRemoval();
                    edit->deleteLater();
                    return;
                }

                ++it;
            }
        } else {
            QmlEditFragment* parentEf = static_cast<QmlEditFragment*>(parent);
            if ( parentEf ){
                parentEf->removeChildFragment(edit);
            }
        }
    }
}

void QmlEditFragmentContainer::removeEdit(QmlEditFragment *edit){
    if (!edit)
        return;

    QObject* parent = edit->parent();
    if ( parent == m_codeHandler ){
        auto it = m_edits.begin();

        while( it != m_edits.end() ){
            QmlEditFragment* itEdit = *it;

            if ( itEdit->declaration()->position() < edit->declaration()->position() )
                break;

            if ( itEdit == edit ){
                m_edits.erase(it);

                for (auto child: edit->childFragments())
                    removeEdit(child);
                emit editCountChanged();

                //TOWORKAROUND
                if ( m_codeHandler->m_editFragment == edit ){
                    m_codeHandler->m_document->removeEditingState(ProjectDocument::Overlay);
                    m_codeHandler->m_editFragment = nullptr;
                }
                edit->emitRemoval();
                edit->deleteLater();
                return;
            }

            ++it;
        }
    } else {
        QmlEditFragment* parentEf = static_cast<QmlEditFragment*>(parent);
        if ( parentEf ){
            parentEf->removeChildFragment(edit);
        }
    }
}

QJSValue QmlEditFragmentContainer::allEdits(){
    QJSValue result = m_codeHandler->m_engine->engine()->newArray(static_cast<quint32>(m_edits.size()));
    quint32 i = 0;
    for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
        result.setProperty(i++, m_codeHandler->m_engine->engine()->newQObject(*it));
    }
    return result;
}

void QmlEditFragmentContainer::clearAllFragments(){
    auto it = m_edits.begin();
    while( it != m_edits.end() ){
        QmlEditFragment* edit = *it;
        it = m_edits.erase(it);
        edit->emitRemoval();
        edit->deleteLater();
    }
    emit editCountChanged();
}

QmlEditFragment *QmlEditFragmentContainer::topEditAtPosition(int position){
    for ( auto it = m_edits.begin(); it != m_edits.end(); ++it ){
        QmlEditFragment* edit = *it;
        if ( edit->declaration()->position() == position ){
            return edit;
        }
    }
    return nullptr;
}

QmlEditFragment *QmlEditFragmentContainer::findObjectFragmentByPosition(int position){
    QmlEditFragment* result = nullptr;
    QLinkedList<QmlEditFragment*> q;
    for (auto it = m_edits.begin(); it != m_edits.end(); ++it)
    {
        q.push_back(*it);
    }

    while (!q.empty())
    {
        auto edit = q.front(); q.pop_front();
        int lower = edit->declaration()->position();
        int upper = lower + edit->declaration()->length();
        if (edit->declaration()->isForObject() && position >= lower && position <= upper)
        {
            q.clear();
            result = edit;
            for (auto it = edit->childFragments().begin(); it != edit->childFragments().end(); ++it)
                q.push_back(*it);
        }

    }
    return result;
}

QmlEditFragment *QmlEditFragmentContainer::findFragmentByPosition(int position){
    QmlEditFragment* result = nullptr;

    QLinkedList<QmlEditFragment*> q;
    for (auto it = m_edits.begin(); it != m_edits.end(); ++it)
    {
        q.push_back(*it);
    }

    while (!q.empty())
    {
        auto edit = q.front(); q.pop_front();
        int lower = edit->declaration()->position();
        int upper = lower + edit->declaration()->length();
        if (position >= lower && position <= upper) //tbd
        {
            q.clear();
            result = edit;

            auto childFragments = edit->childFragments();

            for (auto it = childFragments.begin(); it != childFragments.end(); ++it){
                q.push_back(*it);
            }
        }

    }
    return result;
}

}// namespace
