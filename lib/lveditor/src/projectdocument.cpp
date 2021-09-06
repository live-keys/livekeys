/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/lockedfileiosession.h"
#include "live/codehandler.h"
#include "live/project.h"
#include "live/visuallog.h"
#include "textdocumentlayout.h"

#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QFileInfo>

/**
 * \class lv::ProjectDocumentMarker
 * \brief Dynamic project document markers
 *
 * Markers are wrappers around a position in a document. They get moved automatically with document changes, and get
 * deleted when both characters surrounding them are deleted.
 * \ingroup lveditor
 */

/**
 * \fn lv::ProjectDocumentMarker::position
 * \brief Marker position
 */

/**
 * \fn lv::ProjectDocumentMarker::isValid
 * \brief Shows if the marker is valid
 */

/**
 * \fn lv::ProjectDocumentMarker::create
 * \brief Create a ProjectDocumentMarker
 */

/**
 * \fn lv::ProjectDocumentMarker::~ProjectDocumentMarker
 * \brief Destructor
 */



/**
 * \class lv::ProjectDocument
 * \brief Wrapper for any opened document in LiveKeys
 *
 *
 * \ingroup lveditor
 */
namespace lv{

class ProjectDocumentPrivate {
public:
    QTextDocument*                                      textDocument;

    std::list<ProjectDocumentSection::Ptr>              sections;
    std::list<ProjectDocumentMarker::Ptr>               markers;

    std::list<ProjectDocumentSection::Ptr>              sectionsToRemove;
    bool                                                iteratingSections;

    std::list<ProjectDocumentAction>                    changes;
    mutable std::list<ProjectDocumentAction>::iterator  lastChange;

    QString                                             contentString;
    bool                                                contentStringDirty;

    mutable int                                         editingState;
    mutable bool                                        isSynced;
    int                                                 lastCursorPosition;
};

/**
 * \brief Begin-iterator of the sections
 */
ProjectDocument::SectionIterator ProjectDocument::sectionsBegin(){
    return d_ptr->sections.begin();
}

/**
 * \brief End-iterator of the sections
 */
ProjectDocument::SectionIterator ProjectDocument::sectionsEnd(){
    return d_ptr->sections.end();
}


/**
 * \brief Const begin-iterator of the sections
 */
ProjectDocument::SectionConstIterator ProjectDocument::sectionsBegin() const{
    return d_ptr->sections.begin();
}


/**
 * \brief Const end-iterator of the sections
 */
ProjectDocument::SectionConstIterator ProjectDocument::sectionsEnd() const{
    return d_ptr->sections.end();
}

/**
 * \brief Number of sections
 */
int ProjectDocument::totalSections() const{
    return static_cast<int>(d_ptr->sections.size());
}


/**
 * \brief Shows if the object has any sections
 */
bool ProjectDocument::hasSections() const{
    return totalSections() > 0;
}


void ProjectDocument::resetSync() const{
    d_ptr->isSynced = false;
}

/**
 * \brief Text document which is wrapped inside the ProjectDocument
 */
QTextDocument *ProjectDocument::textDocument(){
    return d_ptr->textDocument;
}

/**
 * \brief Adds editing state flag
 */
void ProjectDocument::addEditingState(EditingState state){
    d_ptr->editingState |= state;
}

/**
 * \brief Removes the given editing state flag
 */
void ProjectDocument::removeEditingState(EditingState state){
    if ( d_ptr->editingState & state ){
        bool restoreSilent = editingStateIs(ProjectDocument::Palette | ProjectDocument::Runtime);
        d_ptr->editingState = d_ptr->editingState & ~state;
        if ( restoreSilent ){
            d_ptr->editingState |= ProjectDocument::Silent;
        }
    }
}

/**
 * \brief Shows if the editing state includes the given flags
 */
bool ProjectDocument::editingStateIs(int flag) const{
    return (flag & d_ptr->editingState) == flag;
}

/**
 * \brief Resets all of the editing state flags
 */
void ProjectDocument::resetEditingState(){
    d_ptr->editingState = 0;
}

/**
  Default constructor
*/
ProjectDocument::ProjectDocument(ProjectFile *file, bool isMonitored, Project *parent)
    : Document(file, parent)
    , d_ptr(new ProjectDocumentPrivate)
{
    d_ptr->textDocument = new QTextDocument(this);
    d_ptr->iteratingSections = false;
    d_ptr->lastChange = d_ptr->changes.end();
    d_ptr->editingState = 0;
    d_ptr->isSynced = true;
    d_ptr->lastCursorPosition = -1;
    d_ptr->contentStringDirty = true;
    setIsMonitored(isMonitored);
    d_ptr->textDocument->setDocumentMargin(0);
    d_ptr->textDocument->setDocumentLayout(new TextDocumentLayout(d_ptr->textDocument));
    readContent();
    connect(d_ptr->textDocument, &QTextDocument::contentsChange,      this, &ProjectDocument::__documentContentsChanged);
    connect(d_ptr->textDocument, &QTextDocument::modificationChanged, this, &ProjectDocument::__documentModificationChanged);
    connect(this,                &Document::saved,                    this, &ProjectDocument::__documentSaved);
}

void ProjectDocument::readContent(){
    if ( file()->exists() ){
        addEditingState(ProjectDocument::Read);
        d_ptr->textDocument->setPlainText(
            QString::fromStdString(parentAsProject()->lockedFileIO()->readFromFile(file()->path().toStdString()))
        );
        removeEditingState(ProjectDocument::Read);
        d_ptr->textDocument->setModified(false);
        setLastModified(QFileInfo(file()->path()).lastModified());
        d_ptr->changes.clear();
        d_ptr->lastChange = d_ptr->changes.end();
        d_ptr->isSynced = true;
        emit contentChanged();
    }
}

int ProjectDocument::contentLength(){
    return d_ptr->textDocument->characterCount();
}

/**
 * @brief ProjectDocument::updateSections
 *
 * Sections are iterated in descending order from their position. If the text between a section changes,
 * the textChanged handler is notified. If the handler requests an invalidation, invalidations are parsed
 * at the end of the iteration, by looking up the m_setctionsToRemove list.
 * @param position
 * @param charsRemoved
 * @param addedText
 */
void ProjectDocument::updateSections(int position, int charsRemoved, const QString &addedText){
    if ( d_ptr->sections.empty() )
        return;

    // Invalidation scenario

    d_ptr->iteratingSections = true;

    int charsAdded = addedText.length();

    ProjectDocument::SectionIterator it = d_ptr->sections.begin();
    while( it != d_ptr->sections.end() ){
        ProjectDocumentSection::Ptr& section = *it;
        if ( section->position() + section->length() <= position ){
            ++it;
            continue;
        }

        int sectionPosition = section->position();

        // notify sections that have removed characters inside them
        if ( section->isValid() ){
            if ( charsRemoved > 0 &&
                 position + charsRemoved > section->position() &&
                 position < section->position() + section->length())
            {
                if ( section->m_textChangedHandler )
                    section->m_textChangedHandler(section, position, charsRemoved, addedText);
                else
                    section->invalidate();

            } else if ( charsAdded > 0 &&
                        position > section->position() &&
                        position <= section->position() + section->length() )
            {
                if ( section->m_textChangedHandler )
                    section->m_textChangedHandler(section, position, charsRemoved, addedText);
                else
                    section->invalidate();

            } else {
                // update other bindings positions
                section->m_position = section->position() - charsRemoved + charsAdded;

                if ( charsRemoved > 0 && d_ptr->textDocument && !section->parentBlock() ){
                    QTextBlock block = d_ptr->textDocument->findBlock(section->position());
                    ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(block.userData());
                    if ( !bd ){
                        bd = new ProjectDocumentBlockData;
                        block.setUserData(bd);
                    }
                    bd->addSection(section);
                }
            }
        }

        if ( !section->isValid() ){

            it = d_ptr->sections.erase(it);

            QTextBlock tb = d_ptr->textDocument->findBlock(sectionPosition);
            emit formatChanged(tb.position(), tb.length());
        } else {
            ++it;
        }
    }

    d_ptr->iteratingSections = false;

    while ( !d_ptr->sectionsToRemove.empty() ){
        ProjectDocumentSection::Ptr se = d_ptr->sectionsToRemove.back();
        d_ptr->sectionsToRemove.pop_back();
        removeSection(se);
    }

    updateSectionBlocks(position, addedText);

}

void ProjectDocument::updateMarkers(int position, int charsRemoved, int charsAdded){
    if ( d_ptr->markers.empty() )
        return;

    auto it = d_ptr->markers.begin();
    while ( it != d_ptr->markers.end() ){
        ProjectDocumentMarker::Ptr& marker = *it;
        if ( marker->m_position <= position )
            break;

        if ( charsRemoved > 0 && marker->position() <= position + charsRemoved ){
            marker->invalidate();
            it = d_ptr->markers.erase(it);
        } else {
            marker->m_position = marker->m_position - charsRemoved + charsAdded;
        }
        ++it;
    }
}

void ProjectDocument::updateSectionBlocks(int position, const QString &addedText){
    QTextBlock block = d_ptr->textDocument->findBlock(position);
    QTextBlock otherBlock = d_ptr->textDocument->findBlock(position + addedText.length());
    if ( d_ptr->sections.size() > 0 &&
         d_ptr->textDocument &&
       // ( addedText.contains(QChar(QChar::ParagraphSeparator)) ||
       //  addedText.contains(QChar(QChar::LineFeed))))
        block.blockNumber() != otherBlock.blockNumber())
    {
        int blockEnd = block.position() + block.length();

        // iterate current block, see if it has any binds, check if binds are before the end of the block
        ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(block.userData());

        if ( bd && bd->m_sections->size() > 0 ){
            ProjectDocumentBlockData* bddestination = nullptr;
            QTextBlock destinationBlock;

            auto seit = bd->m_sections->begin();
            while ( seit != bd->m_sections->end() ){
                ProjectDocumentSection::Ptr& itSection = *seit;

                if ( itSection->position() >= blockEnd ){
                    if ( !bddestination ){
                        destinationBlock = d_ptr->textDocument->findBlock(position + addedText.length());
                        bddestination = static_cast<ProjectDocumentBlockData*>(destinationBlock.userData());
                        if ( !bddestination ){
                            bddestination = new ProjectDocumentBlockData;
                            destinationBlock.setUserData(bddestination);
                        }
                    }
                    ProjectDocumentSection::Ptr nuPtr = ProjectDocumentSection::create(itSection->type(), itSection->position(), itSection->length());
                    nuPtr->m_document = itSection->m_document;
                    nuPtr->m_userData = itSection->m_userData;
                    nuPtr->m_textChangedHandler = itSection->m_textChangedHandler;
                    bddestination->addSection(nuPtr);
                    seit = bd->m_sections->erase(seit);
                    emit formatChanged(destinationBlock.position(), destinationBlock.length());
                    continue;
                }
                ++seit;
            }
        }
    }
}

QString ProjectDocument::getCharsRemoved(int position, int count){
    if ( count > 0 ){
        syncContent();
        QTextCursor c(d_ptr->textDocument);
        c.setPosition(position);
        c.setPosition(position + count, QTextCursor::MoveMode::KeepAnchor);
        return c.selectedText();
    }
    return "";
}

/**
 * \brief Adds a marker at the given position
 */
ProjectDocumentMarker::Ptr ProjectDocument::addMarker(int position){
    // markers are added in descending order according to their position
    auto it = d_ptr->markers.begin();
    while( it != d_ptr->markers.end() ){
        if ( (*it)->position() <= position )
            break;
        ++it;
    }
    ProjectDocumentMarker::Ptr result(new ProjectDocumentMarker(position));
    d_ptr->markers.insert(it, result);
    return result;
}

/**
 * \brief Removes a given marker
 */
void ProjectDocument::removeMarker(ProjectDocumentMarker::Ptr marker){
    auto it = d_ptr->markers.begin();
    while ( it != d_ptr->markers.end() ){
        if ( (*it).data() == marker.data() ){
            d_ptr->markers.erase(it);
            return;
        }
        ++it;
    }
}

/**
  \brief Creates a custom section with the given type, position and length

  \sa lv::ProjectDocumentSection
*/
ProjectDocumentSection::Ptr ProjectDocument::createSection(int type, int position, int length){
    ProjectDocumentSection::Ptr section = ProjectDocumentSection::create(this, type, position, length);

    // sections are added in descending order according to their position
    SectionIterator it = d_ptr->sections.begin();

    while( it != d_ptr->sections.end() ){
        ProjectDocumentSection::Ptr& itSection = *it;

        if ( itSection->position() < section->position() )
            break;

        ++it;
    }
    d_ptr->sections.insert(it, section);

    if ( d_ptr->textDocument ){
        QTextBlock bl = d_ptr->textDocument->findBlock(section->position());
        ProjectDocumentBlockData* blockdata = static_cast<ProjectDocumentBlockData*>(bl.userData());
        if ( !blockdata ){
            blockdata = new ProjectDocumentBlockData;
            bl.setUserData(blockdata);
        }
        blockdata->addSection(section);
    }

    return section;
}

/**
 * \brief Returns the section at the given position
 */
ProjectDocumentSection::Ptr ProjectDocument::sectionAt(int position){
    SectionIterator it = d_ptr->sections.begin();
    while( it != d_ptr->sections.end() ){
        ProjectDocumentSection::Ptr& section = *it;
        if ( section->position() == position )
            return section;

        if ( section->position() < position )
            return ProjectDocumentSection::Ptr(nullptr);

        ++it;
    }
    return ProjectDocumentSection::Ptr(nullptr);
}


/**
 * \brief Removes section at given position
 */
bool ProjectDocument::removeSectionAt(int position){
    SectionIterator it = d_ptr->sections.begin();
    while( it != d_ptr->sections.end() ){
        ProjectDocumentSection::Ptr& section = *it;

        if ( section->position() == position ){
            section->invalidate();
            d_ptr->sections.erase(it);
            return true;
        }

        if ( section->position() < position )
            return false;

        ++it;
    }
    return false;
}

/**
 * \brief Removes section given its pointer
 */
void ProjectDocument::removeSection(ProjectDocumentSection::Ptr section){
    int sectionPosition = section->position();
    section->invalidate();
    if ( d_ptr->iteratingSections ){
        d_ptr->sectionsToRemove.push_back(section);
    } else {
        SectionIterator it = d_ptr->sections.begin();
        while( it != d_ptr->sections.end() ){
            ProjectDocumentSection::Ptr& currentSection = *it;
            if ( currentSection.data() == section.data() ){
                d_ptr->sections.erase(it);

                if ( d_ptr->textDocument ){
                    QTextBlock tb = d_ptr->textDocument->findBlock(sectionPosition);
                    emit formatChanged(tb.position(), tb.length());
                }

                return;
            }
            ++it;
        }
    }
}

/**
 * \brief Shows if the current document is active
 */
bool ProjectDocument::isActive() const{
    Project* prj = qobject_cast<Project*>(parent());
    if ( prj && prj->active() && prj->active()->path() == file()->path() )
        return true;
    return false;
}

/*bool ProjectDocument::isDirty() const
{
    return m_isDirty;
}

void ProjectDocument::setDirty(bool dirty)
{
    m_isDirty = dirty;
}*/

/**
 * \brief Overrides Document::content
 */
QByteArray ProjectDocument::content(){
    return contentString().toUtf8();
}

/**
 * \brief Overrides Document::setContent
 */
void ProjectDocument::setContent(const QByteArray &content){
    d_ptr->textDocument->setPlainText(QString::fromUtf8(content));
}

const QString& ProjectDocument::contentString(){
    if ( d_ptr->contentStringDirty ){
        d_ptr->contentString = d_ptr->textDocument->toPlainText();
        d_ptr->contentStringDirty = false;
    }
    return d_ptr->contentString;
}

/**
 * \brief Shows a small text preview of the text around a given position, including a visual pointer to the position itself
 */
QString ProjectDocument::peekContent(int position) const{
    QString result = "";
    if ( position >= d_ptr->textDocument->characterCount() )
        return result;

    QTextBlock bl = d_ptr->textDocument->findBlock(position);
    QString linePointer;
    int positionInBlock = position - bl.position();

    for ( int i = 0; i < positionInBlock; ++i )
          linePointer.append(' ');
    linePointer.append('^');
    for ( int i = positionInBlock + 1; i < bl.length(); ++i )
        linePointer.append(' ');

    QTextBlock prevBl = bl.previous();
    if ( prevBl.isValid() )
        result += QString("%1").arg(prevBl.blockNumber(), 5, 10, QChar('0')) + prevBl.text() + "\n";
    result += QString("%1").arg(bl.blockNumber(), 5, 10, QChar('0')) + bl.text() + "\n";
    result += QString("%1").arg(bl.blockNumber(), 5, 10, QChar('0')) + linePointer + "\n";
    QTextBlock nextBl = bl.next();
    if ( nextBl.isValid() )
        result += QString("%1").arg(nextBl.blockNumber(), 5, 10, QChar('0')) + nextBl.text();

    return result;
}

QString ProjectDocument::substring(int from, int length) const{
    QTextCursor tc(d_ptr->textDocument);
    tc.setPosition(from);
    tc.setPosition(from + length, QTextCursor::KeepAnchor);
    return tc.selectedText();
}

void ProjectDocument::insert(int from, int length, const QString &text, int editingState){
    if ( editingState == ProjectDocument::Assisted || editingState == ProjectDocument::Palette || editingState == ProjectDocument::Runtime ){
        auto es = static_cast<ProjectDocument::EditingState>(editingState);
        addEditingState(es);

        QTextCursor tc(d_ptr->textDocument);
        tc.beginEditBlock();
        tc.setPosition(from);
        tc.setPosition(from + length - 1, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        tc.insertText(text);
        tc.endEditBlock();

        removeEditingState(es);
    } else {
        QTextCursor tc(d_ptr->textDocument);
        tc.beginEditBlock();
        tc.setPosition(from);
        tc.setPosition(from + length, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        tc.insertText(text);
        tc.endEditBlock();
    }


}

int ProjectDocument::offsetAtLine(int line) const{
    return d_ptr->textDocument->findBlockByLineNumber(line - 1).position();
}

int ProjectDocument::lastCursorPosition()
{
    return d_ptr->lastCursorPosition;
}

void ProjectDocument::setLastCursorPosition(int pos)
{
    d_ptr->lastCursorPosition = pos;
}

ProjectDocument *ProjectDocument::castFrom(Document *document){
    return qobject_cast<ProjectDocument*>(document);
}

void ProjectDocument::addLineAtBlockNumber(QString line, int pos){

    QTextCursor c(textDocument());
    if (pos == 0)
    {
        c.beginEditBlock();
        c.insertText(line + "\n");
        c.endEditBlock();
    } else {
        auto block = textDocument()->findBlockByNumber(pos-1);
        c = QTextCursor(block);
        c.beginEditBlock();
        c.movePosition(QTextCursor::EndOfBlock);
        c.insertText("\n" + line);
        c.endEditBlock();
    }

}

void ProjectDocument::removeLineAtBlockNumber(int pos){
    auto block = textDocument()->findBlockByNumber(pos);
    QTextCursor c(block);
    c.beginEditBlock();
    c.select(QTextCursor::BlockUnderCursor);
    c.removeSelectedText();
    c.endEditBlock();
}

/**
 * \brief Slot for tracking text document changes which updates markers and sections
 */
void ProjectDocument::__documentContentsChanged(int position, int charsRemoved, int charsAdded){
    d_ptr->contentStringDirty = true;
    emit contentsChange(position, charsRemoved, charsAdded);

    QString addedText = "";
    if ( charsAdded == 1 ){
        QChar c = d_ptr->textDocument->characterAt(position);
        if ( c == CodeHandler::ParagraphSeparator )
            c = CodeHandler::NewLine;
        addedText = c;
    } else if ( charsAdded > 0 ){
        QTextCursor cursor(d_ptr->textDocument);
        cursor.setPosition(position);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        addedText = cursor.selection().toPlainText();
    }

    updateMarkers(position, charsRemoved, addedText.size());
    updateSections(position, charsRemoved, addedText);
}

void ProjectDocument::__documentModificationChanged(){
    setIsDirty(d_ptr->textDocument->isModified());
}

void ProjectDocument::__documentSaved(){
    d_ptr->textDocument->setModified(false);
}

void ProjectDocument::syncContent() const{
    while ( d_ptr->lastChange != d_ptr->changes.end() ){
        d_ptr->lastChange->redo();
        ++d_ptr->lastChange;
    }
    d_ptr->isSynced = true;
}

/**
 * \brief ProjectDocument destructor
 */
ProjectDocument::~ProjectDocument(){
    delete d_ptr;
}

/**
 * \class lv::ProjectDocumentAction
 * \brief Undo/redo implementation for project documents
 *
 * \ingroup lveditor
 */

/**
 * \brief Undo implementation
 */
void ProjectDocumentAction::undo(){
//    parent->m_content.replace(position, charsAdded.size(), charsRemoved);
}

/**
 * \brief Redo implementation
 */
void ProjectDocumentAction::redo(){
//    parent->m_content.replace(position, charsRemoved.size(), charsAdded);
}

ProjectDocumentBlockData::ProjectDocumentBlockData()
    : m_sections(new std::list<ProjectDocumentSection::Ptr>())
    , m_exceededSections(new std::list<ProjectDocumentSection::Ptr>())
    , m_numOfCollapsedLines(0)
    , m_stateChangeFlag(false)
{
}

ProjectDocumentBlockData::~ProjectDocumentBlockData(){
    foreach ( const ProjectDocumentSection::Ptr& section, *m_sections ){
        section->m_parentBlock = nullptr;
    }
    delete m_sections;
    delete m_exceededSections;
}

/**
 * \class lv::ProjectDocumentSection
 * \brief Dynamic project document section
 *
 * The section is defined by its position and length. Similar to the dynamic markers, this gets moved around and modified
 * along with document changes.
 * \ingroup lveditor
 */

/**
 * \fn lv::ProjectDocumentSection::position
 * \brief Returns the first position of the section
 */

/**
 * \fn lv::ProjectDocumentSection::length
 * \brief Returns the length of the section
 */

/**
 * \fn lv::ProjectDocumentSection::type
 * \brief Returns the type of section set by the user
 */

/**
 * \fn lv::ProjectDocumentSection::isValid
 * \brief Shows if the section is still valid
 */

/**
 * \fn lv::ProjectDocumentSection::setUserData
 * \brief Set the custom user data inside the section
 */

/**
 * \fn lv::ProjectDocumentSection::userData
 * \brief Returns the custom user data
 */

/**
 * \fn lv::ProjectDocumentSection::document
 * \brief Returns the document the section belongs to
 */

/**
 * \fn lv::ProjectDocumentSection::parentBlock
 * \brief Returns the block containing the first position of the section
 */

/**
 * \fn lv::ProjectDocumentSection::create
 * \brief Create a ProjectDocumentMarker with given parameters
 */

/**
 * \brief Default destructor
 */

ProjectDocumentSection::~ProjectDocumentSection(){
}

/** Sets the callback funciton to be called when the text changes */
void ProjectDocumentSection::onTextChanged(
        std::function<void(ProjectDocumentSection::Ptr, int, int, const QString &)> handler)
{
    m_textChangedHandler = handler;
}

/** Make a section invalid and remove it from its parent block */
void ProjectDocumentSection::invalidate(){
    m_position = -1;
    if ( m_parentBlock )
        m_parentBlock->removeSection(this);
}


void ProjectDocumentBlockData::addSection(ProjectDocumentSection::Ptr section){
    section->m_parentBlock = this;
    m_sections->push_back(section);
}

void ProjectDocumentBlockData::removeSection(ProjectDocumentSection::Ptr section){
    removeSection(section.data());
}

void ProjectDocumentBlockData::removeSection(ProjectDocumentSection *section){
    for ( auto it = m_sections->begin(); it != m_sections->end(); ++it ){
        if ( it->data() == section ){
            m_sections->erase(it);
            return;
        }
    }
    if ( section->m_parentBlock == this )
        section->m_parentBlock = nullptr;
}

void ProjectDocumentBlockData::setCollapse(CollapseFunctionType func)
{
    m_onCollapse = func;

}

void ProjectDocumentBlockData::setNumOfCollapsedLines(int num)
{
    m_numOfCollapsedLines = num;
}

void ProjectDocumentBlockData::setReplacementString(QString &repl)
{
    m_replacementString = repl;
}

// ProjectDocumentBlockData::CollapseState ProjectDocumentBlockData::collapseState() { return m_collapseState; }
int ProjectDocumentBlockData::numOfCollapsedLines() { return m_numOfCollapsedLines; }
CollapseFunctionType ProjectDocumentBlockData::onCollapse() { return m_onCollapse; }
QString& ProjectDocumentBlockData::replacementString() { return m_replacementString; }

void ProjectDocumentBlockData::resetCollapseParams()
{
    // m_collapseState = NoCollapse;
    m_collapsable = false;
	m_replacementString = QString();

}

} // namespace

