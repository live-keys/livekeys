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

#include "metaobjecttypeinfo.h"
#include "live/elements/metaobject.h"

namespace lv{ namespace el{

TypeInfo::Ptr MetaObjectTypeInfo::extract(const MetaObject &mo, const Utf8 &uri, bool isInstance, bool isCreatable){
    std::string name = uri.length() == 0 ? "u/" + mo.name() : "lv/" + uri.data() + "#" + mo.name();

    TypeInfo::Ptr ti = TypeInfo::create(name, mo.base() ? "cpp/" + mo.base()->fullName() : "", isInstance, isCreatable);
    ti->setClassName("cpp/" + mo.fullName());

    // Constructor
    if ( mo.constructor() ){
        Constructor* c = mo.constructor();
        ti->setConstructor(FunctionInfo::extractFromDeclaration("constructor", c->getDeclaration()));
    }

    // Properties
    for ( auto propIt = mo.ownPropertiesBegin(); propIt != mo.ownPropertiesEnd(); ++propIt ){
        Property* p = propIt->second;
        ti->addProperty(PropertyInfo(p->name(), "cpp/" + p->type()));
    }

    // Methods
    for ( auto funcIt = mo.ownMethodsBegin(); funcIt != mo.ownMethodsEnd(); ++funcIt ){
        Function* f = funcIt->second;
        ti->addMethod(FunctionInfo::extractFromDeclaration(funcIt->first, f->getDeclaration()));
    }

    // Functions
    for ( auto funcIt = mo.functionsBegin(); funcIt != mo.functionsEnd(); ++funcIt ){
        Function* f = funcIt->second;
        ti->addFunction(FunctionInfo::extractFromDeclaration(funcIt->first, f->getDeclaration()));
    }

    // Events
    for ( auto eventIt = mo.ownEventsBegin(); eventIt != mo.ownEventsEnd(); ++eventIt ){
        Function* f = eventIt->second;
        ti->addEvent(FunctionInfo::extractFromDeclaration(eventIt->first, f->getDeclaration()));
    }

    return ti;
}


}}// namespace lv, el
