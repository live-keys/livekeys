#include "parseddocument.h"
#include "live/visuallog.h"

#include "tree_sitter/parser.h"
#include "tree_sitter/api.h"

namespace lv{ namespace el{

std::string ParsedDocument::slice(const std::string &source, TSNode node)
{
    auto start = ts_node_start_byte(node);
    auto end = ts_node_end_byte(node);
    return source.substr(start, end-start);
}

std::vector<ImportInfo> ParsedDocument::extractImports(const std::string &source, LanguageParser::AST *ast){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root_node = ts_tree_root_node(tree);

    std::vector<ImportInfo> result;

    auto count = ts_node_child_count(root_node);
    for (int i = 0; i < count; ++i)
    {
        TSNode child = ts_node_child(root_node, i);
        if (strcmp(ts_node_type(child), "import_statement") == 0)
        {
            bool rel = false;
            std::vector<Utf8> segs;
            Utf8 alias;

            auto import_count = ts_node_child_count(child);
            int j = 0;
            while (j < import_count)
            {
                TSNode import_child = ts_node_child(child, j);
                if (strcmp(ts_node_type(import_child), ".") == 0)
                {
                    rel = true;
                } else if (strcmp(ts_node_type(import_child), "import_path") == 0) {
                    auto import_child_count = ts_node_child_count(import_child);
                    for (int k = 0; k < import_child_count; k+=2)
                    {
                        segs.push_back(slice(source, ts_node_child(import_child, k)));
                    }
                } else if (strcmp(ts_node_type(import_child), "import_as") == 0) {
                    alias = slice(source, ts_node_child(import_child, 1));
                }
                ++j;
            }

            result.push_back(ImportInfo(segs, alias, rel));
        }
    }

    return result;
}

CursorContext ParsedDocument::findCursorContext(LanguageParser::AST *ast, int position){

    std::vector<TSNode> path;
    treePath(ast, position, path);

    int context = 0;
    std::vector<SourceRange> expressionPath;
    std::vector<SourceRange> propertyPath;
    SourceRange propertyDeclaredType;
    SourceRange objectType;
    SourceRange objectImportNamespace;

    for (int idx = path.size()-1; idx >= 0; --idx)
    {
        TSNode& curr = path[idx];
        auto type = ts_node_type(curr);

        if (strcmp(type, "property_declaration") == 0 || strcmp(type, "property_assignment") == 0 || strcmp(type, "listener_declaration") == 0)
        {

            bool isAssign = strcmp(type, "property_assignment") == 0;
            int owner = idx-1;
            while (owner >= 0)
            {
                auto ownerType = ts_node_type(path[owner]);
                if (strcmp(ownerType, "component_declaration") == 0
                 || strcmp(ownerType, "new_component_expression") == 0)
                    break;
                --owner;
            }
            if (owner == -1) return CursorContext(context, expressionPath);

            TSNode& parent = path[owner];
            if (strcmp(ts_node_type(parent), "component_declaration") == 0)
            {
                if (ts_node_child_count(parent) >= 3 && strcmp(ts_node_type(ts_node_child(parent, 2)), "component_heritage") == 0)
                {
                    TSNode heritage = ts_node_child(parent, 2);
                    auto heritage_count = ts_node_child_count(heritage);
                    TSNode objectTypeNode = ts_node_child(heritage, heritage_count-1);
                    objectType = SourceRange(ts_node_start_byte(objectTypeNode), ts_node_end_byte(objectTypeNode) - ts_node_start_byte(objectTypeNode));

                    if (heritage_count == 4)
                    {
                        TSNode importNode = ts_node_child(heritage, 1);
                        objectImportNamespace = SourceRange(ts_node_start_byte(importNode), ts_node_end_byte(importNode) - ts_node_start_byte(importNode));

                    }
                }
            }

            if (strcmp(ts_node_type(parent), "new_component_expression") == 0 )
            {
                auto ncec = ts_node_child_count(parent);
                auto body_pos = ts_node_start_byte(ts_node_child(parent, ncec-1));
                if (position <= body_pos)
                {
                    if (ncec == 4)
                    {
                        auto name_pos = ts_node_start_byte(ts_node_child(parent, 2));
                        auto import_start =  ts_node_start_byte(ts_node_child(parent, 0));
                        if (position >= name_pos)
                        {
                            auto import_length = ts_node_end_byte(ts_node_child(parent, 0)) - import_start;
                            objectImportNamespace = SourceRange(import_start, import_length);
                            objectType = SourceRange(name_pos, position - name_pos);
                        } else {
                            objectImportNamespace = SourceRange(import_start, position - import_start);
                        }

                    } else { // ncec = 2
                        auto type_start = ts_node_start_byte(ts_node_child(parent, 0));
                        objectType = SourceRange(type_start, position - type_start);
                    }
                } else {
                    if (ncec == 4)
                    {

                        auto import_start =  ts_node_start_byte(ts_node_child(parent, 0));
                        auto import_length = ts_node_end_byte(ts_node_child(parent, 0)) - import_start;
                        objectImportNamespace = SourceRange(import_start, import_length);
                        auto type_start = ts_node_start_byte(ts_node_child(parent, 2));
                        auto type_length = ts_node_end_byte(ts_node_child(parent, 2)) - type_start;
                        objectType = SourceRange(type_start, type_length);


                    } else { // ncec = 2
                        auto type_start = ts_node_start_byte(ts_node_child(parent, 0));
                        auto type_length = ts_node_end_byte(ts_node_child(parent, 0)) - type_start;
                        objectType = SourceRange(type_start, type_length);
                    }
                }
            }

            int delimiter_pos = ts_node_start_byte(ts_node_child(curr, isAssign? 1: 2)); // position of :
            if (position < delimiter_pos) context = CursorContext::InLeftOfDeclaration;
            else context |= CursorContext::InRightOfDeclaration;
            context |= CursorContext::InElements;

            if (strcmp(type, "property_declaration") == 0 && position < delimiter_pos && strcmp(ts_node_type(path[idx+1]), "identifier") == 0)
            {
                propertyDeclaredType = SourceRange(ts_node_start_byte(path[idx+1]), position - ts_node_start_byte(path[idx+1]));
            }

            if (strcmp(type, "property_declaration") == 0 && position < delimiter_pos && strcmp(ts_node_type(path[idx+1]), "property_identifier") == 0)
            {
                TSNode propType = ts_node_child(curr, 0);
                propertyDeclaredType = SourceRange(ts_node_start_byte(propType), ts_node_end_byte(propType) - ts_node_start_byte(propType));
                expressionPath.push_back(SourceRange(ts_node_start_byte(path[idx+1]), position - ts_node_start_byte(path[idx+1])));
            }

            if (strcmp(type, "listener_declaration") == 0 && position < delimiter_pos && strcmp(ts_node_type(path[idx+1]), "on") == 0)
            {
                propertyDeclaredType = SourceRange(ts_node_start_byte(path[idx+1]), position - ts_node_start_byte(path[idx+1]));
            }

            if (strcmp(type, "listener_declaration") == 0 && position < delimiter_pos && strcmp(ts_node_type(path[idx+1]), "property_identifier") == 0)
            {
                TSNode propType = ts_node_child(curr, 0);
                propertyDeclaredType = SourceRange(ts_node_start_byte(propType), 2);
                expressionPath.push_back(SourceRange(ts_node_start_byte(path[idx+1]), position - ts_node_start_byte(path[idx+1])));
            }

            if (isAssign && position < delimiter_pos)
            {
                expressionPath.push_back(SourceRange(ts_node_start_byte(path[idx+1]), position - ts_node_start_byte(path[idx+1])));
            }

            if (position > delimiter_pos && strcmp(ts_node_type(ts_node_child(curr, isAssign? 2: 3)), "expression_statement") == 0)
            {
                if (!isAssign)
                {
                    TSNode propType = ts_node_child(curr, 0);
                    propertyDeclaredType = SourceRange(ts_node_start_byte(propType), ts_node_end_byte(propType) - ts_node_start_byte(propType));
                }

                TSNode propPath = ts_node_child(curr, isAssign? 0: 1);
                propertyPath.push_back(SourceRange(ts_node_start_byte(propPath), ts_node_end_byte(propPath) - ts_node_start_byte(propPath)));


                TSNode exp = ts_node_child(curr, isAssign? 2:3);
                if (ts_node_child_count(exp) == 1 && strcmp(ts_node_type(ts_node_child(exp, 0)), "member_expression") == 0)
                {
                    TSNode memberExp = ts_node_child(exp, 0);
                    // writeNode(memberExp);
                    auto memcount = ts_node_child_count(memberExp);
                    while (memcount == 3)
                    {
                        // memexp . child
                        TSNode member = ts_node_child(memberExp, 2);
                        auto mstart = ts_node_start_byte(member);
                        auto mend = ts_node_end_byte(member);

                        if (mstart < position)
                            expressionPath.push_back(SourceRange(mstart, (position < mend ? position : mend)-mstart));

                        memberExp = ts_node_child(memberExp, 0);
                        memcount = ts_node_child_count(memberExp);
                    }

                    auto mstart = ts_node_start_byte(memberExp);
                    auto mend = ts_node_end_byte(memberExp);

                    if (mstart < position)
                        expressionPath.push_back(SourceRange(mstart, (position < mend ? position : mend)-mstart));

                    std::reverse(expressionPath.begin(), expressionPath.end());
                }
            }

            return CursorContext(context, expressionPath, propertyPath, propertyDeclaredType, objectType, objectImportNamespace);
        }

        if (strcmp(type, "typed_function_declaration") == 0)
        {
            // TODO: Ask Dinu
        }

        if (strcmp(type, "import_path") == 0)
        {
            auto count = ts_node_child_count(curr);
            context = CursorContext::InImport | CursorContext::InElements;
            for (int ip = 0; ip < count; ip+=2)
            {
                auto start = ts_node_start_byte(ts_node_child(curr, ip));
                auto end = ts_node_end_byte(ts_node_child(curr, ip));
                if (start < position)
                {
                    if (end < position)
                    {
                        expressionPath.push_back(SourceRange(start, end - start));
                    }
                    else {
                        expressionPath.push_back(SourceRange(start, position-start));
                        break;
                    }
                }
            }

            return CursorContext(context,expressionPath);
        }

        if (strcmp(type, "string") == 0)
        {
            context = CursorContext::InStringLiteral;
        }

    }


    return CursorContext(context, expressionPath);
}

void ParsedDocument::treePath(LanguageParser::AST *ast, int position, std::vector<TSNode> &result)
{
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root_node = ts_tree_root_node(tree);

    TSNode current = root_node;
    bool found = true;
    while (found)
    {
        auto count = ts_node_child_count(current);
        found = false;
        for (int idx = 0; idx < count; ++idx)
        {
            TSNode child = ts_node_child(current, idx);
            auto start = ts_node_start_byte(child);
            auto end = ts_node_end_byte(child);

            if (start <= position && position <= end)
            {
                result.push_back(child);
                current = child;
                found = true;
                break;
            }
        }
    }
}

TypeInfo::Ptr ParsedDocument::extractType(const std::string& source, TSNode node)
{
    Utf8                      typeName;
    // Utf8                      className; = ?
    Utf8                      inherits = "Element";
    // std::vector<EnumInfo>     enums; = none
    std::vector<PropertyInfo> properties;
    std::vector<FunctionInfo> functions;
    std::vector<FunctionInfo> events;
    // bool                      isCreatable; = false
    bool                      isInstance = false;

    if (strcmp(ts_node_type(node), "new_component_expression") == 0)
    {
        if (strcmp(ts_node_type(ts_node_child(node, 0)), "component_instance") != 0)
            return nullptr;
        isInstance = true;
    }



    auto count = ts_node_child_count(node);
    for (int i = 0; i < count; i++)
    {
        TSNode cdChild = ts_node_child(node, i);

        if (strcmp(ts_node_type(cdChild), "identifier") == 0)
        {
            typeName = slice(source, cdChild);
        }
        else if (strcmp(ts_node_type(cdChild), "component_heritage") == 0)
        {

            auto start = ts_node_start_byte(ts_node_child(cdChild, 1));
            auto length = ts_node_end_byte(ts_node_child(cdChild, ts_node_child_count(cdChild)-1)) - start;
            inherits = source.substr(start, length);

        }
        else if (strcmp(ts_node_type(cdChild), "component_body") == 0)
        {
            auto component_body_count = ts_node_child_count(cdChild);
            for (int cb = 1; cb < component_body_count-1; ++cb)
            {
                TSNode component_body_child = ts_node_child(cdChild, cb);

                if (strcmp(ts_node_type(component_body_child), "property_declaration") == 0)
                {
                    Utf8 type = slice(source, ts_node_child(component_body_child, 0));
                    Utf8 name = slice(source, ts_node_child(component_body_child, 1));
                    properties.push_back(PropertyInfo(name, type));
                }
                else if (strcmp(ts_node_type(component_body_child), "typed_function_declaration") == 0 || strcmp(ts_node_type(component_body_child), "event_declaration") == 0)
                {
                    Utf8 funcName = slice(source, ts_node_child(component_body_child, 1));
                    FunctionInfo fi(funcName);

                    TSNode formal_type_parameters = ts_node_child(component_body_child, 2);
                    auto ftpc = ts_node_child_count(formal_type_parameters);
                    for (int ftpcidx = 1; ftpcidx < ftpc; ftpcidx+=2)
                    {
                        TSNode formal_type_parameter = ts_node_child(formal_type_parameters, ftpcidx);
                        fi.addParameter(slice(source, ts_node_child(formal_type_parameter, 1)), slice(source, ts_node_child(formal_type_parameter, 0)));
                    }
                    if (strcmp(ts_node_type(component_body_child), "typed_function_declaration") == 0)
                        functions.push_back(fi);
                    else
                        events.push_back(fi);
                }

            }
        }
    }

    auto result = TypeInfo::create(typeName, inherits, false, isInstance);
    for (auto property: properties)
    {
        result->addProperty(property);
    }
    for (auto function: functions)
    {
        result->addFunction(function);
    }
    for (auto event: events)
    {
        result->addEvent(event);
    }
    return result;
}

DocumentInfo::Ptr ParsedDocument::extractInfo(const std::string &source, LanguageParser::AST *ast){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root_node = ts_tree_root_node(tree);

    DocumentInfo::Ptr result = DocumentInfo::create();
    auto count = ts_node_child_count(root_node);
    for (int i = 0; i < count; ++i)
    {
        TSNode child = ts_node_child(root_node, i);
        if (strcmp(ts_node_type(child), "import_statement") == 0)
        {
            bool rel = false;
            std::vector<Utf8> segs;
            Utf8 alias;

            auto import_count = ts_node_child_count(child);
            int j = 0;
            while (j < import_count)
            {
                TSNode import_child = ts_node_child(child, j);
                if (strcmp(ts_node_type(import_child), ".") == 0)
                {
                    rel = true;
                } else if (strcmp(ts_node_type(import_child), "import_path") == 0) {
                    auto import_child_count = ts_node_child_count(import_child);
                    for (int k = 0; k < import_child_count; k+=2)
                    {
                        segs.push_back(slice(source, ts_node_child(import_child, k)));
                    }
                } else if (strcmp(ts_node_type(import_child), "import_as") == 0) {
                    alias = slice(source, ts_node_child(import_child, 1));
                }
                ++j;
            }

            result->addImport(ImportInfo(segs, alias, rel));
        }
        else if (strcmp(ts_node_type(child), "component_declaration") == 0)
        {
            TypeInfo::Ptr res = extractType(source, child);
            result->addType(res);
        }
        else if (strcmp(ts_node_type(child), "expression_statement") == 0)
        {

            if (ts_node_child_count(child) == 1 && strcmp(ts_node_type(ts_node_child(child, 0)), "new_component_expression") == 0)
            {
                TypeInfo::Ptr res = extractType(source, ts_node_child(child, 0));
                if (res != nullptr) result->addType(res);
            }
        }

    }

    return result;
}

}} // namespace lv, el
