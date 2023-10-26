#ifndef LVLANGUAGENODESTOJS_H
#define LVLANGUAGENODESTOJS_H

#include "languagenodes_p.h"

namespace lv{ namespace el{

class LanguageNodesToJs{

public:
    LanguageNodesToJs();

    static std::string slice(const std::string& source, uint32_t start, uint32_t end);
    static std::string slice(const std::string& source, BaseNode* node);
    static bool newLineFollows(const std::string& source, size_t startPosition);
    static bool newLinePrecedes(const std::string& source, size_t endPosition);

    void convert(
        BaseNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );

    void convertProgram(
        ProgramNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );

    void convertComponentDeclaration(
        ComponentDeclarationNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );

    void convertNewComponentExpression(
        NewComponentExpressionNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );

    void convertNewTaggedComponentExpression(
        NewTaggedComponentExpressionNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );


    void convertNewTrippleTaggedComponentExpression(
        NewTrippleTaggedComponentExpressionNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );

    void convertVariableDeclaration(
        VariableDeclarationNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );

    void convertFunctionDeclaration(
        FunctionDeclarationNode* node,
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        BaseNode::ConversionContext *ctx
    );

    void convertPropertyDeclaration(
        PropertyDeclarationNode* node,
        const std::string& source,
        const std::string& componentRef,
        int indt,
        BaseNode::ConversionContext* ctx,
        const PropertyAccessorDeclarationNode::PropertyAccess& propertyAccess,
        ElementsInsertion *compose
    );


private:
    static std::string indent(int i);

};

}} // namespace lv, el

#endif // LVLANGUAGENODESTOJS_H
