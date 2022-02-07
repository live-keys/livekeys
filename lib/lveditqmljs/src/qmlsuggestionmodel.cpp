/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include "qmlsuggestionmodel.h"
#include "live/visuallogqt.h"
#include <QSet>
#include <languageqmlhandler.h>

namespace lv {

QmlSuggestionModel::QmlSuggestionModel(int addPosition, QObject* parent)
    : QAbstractListModel(parent)
    , m_categoryFilter(0)
    , m_supportsObjectNesting(false)
    , m_addPosition(addPosition)
{
    m_roles[Label]          = "label";
    m_roles[ObjectType]     = "objectType";
    m_roles[Type]           = "type";
    m_roles[ImportSpace]    = "importSpace";
    m_roles[Documentation]  = "documentation";
    m_roles[Code]           = "code";
    m_roles[Category]       = "category";
    m_roles[IsGroup]        = "isGroup";
    m_roles[IsWritable]     = "isWritable";
}

QmlSuggestionModel::~QmlSuggestionModel()
{

}

QVariant QmlSuggestionModel::data(const QModelIndex &index, int role) const
{
    int dataIndex = m_filteredData[index.row()];
    if ( role == Label ){
        return m_data[dataIndex].label;
    } else if ( role == Type ){
        return m_data[dataIndex].type;
    } else if ( role == ObjectType ){
        return m_data[dataIndex].objectType;
    } else if ( role == Documentation ){
        return m_data[dataIndex].documentation;
    } else if ( role == Code ){
        return m_data[dataIndex].code;
    } else if ( role == ImportSpace ){
        return m_data[dataIndex].importSpace;
    } else if ( role == Category ){
        return m_data[dataIndex].category;
    } else if ( role == IsGroup ){
        return m_data[dataIndex].isGroup;
    } else if ( role == IsWritable ){
        return m_data[dataIndex].isWritable;
    }
    return QVariant();
}

void QmlSuggestionModel::addItem(const QmlSuggestionModel::ItemData &item)
{
    m_data.append(item);
}

void QmlSuggestionModel::setFilter(const QString &filter)
{
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlSuggestionModel::setImportFilter(const QString &importFilter)
{
    if ( m_importFilter == importFilter )
        return;

    m_importFilter = importFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlSuggestionModel::setTypeFilter(const QString &typeFilter)
{
    if ( m_typeFilter == typeFilter )
        return;

    m_typeFilter = typeFilter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QmlSuggestionModel::setCategoryFilter(int cat)
{
    if ( m_categoryFilter == cat )
        return;

    m_categoryFilter = cat;
    beginResetModel();
    updateFilters();
    endResetModel();
}

QStringList QmlSuggestionModel::importSpaces() const
{
    QSet<QString> cat;
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        if (!it->importSpace.isEmpty())
            cat.insert(it->importSpace);
    }

    QStringList res;
    res << "All";
    foreach( const QString& c, cat ){
        res << c;
    }
    return res;
}

QStringList QmlSuggestionModel::types() const{
    QSet<QString> types;
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        if (!it->objectType.isEmpty())
            types.insert(it->objectType);
    }

    QStringList res;
    res << "All";
    foreach( const QString& c, types ){
        res << c;
    }
    return res;
}

void QmlSuggestionModel::updateFilters(){
    m_filteredData.clear();
    for ( int i = 0; i < m_data.size(); ++i ){
        bool filter = m_data[i].label.startsWith(m_filter, Qt::CaseInsensitive);
        bool typeFilter = m_typeFilter.isEmpty() ? true : m_data[i].objectType == m_typeFilter;
        bool importFilter = m_importFilter.isEmpty() ? true : m_data[i].importSpace == m_importFilter;
        bool categoryFilter = m_categoryFilter == 0 ? true : m_data[i].category == m_categoryFilter;
        if ( filter && typeFilter && importFilter && categoryFilter)
            m_filteredData.append(i);
    }
}

void QmlSuggestionModel::addPropertiesAndFunctionsToModel(const QmlInheritanceInfo &typePath){

    QmlPropertyInfo qpi = typePath.defaultProperty();
    if ( qpi.isValid() ){
        m_supportsObjectNesting = true;
    }

    for ( auto it = typePath.nodes.begin(); it != typePath.nodes.end(); ++it ){
        const QmlTypeInfo::Ptr& ti = *it;

        for (int i = 0; i < ti->totalFunctions(); ++i)
        {
            auto method = ti->functionAt(i);
            if (method.functionType == QmlFunctionInfo::Signal)
            {
                auto name = method.name;
                name = QString("on") + name[0].toUpper() + name.mid(1);

                addItem(QmlSuggestionModel::ItemData(
                    name,
                    ti->prefereredType().name(),
                    "method",
                    "",
                    ti->exportType().join() + "." + name,
                    name,
                    QmlSuggestionModel::ItemData::Event)
                );
            } else if ( method.functionType != QmlFunctionInfo::SlotGenerated ){
                auto name = method.name;

                addItem(QmlSuggestionModel::ItemData(
                    name,
                    ti->prefereredType().name(),
                    "method",
                    "",
                    ti->exportType().join() + "." + name,
                    name,
                    QmlSuggestionModel::ItemData::Function)
                );

            }
        }

        for ( int i = 0; i < ti->totalProperties(); ++i ){
            QString propertyName = ti->propertyAt(i).name;

            if ( !propertyName.startsWith("__") )
//                (
//                    (!(filter & LanguageQmlHandler::NoReadOnly) || ti->propertyAt(i).isWritable)
//                    ||
//                    ((filter & LanguageQmlHandler::ReadOnly) && ti->propertyAt(i).isPointer)

//                ))
            {

                addItem(
                    QmlSuggestionModel::ItemData(
                        propertyName,
                        ti->prefereredType().name(),
                        ti->propertyAt(i).typeName.name(),
                        "",
                        ti->exportType().join() + "." + propertyName,
                        propertyName,
                        QmlSuggestionModel::ItemData::Property,
                        ti->propertyAt(i).isPointer,
                        ti->propertyAt(i).isWritable)
                );
            }


            if ( propertyName.size() > 0 )
                propertyName[0] = propertyName[0].toUpper();

            if (propertyName != "ObjectName"){
                propertyName = "on" + propertyName + "Changed";
                addItem(
                    QmlSuggestionModel::ItemData(
                        propertyName,
                        ti->prefereredType().name(),
                        "method",
                        "",
                        "", //TODO: Find library path
                        propertyName,
                        QmlSuggestionModel::ItemData::Event)
                );
            }
        }
        updateFilters();
    }
}

void QmlSuggestionModel::addObjectsToModel(const QmlScopeSnap &scope, bool addCreatable, bool addSingletons)
{
    // import global objects

    QmlLibraryInfo::Ptr lib = scope.project->libraryInfo(scope.document->path());

    for ( auto it = lib->exports().begin(); it != lib->exports().end(); ++it ){
        if ( it.key() != scope.document->componentName() ){
            addItem(
                QmlSuggestionModel::ItemData(
                    it.key(),
                    "",
                    "",
                    "implicit",
                    scope.document->path(),
                    it.key(),
                    QmlSuggestionModel::ItemData::Object)
            );
        }
    }

    for( const QString& defaultLibrary: scope.project->defaultLibraries() ){
        QmlLibraryInfo::Ptr defaultLib = scope.project->libraryInfo(defaultLibrary);
        if ( defaultLib ){
            for ( auto it = defaultLib->exports().begin(); it != defaultLib->exports().end(); ++it ){
                QmlTypeInfo::Ptr typeInfo = it.value();
                bool add =
                        (typeInfo->isCreatable() && addCreatable) ||
                        (typeInfo->isSingleton() && addSingletons);

                if ( typeInfo->exportType().name() == "Component" )
                    add = false;

                if ( add ){
                    addItem(QmlSuggestionModel::ItemData(
                            it.value()->exportType().name(),
                            "",
                            "",
                            "QtQml",
                            "QtQml",
                            it.value()->exportType().name(),
                            QmlSuggestionModel::ItemData::Object
                    ));
                }
            }
        }
    }

    // import namespace objects

    QSet<QString> imports;

    foreach( const DocumentQmlInfo::Import& imp, scope.document->imports() ){
        if ( imp.as() != "" ){
            imports.insert(imp.as());
        }
    }
    imports.insert("");

    for ( QSet<QString>::iterator it = imports.begin(); it != imports.end(); ++it ){

        foreach( const DocumentQmlInfo::Import& imp, scope.document->imports() ){
            if ( imp.as() == *it ){
                QmlLibraryInfo::Ptr implib = scope.project->libraryInfo(imp.uri());
                if ( !implib.isNull() ){
                    for ( auto it = implib->exports().begin(); it != implib->exports().end(); ++it ){
                        QmlTypeInfo::Ptr typeInfo = it.value();
                        bool add =
                                (typeInfo->isCreatable() && addCreatable) ||
                                (typeInfo->isSingleton() && addSingletons);

                        if ( typeInfo->exportType().name() == "Component" )
                            add = false;

                        if ( add ){
                            addItem(QmlSuggestionModel::ItemData(
                                typeInfo->exportType().name(),
                                "",
                                "",
                                imp.uri(),
                                imp.uri(),
                                typeInfo->exportType().name(),
                                QmlSuggestionModel::ItemData::Object
                            ));
                        }
                    }
                }
            }
        }
    }

    updateFilters();
}


QmlSuggestionModel::ItemData::ItemData(const QString& plabel, const QString& pObjType, const QString& ptype, const QString &pimport, const QString &pdoc, const QString &pcode, const int cat, bool group, bool writable)
    : label(plabel)
    , objectType(pObjType)
    , type(ptype)
    , importSpace(pimport)
    , documentation(pdoc)
    , code(pcode)
    , category(cat)
    , isGroup(group)
    , isWritable(writable)
{

}

}
