#include "lveditfragment.h"
#include "live/qmldeclaration.h"
#include "live/codepalette.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "codeconverter.h"
#include "bindingchannel.h"
#include "bindingspan.h"
#include "bindingspanmodel.h"

namespace lv{

LvEditFragment::LvEditFragment(QmlDeclaration::Ptr declaration, QObject *parent)
    : QObject(parent)
    , m_declaration(declaration)
    , m_bindingPalette(nullptr)
    , m_bindingSpan(new BindingSpan(this))
    , m_visualParent(nullptr)
    , m_bindingSpanModel(nullptr)
{
}

LvEditFragment::~LvEditFragment(){
    ProjectDocumentSection::Ptr section = declaration()->section();
    ProjectDocument* doc = section->document();
    doc->removeSection(section);

    for ( auto it = childFragments().begin(); it != childFragments().end(); ++it ){
        LvEditFragment* edit = *it;
        edit->deleteLater();
    }

    //delete m_bindingSpanModel;
    delete m_bindingSpan;
}

void LvEditFragment::setRelativeBinding(const std::shared_ptr<el::BindingPath> &bp){

    el::BindingPath::Ptr bindingToParent = bp;

    LvEditFragment* parent = parentFragment();
    if( parent && parent->parentFragment() ){
        bindingToParent = el::BindingPath::join(parent->m_relativeBinding, bindingToParent);
    }

    m_relativeBinding = bindingToParent;

    LvEditFragment* root = rootFragment();
    if ( !root )
        return;

    QList<BindingChannel::Ptr> bc = root->bindingSpan()->outputChannels();
    for ( auto it = bc.begin(); it != bc.end(); ++it ){
        BindingChannel::Ptr& bc = *it;
        if ( bc->isEnabled() ){
            el::BindingPath::Ptr newbp = el::BindingPath::join(bc->bindingPath(), m_relativeBinding, false);
            Runnable* r = bc->runnable();
            auto newbc = BindingChannel::create(newbp, r);
            newbc->traverseBindingPath();
            newbc->setEnabled(true);
            bindingSpan()->addOutputChannel(newbc);
        }
    }
}


int LvEditFragment::valuePosition() const{
    return m_declaration->valuePosition();
}


int LvEditFragment::valueLength() const{
    return m_declaration->valueLength();
}

bool LvEditFragment::isForObject() const{
    return m_declaration->isForObject();
}

bool LvEditFragment::isForProperty() const{
    return m_declaration->isForProperty();
}

CodePalette *LvEditFragment::palette(const QString &type){
    for ( auto it = begin(); it != end(); ++it ){
        CodePalette* current = *it;
        if ( current->type() == type )
            return current;
    }
    return nullptr;
}

void LvEditFragment::addPalette(CodePalette *palette){
    m_palettes.append(palette);
}

void LvEditFragment::removePalette(CodePalette *palette){
    for ( auto it = begin(); it != end(); ++it ){
        CodePalette* cp = *it;
        if ( cp == palette ){
            emit aboutToRemovePalette(palette);
            m_palettes.erase(it);
            if ( bindingPalette() != palette )
                palette->deleteLater();
            break;
        }
    }

    if (m_palettes.size() == 0)
    {
        emit paletteListEmpty();
    }
}

int LvEditFragment::totalPalettes() const{
    return m_palettes.size();
}

void LvEditFragment::removeBindingPalette(){
    if ( !m_bindingPalette )
        return;

    if ( hasPalette(m_bindingPalette ) )
        m_bindingPalette = nullptr;
    else {
        m_bindingPalette->deleteLater();
        m_bindingPalette = nullptr;
    }
}

void LvEditFragment::setBindingPalette(CodePalette *palette){
    removeBindingPalette();
    m_bindingPalette = palette;
}

void LvEditFragment::addChildFragment(LvEditFragment *edit){
    m_childFragments.append(edit);
}

void LvEditFragment::removeChildFragment(LvEditFragment *edit){
    for ( auto it = m_childFragments.begin(); it != m_childFragments.end(); ++it ){
        if ( *it == edit ){
            edit->emitRemoval();
            m_childFragments.erase(it);
            edit->deleteLater();
            return;
        }
    }
}

void LvEditFragment::write(const QString &code){
    ProjectDocument* document = m_declaration->document();

    if ( document->editingStateIs(ProjectDocument::Palette))
        return;

    document->addEditingState(ProjectDocument::Palette);

    {
        QTextCursor tc(document->textDocument());
        tc.setPosition(valuePosition());
        tc.setPosition(valuePosition() + valueLength(), QTextCursor::KeepAnchor);
        tc.beginEditBlock();
        tc.removeSelectedText();
        tc.insertText(code);
        tc.endEditBlock();
    }

    document->removeEditingState(ProjectDocument::Palette);
}

QString LvEditFragment::readValueText() const{
    QTextCursor tc(m_declaration->document()->textDocument());
    tc.setPosition(valuePosition());
    tc.setPosition(valuePosition() + valueLength(), QTextCursor::KeepAnchor);
    return tc.selectedText();
}

void LvEditFragment::updatePaletteValue(CodePalette *palette){
    BindingChannel::Ptr inputChannel = bindingSpan()->inputChannel();
    if ( !inputChannel )
        return;

    if ( inputChannel->listIndex() == -1 ){
        // palette->setValueFromBinding(inputChannel->property().read()); // TODO: ELEMENTS #381
    } else {
//        QQmlListReference ppref = qvariant_cast<QQmlListReference>(inputChannel->property().read());
//        palette->setValueFromBinding(QVariant::fromValue(ppref.at(inputChannel->listIndex()))); // TODO: ELEMENTS #381
    }
}

LvEditFragment *LvEditFragment::parentFragment(){
    return qobject_cast<LvEditFragment*>(parent());
}

LvEditFragment *LvEditFragment::rootFragment(){
    LvEditFragment* root = parentFragment();
    if ( !root )
        return nullptr;

    while ( root->parentFragment() ){
        root = root->parentFragment();
    }
    return root;
}

void LvEditFragment::emitRemoval(){
    emit aboutToBeRemoved();

    for ( auto it = begin(); it != end(); ++it ){
        CodePalette* cp = *it;
        cp->deleteLater();
    }

    if ( m_bindingPalette )
        m_bindingPalette->deleteLater();


    m_palettes.clear();
    m_bindingPalette = nullptr;
}

BindingSpanModel* LvEditFragment::bindingModel(lv::LanguageLvHandler *codeHandler){
    if ( !m_bindingSpanModel ){
        m_bindingSpanModel = new BindingSpanModel(this);
        QString fileName = declaration()->document()->file()->name();
        if ( fileName.length() && fileName.front().isUpper() ){
            m_bindingSpanModel->initializeScanner(codeHandler);
        }
    }
    return m_bindingSpanModel;
}

QString LvEditFragment::type() const{
    return m_declaration->type().join();
}

QList<QObject *> LvEditFragment::getChildFragments() const{
    QList<QObject*> result;
    for (LvEditFragment* edit : m_childFragments)
        result.append(edit);
    return result;
}

void LvEditFragment::updateValue(){
    BindingChannel::Ptr inputPath = bindingSpan()->inputChannel();

    if ( inputPath && inputPath->listIndex() == -1 ){
        for ( auto it = m_palettes.begin(); it != m_palettes.end(); ++it ){
            CodePalette* cp = *it;
            // cp->setValueFromBinding(inputPath->property()->read()); // TODO: ELEMENTS #381
        }
        if ( m_bindingPalette ){
            // m_bindingPalette->setValueFromBinding(inputPath->property().read()); // TODO: ELEMENTS #381
            CodeConverter* cvt = static_cast<CodeConverter*>(m_bindingPalette->extension());
            cvt->whenBinding().call();
        }
    }
}

void LvEditFragment::__inputRunnableObjectReady(){
    BindingChannel::Ptr inputChannel = bindingSpan()->inputChannel();
    if ( inputChannel && inputChannel->listIndex() == -1 ){
//        inputChannel->property().connectNotifySignal(this, SLOT(updateValue())); // TODO: ELEMENTS analogous
    }
}

}// namespace
