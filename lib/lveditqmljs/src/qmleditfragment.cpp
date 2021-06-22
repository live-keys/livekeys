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

#include "live/qmleditfragment.h"
#include "live/qmldeclaration.h"
#include "live/codepalette.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "qmlbindingchannel.h"
#include "live/codeqmlhandler.h"
#include "live/viewcontext.h"
#include "documentqmlchannels.h"

#include <QJSValue>
#include <QJSValueIterator>

namespace lv{

/**
 * \class lv::QmlEditFragment
 * \ingroup lveditqmljs
 * \brief An editing fragment for a lv::ProjectDocument.
 *
 * An editing fragment represents a section within a lv::ProjectDocument that is connected to
 * the running application. Fragments have palettes associated with them, and can write code
 * based on the given value of a palette. They provide the set of binding channels connected
 * to the application.
 */

/**
 * \brief QmlEditFragment contructor
 *
 * The Fragment is constructed from a \p declaration object and a \p palette object.
 */
QmlEditFragment::QmlEditFragment(QmlDeclaration::Ptr declaration, lv::CodeQmlHandler* codeHandler, QObject *parent)
    : QObject(parent)
    , m_declaration(declaration)
    , m_bindingPalette(nullptr)
    , m_visualParent(nullptr)
    , m_refCount(0)
    , m_fragmentType(0)
    , m_codeHandler(codeHandler)
{
    if (m_declaration->isForSlot()){
        m_location = Slot;
    } else if (m_declaration->isForComponent() ){
        m_location = Component;
    } else if (m_declaration->isForImports()){
        m_location = Imports;
    } else if (m_declaration->isForObject()){
        m_location = Object;
    } else if (m_declaration->isForProperty()){
        m_location = Property;
    }
}

/**
 * \brief QmlEditFragment destructor
 */
QmlEditFragment::~QmlEditFragment(){
    for ( auto pal : m_palettes ){
        pal->deleteLater();
    }

    ProjectDocumentSection::Ptr section = declaration()->section();
    ProjectDocument* doc = declaration()->document();
    doc->removeSection(section);

    auto fragments = childFragments();
    for ( auto it = fragments.begin(); it != fragments.end(); ++it ){
        QmlEditFragment* edit = *it;
        edit->deleteLater();
    }
}

QObject *QmlEditFragment::createObject(const DocumentQmlInfo::ConstPtr &info, const QString &declaration, const QString &path, QObject *parent, QQmlContext *context)
{
    QString fullDeclaration;
    fullDeclaration = DocumentQmlInfo::Import::join(info->imports()) + "\n" + declaration + "\n";

    ViewEngine* engine = ViewContext::instance().engine();
    ViewEngine::ComponentResult::Ptr cr = engine->createObject(
        path, fullDeclaration.toUtf8(), parent, context
    );
    return cr->object;
}

/**
 * \brief Returns the lv::QmlDeclaration's value postion
 */
int QmlEditFragment::valuePosition() const{
    return m_declaration->valuePosition();
}


/**
 * \brief Returns the lv::QmlDeclaration's value length
 */
int QmlEditFragment::valueLength() const{
    return m_declaration->valueLength();
}

int QmlEditFragment::length() const{
    return m_declaration->length();
}

bool QmlEditFragment::isBuilder() const{
    return isOfFragmentType(QmlEditFragment::FragmentType::Builder);
}

void QmlEditFragment::rebuild(){
    if ( m_channel->isBuilder() ){
        codeHandler()->bindingChannels()->rebuild();
    }
}

bool QmlEditFragment::isGroup() const
{
    return m_fragmentType & QmlEditFragment::FragmentType::Group;
}

void QmlEditFragment::checkIfGroup()
{
    bool isForAnObject = false;
    if ( !declaration()->type().path().isEmpty() )
        isForAnObject = true;
    else if (declaration()->type().name() != "import" && declaration()->type().name() != "slot")
        isForAnObject = QmlTypeInfo::isObject(declaration()->type().name());

    if (!isForAnObject) return;

    if (declaration()->type().language() == QmlTypeReference::Cpp){
        addFragmentType(QmlEditFragment::FragmentType::Group);
    }
}

QString QmlEditFragment::defaultValue() const{
    return QmlTypeInfo::typeDefaultValue(m_declaration->type().name());
}

CodePalette *QmlEditFragment::palette(const QString &type){
    for ( auto it = begin(); it != end(); ++it ){
        CodePalette* current = *it;
        if ( current->type() == type )
            return current;
    }
    return nullptr;
}

void QmlEditFragment::addPalette(CodePalette *palette){
    m_palettes.append(palette);
}

void QmlEditFragment::removePalette(CodePalette *palette){
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

int QmlEditFragment::totalPalettes() const{
    return m_palettes.size();
}

void QmlEditFragment::removeBindingPalette(){
    if ( !m_bindingPalette )
        return;

    if ( hasPalette(m_bindingPalette ) )
        m_bindingPalette = nullptr;
    else {
        m_bindingPalette->deleteLater();
        m_bindingPalette = nullptr;
    }
}

void QmlEditFragment::setBindingPalette(CodePalette *palette){
    removeBindingPalette();
    m_bindingPalette = palette;
}

void QmlEditFragment::addChildFragment(QmlEditFragment *edit){
    m_childFragments.append(edit);
    edit->setParent(this);
    if ( m_channel ){
        DocumentQmlChannels* channels = m_codeHandler->bindingChannels();
        disconnect(channels, &DocumentQmlChannels::selectedChannelChanged, edit, &QmlEditFragment::__selectedChannelChanged);
    }
}

void QmlEditFragment::removeChildFragment(QmlEditFragment *edit){
    for ( auto it = m_childFragments.begin(); it != m_childFragments.end(); ++it ){
        if ( *it == edit ){
            edit->emitRemoval();
            edit->deleteLater();
            m_childFragments.erase(it);
            return;
        }
    }
}

void QmlEditFragment::setObjectId(QString id)
{
    m_objectId = id;
}

QString QmlEditFragment::objectId()
{
    return m_objectId;
}

void QmlEditFragment::writeProperties(const QJSValue &properties)
{
    if ( !properties.isObject() ){
        qWarning("Properties must be of object type, use 'write' to add code directly.");
        return;
    }

    QVariantMap propsWritable = m_codeHandler->propertiesWritable(this);

    int valuePosition = declaration()->valuePosition();
    int valueLength   = declaration()->valueLength();
    QString source    = declaration()->document()->content().mid(valuePosition, valueLength);

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(declaration()->document()->file()->path());
    if ( !docinfo->parse(source) )
        return;

    lv::DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

    QSet<QString> leftOverProperties;

    QJSValueIterator it(properties);
    while ( it.hasNext() ){
        it.next();
        leftOverProperties.insert(it.name());
    }

    QString indent = "";

    for ( auto it = root->properties.begin(); it != root->properties.end(); ++it ){
        lv::DocumentQmlValueObjects::RangeProperty* p = *it;
        QString propertyName = source.mid(p->begin, p->propertyEnd - p->begin);

        if ( indent.isEmpty() ){
            int index = p->begin - 1;
            while (index >= 0) {
                if ( source[index] == QChar('\n') ){
                    break;
                }
                if ( !source[index].isSpace() )
                    break;
                --index;
            }
            if ( index + 1 < p->begin - 1){
                indent = source.mid(index + 1, p->begin - 1 - index);
            }
        }

        if ( leftOverProperties.contains(propertyName)){
            if (propsWritable.value(propertyName).toBool())
                source.replace(p->valueBegin, p->end - p->valueBegin, buildCode(properties.property(propertyName)));
            leftOverProperties.remove(propertyName);
        }
    }

    int writeIndex = source.length() - 2;
    while ( writeIndex >= 0 ){
        if ( !source[writeIndex].isSpace() ){
            break;
        }
        --writeIndex;
    }

    if ( indent.isEmpty() ){
        int indentIndex = source.length() - 2;
        while ( indentIndex >= 0 ){
            if ( source[indentIndex] == QChar('\n') ){
                break;
            }
            if ( !source[indentIndex].isSpace() )
                break;
            --indentIndex;
        }
        if ( indentIndex + 1 < source.length() - 2){
            indent = source.mid(indentIndex + 1, source.length() - 2 - indentIndex) + "    ";
        }
    }

    for ( auto it = leftOverProperties.begin(); it != leftOverProperties.end(); ++it ){
        if (propsWritable.value(*it).toBool())
            source.insert(writeIndex + 1, "\n" + indent + *it + ": " + buildCode(properties.property(*it)));
    }

    writeCode(source);
}

void QmlEditFragment::write(const QJSValue value){
    m_codeHandler->populatePropertyInfoForFragment(this);
    bool isWritable = m_objectInfo.value("isWritable").toBool();

    if (isWritable || m_declaration->isForSlot()){
        writeCode(buildCode(value));
    }
}

/**
 * \brief Writes the \p code to the value part of this fragment
 */
void QmlEditFragment::writeCode(const QString &code){
    auto old = readValueText();
    if (code == old) return;
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

    if (code == "null" || old == "null")
        emit isNullChanged();
}

QObject *QmlEditFragment::readObject()
{
    if ( m_channel->isEnabled() ){
        if ( m_channel->property().propertyTypeCategory() == QQmlProperty::List ){
            QQmlListReference ppref = qvariant_cast<QQmlListReference>(m_channel->property().read());
            if (ppref.canAt()){
                QObject* parent = ppref.count() > 0 ? ppref.at(0)->parent() : ppref.object();

                // create correct order for list reference
                QObjectList ordered;
                for (auto child: parent->children())
                {
                    bool found = false;
                    for (int i = 0; i < ppref.count(); ++i)
                        if (child == ppref.at(i)){
                            found = true;
                            break;
                        }
                    if (found)
                        ordered.push_back(child);
                }
                return ordered[m_channel->listIndex()];
            }
        }
        else if (m_channel->property().propertyTypeCategory() == QQmlProperty::Object){
            return qvariant_cast<QObject*>(m_channel->property().read());
        }
    }

    return nullptr;
}

QVariant QmlEditFragment::parse()
{
    QString val = readValueText();
    if ( val.length() > 1 && (
             (val.startsWith('"') && val.endsWith('"')) ||
             (val.startsWith('\'') && val.endsWith('\'') )
        ))
    {
        return QVariant(val.mid(1, val.length() - 2));
    } else if ( val == "true" ){
        return QVariant(true);
    } else if ( val == "false" ){
        return QVariant(false);
    } else {
        bool ok;
        qint64 number = val.toLongLong(&ok);
        if (ok)
            return QVariant(number);

        return QVariant(val.toDouble());
    }
}

void QmlEditFragment::updateBindings()
{
    if (bindingPalette())
        m_whenBinding.call();
}

void QmlEditFragment::updateFromPalette()
{
    CodePalette* palette = qobject_cast<CodePalette*>(sender());
    if ( palette ){
        commit(palette->value());
    }
}

void QmlEditFragment::suggestionsForExpression(const QString &expression, CodeCompletionModel *model, bool suggestFunctions)
{
    QObject* editParent = parent();
    CodeQmlHandler* qmlHandler = nullptr;
    while ( editParent ){
        qmlHandler = qobject_cast<CodeQmlHandler*>(editParent);
        if ( qmlHandler )
            break;

        editParent = editParent->parent();
    }

    if (qmlHandler){
        qmlHandler->suggestionsForProposedExpression(declaration(), expression, model, suggestFunctions);
    }
}


/**
 * \brief Reads the code value of this fragment and returns it.
 */
QString QmlEditFragment::readValueText() const{
    QTextCursor tc(m_declaration->document()->textDocument());
    tc.setPosition(valuePosition());
    tc.setPosition(valuePosition() + valueLength(), QTextCursor::KeepAnchor);
    return tc.selectedText();
}

void QmlEditFragment::updatePaletteValue(CodePalette *palette){
    if ( !m_channel )
        return;

    if ( m_channel->listIndex() == -1 ){
        if ( m_channel->isBuilder() ){
            palette->initViaSource();
        } else {
            palette->setValueFromBinding(m_channel->property().read());
        }
    } else {
        QQmlListReference ppref = qvariant_cast<QQmlListReference>(m_channel->property().read());
        QObject* parent = ppref.count() > 0 ? ppref.at(0)->parent() : ppref.object();

        // create correct order for list reference
        QObjectList ordered;

        for (auto child: parent->children())
        {
            bool found = false;
            for (int i = 0; i < ppref.count(); ++i)
                if (child == ppref.at(i)){
                    found = true;
                    break;
                }
            if (found)
                ordered.push_back(child);
        }

        palette->setValueFromBinding(QVariant::fromValue(ordered[m_channel->listIndex()]));
    }
}

void QmlEditFragment::initializePaletteValue(CodePalette *palette){
    if ( !m_channel )
        return;

    if ( m_channel->listIndex() == -1 ){
        if ( m_channel->isBuilder() ){
            palette->initViaSource();
        } else {
            palette->initValue(m_channel->property().read());
        }
    } else {
        QQmlListReference ppref = qvariant_cast<QQmlListReference>(m_channel->property().read());
        QObject* parent = ppref.count() > 0 ? ppref.at(0)->parent() : ppref.object();

        // create correct order for list reference
        QObjectList ordered;

        for (auto child: parent->children())
        {
            bool found = false;
            for (int i = 0; i < ppref.count(); ++i)
                if (child == ppref.at(i)){
                    found = true;
                    break;
                }
            if (found)
                ordered.push_back(child);
        }

        palette->initValue(QVariant::fromValue(ordered[m_channel->listIndex()]));
    }
}

QmlEditFragment *QmlEditFragment::parentFragment(){
    return qobject_cast<QmlEditFragment*>(parent());
}

QmlEditFragment *QmlEditFragment::rootFragment(){
    QmlEditFragment* root = parentFragment();
    if ( !root )
        return nullptr;

    while ( root->parentFragment() ){
        root = root->parentFragment();
    }
    return root;
}

void QmlEditFragment::setObjectInitializeType(const QmlTypeReference &type){
    m_objectInitializeType = type;
}

const QmlTypeReference &QmlEditFragment::objectInitializeType() const{
    return m_objectInitializeType;
}

void QmlEditFragment::emitRemoval(){
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

QStringList QmlEditFragment::bindingPath(){
    QmlBindingChannel::Ptr inputChannel = m_channel;
    if ( inputChannel ){
        return DocumentQmlChannels::describePath(inputChannel->bindingPath());
    }
    return QStringList();
}

void QmlEditFragment::__selectedChannelChanged(){
    QmlEditFragment* pf = parentFragment();

    if ( m_channel && !m_channel->isBuilder() &&  ( m_channel->type() == QmlBindingChannel::Object || m_channel->type() == QmlBindingChannel::ListIndex ) ){
        QObject* ob = m_channel->object();
        disconnect(ob, &QObject::destroyed, this, &QmlEditFragment::__channelObjectErased);
    }

    if ( pf ){
        m_channel = DocumentQmlChannels::traverseBindingPathFrom(pf->channel(), m_channel->bindingPath());
        if ( m_channel ){
            m_channel->setEnabled(true);
            if ( m_channel->listIndex() == -1 ){
                m_channel->property().connectNotifySignal(this, SLOT(updateValue()));
            }
            for ( auto it = m_palettes.begin(); it != m_palettes.end(); ++it ){
                updatePaletteValue(*it);
            }
        }
    } else {
        if ( m_channel ){
            m_channel = DocumentQmlChannels::traverseBindingPathFrom(m_codeHandler->bindingChannels()->selectedChannel(), m_channel->bindingPath());
            if ( m_channel ){
                m_channel->setEnabled(true);
                if ( m_channel->listIndex() == -1 ){
                    m_channel->property().connectNotifySignal(this, SLOT(updateValue()));
                }
                for ( auto it = m_palettes.begin(); it != m_palettes.end(); ++it ){
                    updatePaletteValue(*it);
                }
            }
        }
    }
    for ( auto it = m_childFragments.begin(); it != m_childFragments.end(); ++it ){
        (*it)->__selectedChannelChanged();
    }
}

QString QmlEditFragment::type() const{
    return m_declaration->type().join();
}

QString QmlEditFragment::typeName() const{
    return m_declaration->type().name();
}

QString QmlEditFragment::identifier() const
{
    const QStringList& ic = m_declaration->identifierChain();
    if (ic.size() == 0) return "";
    return ic[ic.size()-1];
}

QString QmlEditFragment::objectInitializerType() const{
    return m_objectInitializeType.join();
}

QList<QObject*> QmlEditFragment::paletteList() const{
    QList<QObject*> result;
    for (CodePalette* palette : m_palettes)
        result.append(palette);
    return result;
}

QList<QObject *> QmlEditFragment::getChildFragments() const{
    QList<QObject*> result;
    for (QmlEditFragment* edit : m_childFragments)
        result.append(edit);
    return result;
}

void QmlEditFragment::updateValue(){
    QmlBindingChannel::Ptr inputPath = m_channel;

    if ( inputPath && inputPath->listIndex() == -1 ){
        for ( auto it = m_palettes.begin(); it != m_palettes.end(); ++it ){
            CodePalette* cp = *it;
            cp->setValueFromBinding(inputPath->property().read());
        }
        if ( m_bindingPalette ){
            m_bindingPalette->setValueFromBinding(inputPath->property().read());
            m_whenBinding.call();
        }
    }
}

void QmlEditFragment::__inputRunnableObjectReady(){
    QmlBindingChannel::Ptr inputChannel = m_channel;
    if ( inputChannel && inputChannel->listIndex() == -1 ){
        inputChannel->property().connectNotifySignal(this, SLOT(updateValue()));
    }
}

void QmlEditFragment::addNestedObjectInfo(QVariantMap& object)
{
    m_nestedObjectsInfo.push_back(object);
}

void QmlEditFragment::clearNestedObjectsInfo()
{
    m_nestedObjectsInfo = QVariantList();
}

void QmlEditFragment::setObjectInfo(QVariantMap &info)
{
    m_objectInfo = info;
}

void QmlEditFragment::setChannel(QSharedPointer<QmlBindingChannel> channel){
    if ( !m_channel ){
        if ( !parentFragment() ){
            DocumentQmlChannels* channels = m_codeHandler->bindingChannels();
            connect(channels, &DocumentQmlChannels::selectedChannelChanged, this, &QmlEditFragment::__selectedChannelChanged);
        }
    }
    m_channel = channel;
    if ( m_channel && !m_channel->isBuilder() && m_channel->type() == QmlBindingChannel::Object ){
        QObject* ob = m_channel->object();
        connect(ob, &QObject::destroyed, this, &QmlEditFragment::__channelObjectErased);
    } else if ( m_channel && !m_channel->isBuilder() && m_channel->type() == QmlBindingChannel::ListIndex ){
        QObject* ob = m_channel->object();
        connect(ob, &QObject::destroyed, this, &QmlEditFragment::__channelObjectErased);
    }
    if ( m_channel && m_channel->isBuilder() )
        addFragmentType(QmlEditFragment::Builder);
    else {
        removeFragmentType(QmlEditFragment::Builder);
    }

}

QSharedPointer<QmlBindingPath> QmlEditFragment::fullBindingPath(){
    if ( !m_channel )
        return nullptr;
    if ( !parentFragment() )
        return m_channel->bindingPath();

    return QmlBindingPath::join(parentFragment()->fullBindingPath(), m_channel->bindingPath());
}

int QmlEditFragment::fragmentType() const
{
    return m_fragmentType;
}

bool QmlEditFragment::isOfFragmentType(QmlEditFragment::FragmentType type) const
{
    return type & m_fragmentType;
}

void QmlEditFragment::addFragmentType(QmlEditFragment::FragmentType type)
{
    m_fragmentType |= type;
}

void QmlEditFragment::removeFragmentType(QmlEditFragment::FragmentType type)
{
    m_fragmentType &= ~type;
}

void QmlEditFragment::commit(const QVariant &value){
    if ( m_channel && m_channel->canModify() ){
        if ( m_channel->listIndex() == -1 ){
            m_channel->property().write(value);
        }
    }
}

void QmlEditFragment::incrementRefCount()
{
    ++m_refCount;
    emit refCountChanged();
}

void QmlEditFragment::decrementRefCount()
{
    --m_refCount;
    emit refCountChanged();
}

int QmlEditFragment::refCount()
{
    return m_refCount;
}

QVariantList QmlEditFragment::nestedObjectsInfo()
{
    return m_nestedObjectsInfo;
}

QVariantMap QmlEditFragment::objectInfo()
{
    return m_objectInfo;
}

void QmlEditFragment::signalPropertyAdded(QmlEditFragment *ef, bool expandDefault)
{
    emit propertyAdded(ef, expandDefault);
}

void QmlEditFragment::signalObjectAdded(QmlEditFragment *ef, QPointF cursorCoords)
{
    emit objectAdded(ef, cursorCoords);
}


bool QmlEditFragment::bindExpression(const QString &expression){
    if ( m_codeHandler )
        return m_codeHandler->findBindingForExpression(this, expression);
    return false;
}

bool QmlEditFragment::bindFunctionExpression(const QString &expression){
    if (m_codeHandler){
        return m_codeHandler->findFunctionBindingForExpression(this, expression);
    }

    return false;
}

bool QmlEditFragment::isNull()
{
    return readValueText() == "null";
}

bool QmlEditFragment::isMethod(){
    return m_channel ? m_channel->type() == QmlBindingChannel::Method : false;
}

void QmlEditFragment::__channelObjectErased(){
    if ( !m_channel )
        return;

    if ( m_channel->type() == QmlBindingChannel::Object ){
        m_codeHandler->removeEditingFragment(this);
    } else if ( m_channel->type() == QmlBindingChannel::ListIndex ){
        auto parentFrag = parentFragment();
        if (parentFrag){
            for (auto cf: parentFrag->childFragments()){
                if (!cf->channel()) continue;
                if (cf->channel()->listIndex() <= m_channel->listIndex()) continue;
                cf->channel()->updateConnection(cf->channel()->listIndex()-1);
            }
        }
        m_codeHandler->removeEditingFragment(this);
    }
}

QString QmlEditFragment::buildCode(const QJSValue &value){
    if ( value.isArray() ){
        QString result;
        QJSValueIterator it(value);
        result = "[";
        bool first = true;
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                if ( first ){
                    first = false;
                } else {
                    result += ",";
                }
                result += buildCode(it.value());
            }
        }
        result += "]";
        return result;
    } else if ( value.isDate() ){
        QDateTime dt = value.toDateTime();

        QString year    = QString::number(dt.date().year()) + ",";
        QString month   = QString::number(dt.date().month()) + ",";
        QString day     = QString::number(dt.date().day()) + ",";
        QString hour    = QString::number(dt.time().hour()) + ",";
        QString minute  = QString::number(dt.time().minute()) + ",";
        QString second  = QString::number(dt.time().second()) + ",";
        QString msecond = QString::number(dt.time().msec());

        return "new Date(" + year + month + day + hour + minute + second + msecond + ")";

    } else if ( value.isObject() ){
        if ( value.hasOwnProperty("__ref") ){
            QString result = value.property("__ref").toString();
            if ( result.isEmpty() ){
                return "null";
            } else {
                return result;
            }
        }

        QString text = value.toString();
        if (text.length() > 6 && text.left(6) == "QSizeF"){
            return "'" + value.property("width").toString() + "x" + value.property("height").toString() + "'";
        }

        QString result = "{";
        bool first = true;
        QJSValueIterator it(value);
        while ( it.hasNext() ){
            it.next();
            if ( first ){
                first = false;
            } else {
                result += ",";
            }

            result += it.name() + ": " + buildCode(it.value());
        }

        return result + "}";
    } else if ( value.isString() ){
        return "'" + value.toString() + "'";
    } else if ( value.isBool() || value.isNumber() ){
        return value.toString();
    }
    return "";
}

}// namespace

