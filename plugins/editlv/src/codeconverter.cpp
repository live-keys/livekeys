#include "codeconverter.h"
#include "live/projectfile.h"
#include "live/codepalette.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "bindingchannel.h"
#include "bindingspan.h"
#include "lveditfragment.h"

#include "live/codecompletionmodel.h"
#include "languagelvhandler.h"

#include <QDateTime>
#include <QVariant>
#include <QJSValue>
#include <QJSValueIterator>

namespace lv{

CodeConverter::CodeConverter(LvEditFragment *edit, QObject *parent)
    : QObject(parent)
    , m_edit(edit)
{
}

CodeConverter::~CodeConverter(){
}

QObject *CodeConverter::create(
        const el::ParsedDocument &pd,
        const QString &declaration,
        const QString &path,
        QObject* parent)
{
    QString fullDeclaration;
//    for ( auto it = scope.imports().begin(); it != scope.imports().end(); ++it ){
//        fullDeclaration +=
//            "import " + it->first.path() + " " +
//            QString::number(it->first.versionMajor()) + "." + QString::number(it->first.versionMinor());
//        if ( !it->first.as().isEmpty() ){
//            fullDeclaration += " as " + it->first.as();
//        }
//        fullDeclaration += "\n";
//    } // TODO: ELEMENTS Imports from ParsedDocument
    fullDeclaration += "\n" + declaration + "\n";

    QObject* obj = ViewContext::instance().engine()->createObject(fullDeclaration, parent, path);

    return obj;
}

void CodeConverter::writeProperties(const QJSValue &properties){
    if ( !properties.isObject() ){
        qWarning("Properties must be of object type, use 'write' to add code directly.");
        return;
    }

    int valuePosition = m_edit->declaration()->valuePosition();
    int valueLength   = m_edit->declaration()->valueLength();
    QString source    = m_edit->declaration()->document()->content().mid(valuePosition, valueLength);

    // TODO: ELEMENTS (ParsedDocument stuff)

    QSet<QString> leftOverProperties;

    QJSValueIterator it(properties);
    while ( it.hasNext() ){
        it.next();
        leftOverProperties.insert(it.name());
    }

    QString indent = "";

    // TODO: ELEMENTS

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

void CodeConverter::write(const QJSValue value){
    m_edit->write(buildCode(value));
}

QVariant CodeConverter::parse(){
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

void CodeConverter::updateBindings(){
    if ( m_edit->bindingPalette() ){
        m_whenBinding.call();
    }
}

void CodeConverter::suggestionsForExpression(const QString &/*expression*/, CodeCompletionModel */*model*/){
    QObject* editParent = m_edit->parent();
    LanguageLvHandler* lvHandler = nullptr;
    while ( editParent ){
        lvHandler = qobject_cast<LanguageLvHandler*>(editParent);
        if ( lvHandler )
            break;

        editParent = editParent->parent();
    }

    if (lvHandler){
        // lvHandler->suggestionsForProposedExpression(m_edit->declaration(), expression, model); // TODO: ELEMENTS #384
    }
}

bool CodeConverter::bindExpression(const QString &/*expression*/){
    QObject* editParent = m_edit->parent();
    LanguageLvHandler* lvHandler = nullptr;
    while ( editParent ){
        lvHandler = qobject_cast<LanguageLvHandler*>(editParent);
        if ( lvHandler )
            break;

        editParent = editParent->parent();
    }

    if (lvHandler){
        // return lvHandler->findBindingForExpression(m_edit, expression); // TODO: ELEMENTS #384
    }

    return false;
}

void CodeConverter::updateFromPalette(){
    CodePalette* palette = static_cast<CodePalette*>(parent());
    if ( palette && m_edit->bindingSpan() ){
        m_edit->bindingSpan()->commit(palette->value());
    }
}

QString CodeConverter::buildCode(const QJSValue &value){
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
            return value.property("__ref").toString();
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
