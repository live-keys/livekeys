#include "qmlcodeconverter.h"
#include "live/qmleditfragment.h"
#include "live/projectfile.h"
#include "live/documentqmlinfo.h"
#include "live/codepalette.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "qmlbindingchannel.h"
#include "qmlbindingspan.h"

#include "live/codecompletionmodel.h"
#include "live/codeqmlhandler.h"

#include <QDateTime>
#include <QVariant>
#include <QJSValue>
#include <QJSValueIterator>

namespace lv{

QmlCodeConverter::QmlCodeConverter(QmlEditFragment *edit, QObject *parent)
    : QObject(parent)
    , m_edit(edit)
{
}

QmlCodeConverter::~QmlCodeConverter(){
}

QObject *QmlCodeConverter::create(
        const DocumentQmlInfo::ConstPtr &info,
        const QString &declaration,
        const QString &path,
        QObject* parent)
{
    QString fullDeclaration;

    for ( auto it = info->imports().begin(); it != info->imports().end(); ++it ){
        fullDeclaration +=
            "import " + it->uri() + " " +
            QString::number(it->versionMajor()) + "." + QString::number(it->versionMinor());
        if ( !it->as().isEmpty() ){
            fullDeclaration += " as " + it->as();
        }
        fullDeclaration += "\n";
    }
    fullDeclaration += "\n" + declaration + "\n";

    QObject* obj = ViewContext::instance().engine()->createObject(fullDeclaration, parent, path);

    return obj;
}

void QmlCodeConverter::writeProperties(const QJSValue &properties){
    if ( !properties.isObject() ){
        qWarning("Properties must be of object type, use 'write' to add code directly.");
        return;
    }

    int valuePosition = m_edit->declaration()->valuePosition();
    int valueLength   = m_edit->declaration()->valueLength();
    QString source    = m_edit->declaration()->document()->content().mid(valuePosition, valueLength);

    lv::DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create(m_edit->declaration()->document()->file()->path());
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

        if ( leftOverProperties.contains(propertyName) ){
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
        source.insert(writeIndex + 1, "\n" + indent + *it + ": " + buildCode(properties.property(*it)));
    }

    m_edit->write(source);
}

void QmlCodeConverter::write(const QJSValue value){
    m_edit->write(buildCode(value));
}

void QmlCodeConverter::writeForFragment(QmlEditFragment *edit, const QJSValue value)
{
    edit->write(buildCode(value));
}

QVariant QmlCodeConverter::parse(){
    QString val = m_edit->readValueText();
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

void QmlCodeConverter::updateBindings(){
    if ( m_edit->bindingPalette() ){
        m_whenBinding.call();
    }
}

void QmlCodeConverter::suggestionsForExpression(const QString &expression, CodeCompletionModel *model, bool suggestFunctions){
    QObject* editParent = m_edit->parent();
    CodeQmlHandler* qmlHandler = nullptr;
    while ( editParent ){
        qmlHandler = qobject_cast<CodeQmlHandler*>(editParent);
        if ( qmlHandler )
            break;

        editParent = editParent->parent();
    }

    if (qmlHandler){
        qmlHandler->suggestionsForProposedExpression(m_edit->declaration(), expression, model, suggestFunctions);
    }
}

bool QmlCodeConverter::bindExpression(const QString &expression){
    return QmlCodeConverter::bindExpressionForFragment(m_edit, expression);
}

bool QmlCodeConverter::bindExpressionForFragment(QmlEditFragment *edit, const QString &expression)
{
    if (!edit) return false;

    QObject* editParent = edit->parent();
    CodeQmlHandler* qmlHandler = nullptr;
    while ( editParent ){
        qmlHandler = qobject_cast<CodeQmlHandler*>(editParent);
        if ( qmlHandler )
            break;

        editParent = editParent->parent();
    }

    if (qmlHandler){
        return qmlHandler->findBindingForExpression(edit, expression);
    }

    return false;
}

bool QmlCodeConverter::bindFunctionExpression(const QString &expression){
    QObject* editParent = m_edit->parent();
    CodeQmlHandler* qmlHandler = nullptr;
    while ( editParent ){
        qmlHandler = qobject_cast<CodeQmlHandler*>(editParent);
        if ( qmlHandler )
            break;

        editParent = editParent->parent();
    }

    if (qmlHandler){
        return qmlHandler->findFunctionBindingForExpression(m_edit, expression);
    }

    return false;
}

void QmlCodeConverter::updateFromPalette(){
    CodePalette* palette = static_cast<CodePalette*>(parent());
    if ( palette && m_edit->bindingSpan() ){
        m_edit->bindingSpan()->commit(palette->value());
    }
}

QString QmlCodeConverter::buildCode(const QJSValue &value){
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

        if (value.hasOwnProperty("length")){
            // isArray isn't registering is for some reason
            QJSValueIterator it(value);
            QString result = "[";
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
