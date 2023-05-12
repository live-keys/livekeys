#include "languagenodestojs_p.h"
#include "live/exception.h"
#include "live/visuallog.h"

namespace lv{ namespace el{


std::string LanguageNodesToJs::indent(int i){
    std::string res;
    res.append(4 * i, ' ');
    return res;
}

LanguageNodesToJs::LanguageNodesToJs()
{

}

std::string LanguageNodesToJs::slice(const std::string &source, uint32_t start, uint32_t end){
    return source.substr(start, end - start);
}

std::string LanguageNodesToJs::slice(const std::string &source, BaseNode *node){
    return slice(source, node->startByte(), node->endByte());
}

void LanguageNodesToJs::convert(BaseNode* node, const std::string &source, std::vector<ElementsInsertion *> &sections, int indentValue, BaseNode::ConversionContext *ctx){
    if ( node->isNodeType<ProgramNode>() ){
        convertProgram(node->as<ProgramNode>(), source, sections, indentValue, ctx);
    } else if ( node->isNodeType<ComponentDeclarationNode>() ){
        convertComponentDeclaration(node->as<ComponentDeclarationNode>(), source, sections, indentValue, ctx);
    } else if ( node->isNodeType<NewComponentExpressionNode>() ){
        convertNewComponentExpression(node->as<NewComponentExpressionNode>(), source, sections, indentValue, ctx);
    } else if ( node->isNodeType<RootNewComponentExpressionNode>() ){
        convertNewComponentExpression(node->as<NewComponentExpressionNode>(), source, sections, indentValue, ctx);
    } else if ( node->isNodeType<NewTaggedComponentExpressionNode>() ){
        convertNewTaggedComponentExpression(node->as<NewTaggedComponentExpressionNode>(), source, sections, indentValue, ctx);
    } else if ( node->isNodeType<NewTrippleTaggedComponentExpressionNode>() ){
        convertNewTrippleTaggedComponentExpression(node->as<NewTrippleTaggedComponentExpressionNode>(), source, sections, indentValue, ctx);
    } else {
        for ( BaseNode* child : node->children() ){
            convert(child, source, sections, indentValue, ctx);
        }
    }
}

void LanguageNodesToJs::convertProgram(ProgramNode *node, const std::string &source, std::vector<ElementsInsertion *> &sections, int indentValue, BaseNode::ConversionContext *ctx){

    if ( ctx && !ctx->jsImportsEnabled && !node->jsImports().empty() ){
        THROW_EXCEPTION(lv::Exception, "Javascript imports are not enabled.", lv::Exception::toCode("~Enabled"));
    }

    ElementsInsertion* importsCompose = new ElementsInsertion;
    importsCompose->from = 0;
    int to = 0;
    for ( ImportNode* node: node->imports() ){
        if ( node->endByte() > to )
            to = node->endByte();
    }
    for ( JsImportNode* node : node->jsImports() ){
        if ( node->endByte() > to )
            to = node->endByte();
    }
    importsCompose->to = to;

    for ( JsImportNode* node : node->jsImports() ){
        std::string importsNames;
        for ( IdentifierNode* nameNode : node->importNames() ){
            if ( !importsNames.empty() )
                importsNames += ",";
            importsNames += slice(source, nameNode);
        }

        std::string importPath = slice(source, node->importPath());
        if ( importPath.length() > 2 )
            importPath = importPath.substr(1, importPath.length() - 2);
        if ( ctx && importPath.length() > 2 ){
            if ( !ctx->relativePathFromBuild.empty() && importPath[0] == '.' && (importPath[1] == '.' || importPath[1] == '/') ){
                importPath = ctx->relativePathFromBuild + "/" + importPath;
            }
        }

        *importsCompose << "import {" << importsNames << "} from \'" << importPath << "\'\n";
    }

    if ( ctx && !ctx->baseComponentImportUri.empty() && !ctx->baseComponent.empty() ){
        ProgramNode::ImportType it;
        it.name = ctx->baseComponent;
        it.resolvedPath = ctx->baseComponentImportUri;
        node->addImportType(it);
    }

    for ( auto it = node->importTypes().begin(); it != node->importTypes().end(); ++it ){
        if ( it->first.empty() ){
            for ( auto impIt = it->second.begin(); impIt != it->second.end(); ++ impIt ){
                if ( !ctx->allowUnresolved && impIt->second.resolvedPath.empty() ){
                    delete importsCompose;
                    THROW_EXCEPTION(lv::Exception, "Identifer not found in imports: \'" + impIt->second.name + "\' in " + node->filePath(), lv::Exception::toCode("~Identifier"));
                }
                std::string impPath = impIt->second.resolvedPath.empty() ? "__UNRESOLVED__" : impIt->second.resolvedPath;
                *importsCompose << ("import {" + impIt->second.name + "} from '" + impPath + "'\n");
            }
        } else {
            std::string moduleWrap = "let " + it->first + " = {";
            for ( auto impIt = it->second.begin(); impIt != it->second.end(); ++ impIt ){
                if ( !ctx->allowUnresolved && impIt->second.resolvedPath.empty() ){
                    delete importsCompose;
                    THROW_EXCEPTION(lv::Exception, "Identifer not found in imports: \'" + impIt->second.name + "\' in " + node->filePath(), lv::Exception::toCode("~Identifier"));
                }
                std::string impPath = impIt->second.resolvedPath.empty() ? "__UNRESOLVED__" : impIt->second.resolvedPath;
                std::string impKey = "__" + it->first + "__" + impIt->second.name;
                std::string impName = impIt->second.name;

                if ( impIt != it->second.begin() )
                    moduleWrap += ", ";
                moduleWrap += impIt->second.name + ":" + impKey;

                *importsCompose << ("import {" + impName + " as " + impKey + "} from '" + impPath + "'\n");
            }
            moduleWrap += "}\n";
            *importsCompose << moduleWrap;
        }
    }

    sections.push_back(importsCompose);

    size_t offset = sections.size();

    for ( BaseNode* child: node->exports() )
        convert(child, source, sections, indentValue, ctx);

//    BaseNode::convertToJs(source, fragments, indentValue);

    if (node->idComponents().empty())
        return;

    auto iter = sections.begin() + offset;
    ElementsInsertion* compose = new ElementsInsertion;
    if (offset == sections.size()) {
        compose->from = compose->to = static_cast<int>(source.length());
    } else {
        compose->from = compose->to = (*iter)->from;
    }

    sections.insert(iter, compose);
}

void LanguageNodesToJs::convertComponentDeclaration(ComponentDeclarationNode *node, const std::string &source, std::vector<ElementsInsertion *> &sections, int indentValue, BaseNode::ConversionContext *ctx){
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = node->startByte();
    compose->to   = node->endByte();

    std::string componentName = node->name(source);

    std::string heritage = "";
    if ( node->heritage().size() > 0 ){
        heritage = slice(source, node->heritage()[0]);
        for ( size_t i = 1; i < node->heritage().size(); ++i ){
            heritage += "." + slice(source, node->heritage()[i]);
        }
    }

    if ( heritage.empty() )
        heritage = BaseNode::ConversionContext::baseComponentName(ctx);

    if (node->parent() && node->parent()->isNodeType<ProgramNode>() ){
        *compose << "\n" << indent(indentValue) << "export class ";
    } else {
        *compose << "class ";
    }

    *compose << (node->isAnonymous() ? "" : componentName + " ") << "extends " + heritage + "{\n\n";

    // handle constructor
    if (node->componentBody()->constructor()){
        *compose << indent(indentValue + 1) <<  "constructor";
        if ( node->componentBody()->constructor()->parameters() && node->componentBody()->constructor()->parameters()->parameters().size() ){
            std::string params = "(";
            for (size_t i = 0; i != node->componentBody()->constructor()->parameters()->parameters().size(); ++i){
                if ( i != 0 )
                    params += ",";
                ParameterNode* param = node->componentBody()->constructor()->parameters()->parameters()[i];
                params += slice(source, param->identifier());
            }
            params += ")";
            *compose << params;
        } else {
            *compose << "()";
        }

        *compose << slice(source, node->componentBody()->constructor()->body()->startByte(), node->componentBody()->constructor()->body()->endByte()) << "\n";

    } else {
        *compose << indent(indentValue + 1) << "constructor(){\n"
                 << indent(indentValue + 2) << "super()\n"
                 << indent(indentValue + 2) << (node->isAnonymous() ? "new.target" : componentName) << ".prototype.__initialize.call(this)\n"
                 << indent(indentValue + 1) << "}\n";
    }

    *compose << indent(indentValue + 1) << "__initialize(){\n";

    if (node->componentId() || !node->idComponents().empty())
        *compose << indent(indentValue + 2) << "this.ids = {}\n\n";

    if (node->componentId()){
        *compose << indent(indentValue + 2) << "var " << slice(source, node->componentId()) << " = this\n";
        *compose << indent(indentValue + 2) << "this.ids[\"" << slice(source, node->componentId()) << "\"] = " << slice(source, node->componentId()) << "\n\n";
    }

    for (size_t i = 0; i < node->idComponents().size(); ++i)
    {
        *compose << indent(indentValue + 2) << "var " << slice(source, node->idComponents()[i]->id()) << " = new " << node->idComponents()[i]->initializerName(source);
        if (node->idComponents()[i]->arguments())
            *compose << slice(source, node->idComponents()[i]->arguments()) << "\n";
        else
            *compose << "()\n";
        *compose << indent(indentValue + 2) << "this.ids[\"" << slice(source, node->idComponents()[i]->id()) << "\"] = " << slice(source, node->idComponents()[i]->id()) << "\n\n";
    }

    for (size_t i = 0; i < node->idComponents().size();++i)
    {
        std::string id = slice(source, node->idComponents()[i]->id());
        auto properties = node->idComponents()[i]->properties();

        for (uint32_t idx = 0; idx < properties.size(); ++idx){
            std::string propertyName = slice(source, properties[idx]->name());
            PropertyAccessorDeclarationNode::PropertyAccess accessPair;
            convertPropertyDeclaration(properties[idx], source, id, indentValue + 2, ctx, accessPair, compose);
        }
    }

    for (size_t i = 0; i < node->properties().size(); ++i){
        std::string propertyName = slice(source, node->properties()[i]->name());
        PropertyAccessorDeclarationNode::PropertyAccess accessPair = node->propertyAccessors(source, propertyName);
        convertPropertyDeclaration(node->properties()[i], source, "this", indentValue + 2, ctx, accessPair, compose);
    }

    for (size_t i = 0; i < node->events().size(); ++i){
        std::string paramList = "";
        if ( node->events()[i]->parameterList() ){
            ParameterListNode* pdn = node->events()[i]->parameterList()->as<ParameterListNode>();
            for ( auto it = pdn->parameters().begin(); it != pdn->parameters().end(); ++it ){
                if ( it != pdn->parameters().begin() )
                    paramList += ",";
                paramList += "[\'" + TypeNode::sliceWithoutAnnotation(source, (*it)->type()) + "\',\'" + slice(source, (*it)->identifier()) + "\']";
            }
        }

        *compose << indent(indentValue + 1) << (BaseNode::ConversionContext::baseComponentName(ctx) + ".addEvent(this, \'" + slice(source, node->events()[i]->name()) + "\', [" + paramList + "])\n");
    }

    for (size_t i = 0; i < node->listeners().size(); ++i){

        std::string paramList = "";
        if ( node->listeners()[i]->parameterList() ){
            ParameterListNode* pdn = node->listeners()[i]->parameterList()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }

        *compose << indent(indentValue + 2) << "this.on(\'" << slice(source, node->listeners()[i]->name()) << "\', function(" << paramList << ")";

        if ( node->listeners()[i]->body() ){
            JSSection* jssection = new JSSection;
            jssection->from = node->listeners()[i]->body()->startByte();
            jssection->to   = node->listeners()[i]->body()->endByte();
            convert(node->listeners()[i]->body(), source, jssection->m_children, indentValue + 1, ctx);
            *compose << jssection << ".bind(this));\n";
        } else {
            JSSection* jssection = new JSSection;
            jssection->from = node->listeners()[i]->bodyExpression()->startByte();
            jssection->to   = node->listeners()[i]->bodyExpression()->endByte();
            convert(node->listeners()[i]->bodyExpression(), source, jssection->m_children, indentValue + 1, ctx);
            *compose << "{" << jssection << "}.bind(this));\n";
        }
    }

    for (size_t i = 0; i < node->properties().size(); ++i){

        std::string bindingsInJs = node->properties()[i]->bindingIdentifiersToJs(source);

        if (bindingsInJs.size() > 0 && node->properties()[i]->isBindingsAssignment() ){
            std::string comp = indent(indentValue + 1) + BaseNode::ConversionContext::baseComponentName(ctx) + ".assignPropertyExpression(this,\n"
                             + indent(indentValue + 1) + "'" + slice(source, node->properties()[i]->name()) + "',\n";
            if (node->properties()[i]->expression()){
                auto expr = node->properties()[i]->expression();
                comp += indent(indentValue + 1) + "function(){ return ";
                el::JSSection* section = new el::JSSection;
                section->from = expr->startByte();
                section->to = expr->endByte();
                convert(expr, source, section->m_children, indentValue + 1, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    comp += s;
                delete section;
                comp += "}.bind(this),\n";
            } else if (node->properties()[i]->statementBlock()){
                auto block = node->properties()[i]->statementBlock();
                comp += indent(indentValue + 1) + "(function()\n" ;
                el::JSSection* section = new el::JSSection;
                section->from = block->startByte();
                section->to = block->endByte();
                convert(block, source, section->m_children, indentValue + 1, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    comp += s;
                delete section;
                comp += ".bind(this)\n),\n";
            }
            comp += indent(indentValue + 1) + bindingsInJs + "\n";
            comp += indent(indentValue + 1) + ")\n";
            *compose << comp;
        } else if ( node->properties()[i]->hasAssignment() ){
            *compose << indent(indentValue + 2) << "this." << slice(source,node->properties()[i]->name())
                     << " = ";
            if (node->properties()[i]->expression()){
                auto expr = node->properties()[i]->expression();
                // convert the subexpression
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                convert(expr, source, expressionSection->m_children, indentValue + 2, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete expressionSection;

                *compose << "\n";
            } else if (node->properties()[i]->statementBlock()){
                auto block = node->properties()[i]->statementBlock();
                *compose << "(function()" ;
                el::JSSection* section = new el::JSSection;
                section->from = block->startByte();
                section->to = block->endByte();
                convert(block, source, section->m_children, indentValue + 1, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete section;
                *compose << ".bind(this))()\n";
            }
        }
    }

    for (size_t i = 0; i < node->assignments().size(); ++i){

        std::string bindingsInJs = node->assignments()[i]->bindingIdentifiersToJs(source);

        if ( bindingsInJs.length() > 0 && node->assignments()[i]->isBindingAssignment() ){
            if (node->assignments()[i]->expression()){
                auto& property = node->assignments()[i]->property();
                std::string object = "this";
                for (size_t x = 0; x < property.size()-1; x++){
                    object += "." + slice(source, property[x]);
                }

                *compose << indent(indentValue + 2) << BaseNode::ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indentValue + 3) << "'" << slice(source, property[property.size()-1])
                         << "',\n" << indent(indentValue + 3) << "function(){ return ";

                auto expr = node->assignments()[i]->expression();
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                convert(expr, source, expressionSection->m_children, indentValue + 4, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete expressionSection;

                *compose << "}.bind(" << object << "),\n"
                         << indent(indentValue + 3) << bindingsInJs << "\n"
                         << indent(indentValue + 2) << ")\n";
            } else if ( node->assignments()[i]->statementBlock() ){
                auto& property = node->assignments()[i]->property();
                std::string object = "this";
                for (size_t x = 0; x < property.size()-1; x++){
                    object += "." + slice(source, property[x]);
                }

                *compose << indent(indentValue + 2) << BaseNode::ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indentValue + 3) << "'" << slice(source, property[property.size() - 1])
                         << "',\n" << indent(indentValue + 3) << "function()";

                auto expr = node->assignments()[i]->statementBlock();
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                convert(expr, source, expressionSection->m_children, indentValue + 4, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete expressionSection;

                *compose << ".bind(" << object << "),\n"
                         << indent(indentValue + 2) << bindingsInJs << "\n"
                         << indent(indentValue + 1) << ")\n";
            }
        } else {
            if (node->assignments()[i]->expression() && !node->assignments()[i]->property().empty()){
                *compose << indent(indentValue + 2) << "this";

                for (size_t prop = 0; prop < node->assignments()[i]->property().size(); ++prop){
                    *compose << "." << slice(source, node->assignments()[i]->property()[prop]);
                }

                auto expr = node->assignments()[i]->expression();
                std::string comp = "" ;
                el::JSSection* section = new el::JSSection;
                section->from = expr->startByte();
                section->to = expr->endByte();
                convert(expr, source, section->m_children, indentValue + 1, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat){
                    comp += s;
                }
                delete section;

                *compose << " = " << comp << "\n"; //slice(source, m_assignments[i]->m_expression) << "\n\n";
            }
            else if (node->assignments()[i]->statementBlock() ) {
                std::string propName = "this";

                for (size_t prop = 0; prop < node->assignments()[i]->property().size(); ++prop){
                    propName += "." + slice(source, node->assignments()[i]->property()[prop]);
                }
                *compose << indent(indentValue + 2) << propName << " = " << "(function()";

                auto block = node->assignments()[i]->statementBlock();

                el::JSSection* section = new el::JSSection;
                section->from = block->startByte();
                section->to = block->endByte();
                convert(block, source, section->m_children, indentValue + 3, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat) {
                    *compose << s;
                }
                delete section;

                *compose << ".bind(this)())\n\n";
            }
        }
    }

    if (!node->nestedComponents().empty()){
        *compose << indent(indentValue + 2) << BaseNode::ConversionContext::baseComponentName(ctx) << ".assignChildren(this, [\n";
        for (size_t i = 0; i < node->nestedComponents().size(); ++i)
        {
            if (i != 0)
                *compose << indent(indentValue + 3) << ",\n";
            el::JSSection* section = new el::JSSection;
            section->from = node->nestedComponents()[i]->startByte();
            section->to = node->nestedComponents()[i]->endByte();
            convert(node->nestedComponents()[i], source, section->m_children, indentValue + 3, ctx);
            std::vector<std::string> flat;
            section->flatten(source, flat);
            for (auto s: flat)
                *compose << s;
            delete section;
        }
        *compose << indent(indentValue + 2) << "])\n";
    }

    *compose << indent(indentValue + 1) << "}\n\n";

    for (size_t i = 0; i < node->propertyAccessors().size(); ++i){
        PropertyAccessorDeclarationNode* pa = node->propertyAccessors()[i];
        if ( !pa->isPropertyAttached() ){
            std::string header;
            if ( pa->access() == PropertyAccessorDeclarationNode::Getter ){
                header += "get " + slice(source, pa->name()) + "()";
            } else if ( pa->access() == PropertyAccessorDeclarationNode::Setter ){
                std::string param;
                if ( pa->parameters() ){
                    ParameterListNode* pdn = pa->parameters()->as<ParameterListNode>();
                    if  ( pdn->parameters().size() > 0 ){
                        param += slice(source, pdn->parameters()[0]->identifier());
                    }
                }
                header += "set " + BaseNode::slice(source, pa->name()) + "(" + param + ")";
            }

            *compose << indent(indentValue + 1) << header;

            JSSection* jssection = new JSSection;
            jssection->from = pa->body()->startByte();
            jssection->to   = pa->body()->endByte();
            convert(pa->body(), source, jssection->m_children, indentValue + 1, ctx);
            std::vector<std::string> flat;
            jssection->flatten(source, flat);
            for (auto s: flat){
                *compose << s << "\n";
            }
            delete jssection;
            *compose << "\n";
        }
    }

    for ( auto it = node->methods().begin(); it != node->methods().end(); ++it ){
        TypedMethodDeclarationNode* tfdn = *it;

        std::string paramList = "";
        if ( tfdn->parameters() ){
            ParameterListNode* pdn = tfdn->parameters()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }
        std::string annotations = "";
        if ( tfdn->isStatic() )
            annotations += "static ";
        if ( tfdn->isAsync() )
            annotations += "async ";
        *compose << indent(indentValue + 1) << annotations << slice(source, tfdn->name()) << "(" << paramList << ")";

        JSSection* jssection = new JSSection;
        jssection->from = tfdn->body()->startByte();
        jssection->to   = tfdn->body()->endByte();
        convert(tfdn->body(), source, jssection->m_children, indentValue + 1, ctx);
        *compose << jssection << "\n";
    }

    *compose << indent(indentValue) << "}\n";

    for ( auto it = node->staticProperties().begin(); it != node->staticProperties().end(); ++it ){
        StaticPropertyDeclarationNode* spd = (*it)->as<StaticPropertyDeclarationNode>();
        *compose << indent(indentValue)  << componentName << "." << slice(source, spd->name());
        if ( spd->expression() ){
            *compose << " = ";
            JSSection* jssection = new JSSection;
            jssection->from = spd->expression()->startByte();
            jssection->to   = spd->expression()->endByte();
            convert(spd, source, jssection->m_children, indentValue + 1, ctx);
            *compose << jssection << "\n";
        } else {
            *compose << "\n";
        }
    }

    sections.push_back(compose);
}

void LanguageNodesToJs::convertNewComponentExpression(NewComponentExpressionNode *node, const std::string &source, std::vector<ElementsInsertion *> &sections, int indt, BaseNode::ConversionContext *ctx)
{
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = node->startByte();
    compose->to   = node->endByte();

    if ( node->parent() && node->parent()->isNodeType<ComponentInstanceStatementNode>() ){
        compose->from = node->parent()->startByte();
        std::string instanceName = node->parent()->as<ComponentInstanceStatementNode>()->name(source);
        *compose << "\nexport let " << instanceName << " = ";
    }
    *compose << indent(indt) << "(function(parent){\n" << indent(indt + 1) << "this.setParent(parent)\n";

    std::string id_root = "this";
    bool isRoot = (dynamic_cast<RootNewComponentExpressionNode*>(node) != nullptr);
    if (isRoot && (!node->idComponents().empty() || node->id() ) ) {
        *compose << indent(indt + 1) << "this.ids = {}\n\n";
    }

    if (node->id()) {
        *compose << indent(indt + 1) << BaseNode::ConversionContext::baseComponentName(ctx) << ".assignId(" << slice(source, node->id()) << ", \"" << slice(source, node->id()) << "\")\n";
        if (isRoot){
            id_root = slice(source, node->id());
            *compose << indent(indt + 1) << "var " << id_root << " = this\n";
            *compose << indent(indt + 1) << "this.ids[\"" << id_root << "\"] = " << id_root << "\n";
        }
    }

    if (isRoot && !node->idComponents().empty()){
        for (size_t i = 0; i < node->idComponents().size();++i){
            auto type = node->idComponents()[i]->initializerName(source);
            *compose << indent(indt + 1) << "var " << slice(source, node->idComponents()[i]->id()) << " = new " << type;
            if (node->idComponents()[i]->arguments())
                *compose << slice(source, node->idComponents()[i]->arguments()) << "\n";
            else
                *compose << "()\n";
            *compose << indent(indt + 1) << "this.ids[\"" << slice(source, node->idComponents()[i]->id()) << "\"] = " << slice(source, node->idComponents()[i]->id()) << "\n\n";
        }
    }

    if (isRoot || !node->id()){
        for (size_t i = 0; i < node->properties().size(); ++i){
            std::string propertyName = slice(source, node->properties()[i]->name());
            PropertyAccessorDeclarationNode::PropertyAccess accessPair;
            convertPropertyDeclaration(node->properties()[i], source, id_root, indt + 2, ctx, accessPair, compose);
        }
    }

    if (isRoot && !node->idComponents().empty()){
        for (size_t i = 0; i < node->idComponents().size();++i){
            std::string id = slice(source, node->idComponents()[i]->id());
            auto properties = node->idComponents()[i]->properties();

            for (size_t idx = 0; idx < properties.size(); ++idx){
                std::string propertyName = slice(source, properties[idx]->name());
                PropertyAccessorDeclarationNode::PropertyAccess accessPair;
                convertPropertyDeclaration(properties[idx], source, id, indt + 1, ctx, accessPair, compose);
            }
        }
    }

    for (size_t i = 0; i < node->properties().size(); ++i){

        std::string bindingsInJs = node->properties()[i]->bindingIdentifiersToJs(source);
        if (bindingsInJs.size() > 0 && node->properties()[i]->isBindingsAssignment() ){
            std::string comp = "";
            if (node->properties()[i]->expression()){
                comp += indent(indt + 1) + BaseNode::ConversionContext::baseComponentName(ctx) + ".assignPropertyExpression(this,\n"
                      + indent(indt + 2) + "'" + slice(source, node->properties()[i]->name()) + "',\n"
                      + indent(indt + 2) + "function(){ return ";

                JSSection* expressionSection = new JSSection;
                expressionSection->from = node->properties()[i]->expression()->startByte();
                expressionSection->to   = node->properties()[i]->expression()->endByte();
                convert(node->properties()[i], source, expressionSection->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    comp += s;

                comp += "}.bind(this),\n"
                      + indent(indt + 2) + bindingsInJs + "\n";
                comp += indent(indt + 1) +  + ")\n";
            } else {
                comp += indent(indt + 1) + BaseNode::ConversionContext::baseComponentName(ctx) + ".assignPropertyExpression(this,\n"
                      + indent(indt + 2) + "'" + slice(source, node->properties()[i]->name()) + "',\n" + indent(indt + 2) + "function()";
                el::JSSection* section = new el::JSSection;
                auto block = node->properties()[i]->statementBlock();
                section->from = block->startByte();
                section->to = block->endByte();
                convert(block, source, section->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    comp += s;
                delete section;
                comp += ".bind(this),\n"
                      + indent(indt + 1) + bindingsInJs + "\n";
                comp += indent(indt + 1) + ")\n";
            }

            *compose << comp;
        } else if ( node->properties()[i]->hasAssignment() ){
            if (node->properties()[i]->expression()){
                auto expr = node->properties()[i]->expression();
                *compose << indent(indt + 1) << "this." << slice(source, node->properties()[i]->name()) << " = ";

                // convert the subexpression
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                convert(expr, source, expressionSection->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;

                *compose << "\n";
            } else if (node->properties()[i]->statementBlock()) {
                *compose << indent(indt + 1) << "this." << slice(source, node->properties()[i]->name()) << " = " << "(function()";
                el::JSSection* section = new el::JSSection;
                auto block = node->properties()[i]->statementBlock();
                section->from = block->startByte();
                section->to = block->endByte();
                convert(block, source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << ".bind(this)())\n\n";
            }
        }
    }

    for (size_t i = 0; i < node->assignments().size(); ++i){
        if ( node->assignments()[i]->property().size() == 0 )
            continue;

        std::string bindingsInJs = node->assignments()[i]->bindingIdentifiersToJs(source);

        if ( bindingsInJs.length() > 0 ){ // has bindings
            if (node->assignments()[i]->expression() ){
                auto& property = node->assignments()[i]->property();
                std::string object = "this";
                for (size_t x = 0; x < property.size() - 1; x++){
                    object += "." + slice(source, property[x]);
                }
                *compose << indent(indt + 1) << BaseNode::ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indt + 2) << "'" << slice(source, property[property.size()-1]) << "',\n"
                         << indent(indt + 2) << "function(){ return ";

                el::JSSection* section = new el::JSSection;
                auto expr = node->assignments()[i]->expression();
                section->from = expr->startByte();
                section->to = expr->endByte();
                convert(expr, source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << "}.bind(" << object << "),\n"
                         << indent(indt + 2) << bindingsInJs << "\n"
                         << indent(indt + 1) << ")\n";
            } else if ( node->assignments()[i]->statementBlock() ){
                auto& property = node->assignments()[i]->property();
                std::string object = "this";
                for (size_t x = 0; x < property.size() - 1; x++){
                    object += "." + slice(source, property[x]);
                }
                *compose << indent(indt + 1) << BaseNode::ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indt + 2) << "'" << slice(source, property[property.size()-1]) << "',\n"
                         << indent(indt + 2) << "function()";

                el::JSSection* section = new el::JSSection;
                auto block = node->assignments()[i]->statementBlock();
                section->from = block->startByte();
                section->to = block->endByte();
                convert(block, source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << ".bind(" << object << "),\n"
                         << indent(indt + 2) << bindingsInJs << "\n"
                         << indent(indt + 1) << ")\n";
            }
        } else {
            if (node->assignments()[i]->expression()){
                *compose << indent(indt + 1) << "this";
                auto expr = node->assignments()[i]->expression();
                for (size_t prop = 0; prop < node->assignments()[i]->property().size(); ++prop){
                    *compose << "." << slice(source, node->assignments()[i]->property()[prop]);
                }
                *compose << " = ";

                // convert the subexpression
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                convert(expr, source, expressionSection->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;

                *compose << "\n";

            } else if (node->assignments()[i]->statementBlock() ) {
                std::string propName = "this";
                for (size_t prop = 0; prop < node->assignments()[i]->property().size(); ++prop){
                    propName += "." + slice(source, node->assignments()[i]->property()[prop]);
                }
                *compose << indent(indt + 1) << propName << " = " << "(function()";

                el::JSSection* section = new el::JSSection;
                auto block = node->assignments()[i]->statementBlock();
                section->from = block->startByte();
                section->to = block->endByte();
                convert(block, source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << ".bind(this)())\n";
            }
        }
    }

    for ( auto it = node->events().begin(); it != node->events().end(); ++it ){
        EventDeclarationNode* edn = *it;

        std::string paramList = "";
        if ( edn->parameterList() ){
            ParameterListNode* pdn = edn->parameterList()->as<ParameterListNode>();
            for ( auto it = pdn->parameters().begin(); it != pdn->parameters().end(); ++it ){
                if ( it != pdn->parameters().begin() )
                    paramList += ",";
                paramList += "[\'" + TypeNode::sliceWithoutAnnotation(source, (*it)->type()) + "\',\'" + slice(source, (*it)->identifier()) + "\']";
            }
        }
        *compose << indent(indt + 1) << (BaseNode::ConversionContext::baseComponentName(ctx) + ".addEvent(this, \'" + slice(source, edn->name()) + "\', [" + paramList + "])\n");
    }

    for ( auto it = node->listeners().begin(); it != node->listeners().end(); ++it ){
        ListenerDeclarationNode* ldn = *it;

        std::string paramList = "";
        if ( ldn->parameterList() ){
            ParameterListNode* pdn = ldn->parameterList()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }

        *compose << indent(indt + 1) << "this.on(\'" << slice(source, ldn->name()) << "\', function(" << paramList << ")";

        if ( ldn->body() ){
            JSSection* jssection = new JSSection;
            jssection->from = ldn->body()->startByte();
            jssection->to   = ldn->body()->endByte();
            convert(ldn->body(), source, jssection->m_children, indt + 1, ctx);
            *compose << jssection << ".bind(this));\n";
        } else {
            JSSection* jssection = new JSSection;
            jssection->from = ldn->bodyExpression()->startByte();
            jssection->to   = ldn->bodyExpression()->endByte();
            convert(ldn->bodyExpression(), source, jssection->m_children, indt + 1, ctx);
            *compose << "{" << jssection << "}.bind(this));\n";
        }
    }

    for ( auto it = node->methods().begin(); it != node->methods().end(); ++it ){
        TypedMethodDeclarationNode* tfdn = *it;

        std::string paramList = "";
        if ( tfdn->parameters() ){
            ParameterListNode* pdn = tfdn->parameters()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }
        JSSection* jssection = new JSSection;
        jssection->from = tfdn->body()->startByte();
        jssection->to   = tfdn->body()->endByte();
        convert(tfdn, source, jssection->m_children, indt + 2, ctx);
        *compose << indent(indt + 1) << "this." << slice(source, tfdn->name()) << " = function(" << paramList << ")" << jssection << "\n";
    }

    if (!node->nestedComponents().empty()){
        *compose << indent(indt + 1) << BaseNode::ConversionContext::baseComponentName(ctx) << ".assignChildrenAndComplete(this, [\n";
        for (unsigned i = 0; i < node->nestedComponents().size(); ++i){
            if (i != 0) *compose << ",\n";
            el::JSSection* section = new el::JSSection;
            section->from = node->nestedComponents()[i]->startByte();
            section->to = node->nestedComponents()[i]->endByte();
            convert(node->nestedComponents()[i], source, section->m_children, indt + 2, ctx);
            std::vector<std::string> flat;
            section->flatten(source, flat);
            for (auto s: flat)
                *compose << s << "\n";
            delete section;
        }
        *compose << indent(indt + 1) << "])\n";
    } else {
        *compose << indent(indt + 1) << BaseNode::ConversionContext::baseComponentName(ctx) << ".complete(this)\n";
    }

    *compose << indent(indt + 1) << "return this\n" << indent(indt) << "}.bind(";

    if (!node->id() || isRoot)
    {
        *compose << "new ";
        std::string name;
        for ( auto nameIden : node->name() ){
            if ( !name.empty() )
                name += ".";
            name += slice(source, nameIden);
        }
        *compose << name;

        if (!node->arguments())
            *compose << "()";
        else
            *compose << slice(source, node->arguments());
    } else {
        *compose << slice(source, node->id());
    }

    bool isThis = node->parent() && node->parent()->isNodeType<ComponentBodyNode>() ;
    isThis = isThis || (node->parent()
                        && node->parent()->parent()
                        && node->parent()->parent()->isNodeType<PropertyDeclarationNode>()
                        && node->parent()->parent()->parent()
                        && node->parent()->parent()->parent()->isNodeType<ComponentBodyNode>() );

    if (isThis){
        *compose << ")(this)";
    } else {
        *compose << ")(null)";
    }

    *compose << ")\n";

    sections.push_back(compose);
}


void LanguageNodesToJs::convertNewTaggedComponentExpression(NewTaggedComponentExpressionNode *node, const std::string &source, std::vector<ElementsInsertion *> &sections, int indentValue, BaseNode::ConversionContext *ctx){
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = node->startByte();
    compose->to = node->endByte();
    std::string name, value;
    for (auto child: node->children())
    {
        if (child->isNodeType<IdentifierNode>() ) {
            name = slice(source, child);
        } else {
            std::string sourceVal = slice(source, child);
            sourceVal = sourceVal.substr(1, sourceVal.length() - 2);

            enum State{
                Default,
                Space,
                Escape
            };
            value = "";

            if ( !sourceVal.empty() ){

                State state = Space;
                size_t i = 0;

                if ( sourceVal[0] == ' ' ){
                    value += ' ';
                }

                while (i < sourceVal.length()){
                    if ( sourceVal[i] == '\n' || sourceVal[i] == '\t' ){
                        if ( state == Default ){
                            value += ' ';
                            state = Space;
                        } else if ( state == Escape ){
                            value += "\\\\";
                            state = Space;
                        } // do nothing for space state
                    } else if ( sourceVal[i] == '\r' ){
                        if ( state == Escape ){
                            value += '\\';
                            state = Default;
                        }
                    } else if ( sourceVal[i] == ' ' ){
                        if ( state == Default ){
                            value += ' ';
                            state = Space;
                        } else if ( state == Escape ){
                            value += '\\';
                            state = Space;
                        } // do nothing for space state
                    } else if ( sourceVal[i] == '\\' ){
                        if ( state == Escape ){
                            value += "\\\\";
                            state = Default;
                        } else {
                            state = Escape;
                        }
                    } else if ( sourceVal[i] == 's' ){
                        if ( state == Escape ){
                            value += ' ';
                            state = Default;
                        } else {
                            value += 's';
                        }
                    } else {
                        if ( state == Escape ){
                            value += '\\';
                        }
                        value += sourceVal[i];
                        state = Default;
                    }
                    ++i;
                }
            }
        }
    }

    *compose << indent(indentValue + 0) << "(function(parent){\n";
    *compose << indent(indentValue + 1) << "this.setParent(parent)\n";
    *compose << indent(indentValue + 1) << BaseNode::ConversionContext::baseComponentName(ctx) << ".complete(this)\n";
    *compose << indent(indentValue + 1) << "return this\n";
    *compose << indent(indentValue + 0) << "}.bind(new " << name << "(\"" << value << "\"))(this))\n";

    sections.push_back(compose);
}

void LanguageNodesToJs::convertNewTrippleTaggedComponentExpression(NewTrippleTaggedComponentExpressionNode *node, const std::string &source, std::vector<ElementsInsertion *> &sections, int indentValue, BaseNode::ConversionContext *ctx){
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = node->startByte();
    compose->to = node->endByte();
    std::string name, value;

    for (auto child: node->children())
    {
        if (child->isNodeType<IdentifierNode>() ) {
            name = slice(source, child);
        } else {
            value = slice(source, child);
            value = value.substr(3, value.length() - 6);

            size_t startAt = value.find('\n');
            size_t endAt   = value.rfind('\n');

            if ( startAt != std::string::npos ){
                startAt += 1;
                size_t len = endAt - startAt;
                if ( len > 0 ){
                    value = value.substr(startAt, len);
                } else {
                    value = "";
                }
            }

            Utf8::replaceAll(value, "\r", "");
            Utf8::replaceAll(value, "\n", "\\n");
            Utf8::replaceAll(value, "\t", "\\t");
        }
    }

    *compose << indent(indentValue + 0) << "(function(parent){\n";
    *compose << indent(indentValue + 1) << "this.setParent(parent)\n";
    *compose << indent(indentValue + 1) << BaseNode::ConversionContext::baseComponentName(ctx) << ".complete(this)\n";
    *compose << indent(indentValue + 1) << "return this\n";
    *compose << indent(indentValue + 0) << "}.bind(new " << name << "(\"" << value << "\"))(this))\n";

    sections.push_back(compose);
}

void LanguageNodesToJs::convertPropertyDeclaration(PropertyDeclarationNode *node, const std::string &source, const std::string &componentReference, int indt, BaseNode::ConversionContext *ctx, const PropertyAccessorDeclarationNode::PropertyAccess &propertyAccess, ElementsInsertion *compose){
    *compose << indent(indt) << BaseNode::ConversionContext::baseComponentName(ctx) << ".addProperty(" + componentReference + ", '" << slice(source, node->name())
             << "', { type: '" << (node->type() ? slice(source, node->type()) : "") << "', notify: '"
             << slice(source, node->name()) << "Changed'";

    if ( propertyAccess.getter ){
        *compose << ", get: function()";
        JSSection* jssection = new JSSection;
        jssection->from = propertyAccess.getter->body()->startByte();
        jssection->to   = propertyAccess.getter->body()->endByte();
        convert(propertyAccess.getter->body(), source, jssection->m_children, indt + 1, ctx);
        std::vector<std::string> flat;
        jssection->flatten(source, flat);
        for (auto s: flat){
            *compose << s << "\n";
        }
        delete jssection;
    }
    if ( propertyAccess.setter ){
        *compose << ", set: function(" << (propertyAccess.setter->firstParameterName() ? slice(source, propertyAccess.setter->firstParameterName()) : "") << ")";
        JSSection* jssection = new JSSection;
        jssection->from = propertyAccess.setter->body()->startByte();
        jssection->to   = propertyAccess.setter->body()->endByte();
        convert(propertyAccess.setter->body(), source, jssection->m_children, indt + 1, ctx);
        std::vector<std::string> flat;
        jssection->flatten(source, flat);
        for (auto s: flat){
            *compose << s << "\n";
        }
        delete jssection;
    }


    *compose << "})\n";
}

}} // namespace lv, el
