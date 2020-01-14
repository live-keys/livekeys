#include "cursorcontext.h"

/**
 * \class lv::el::CursorContext
 * \brief Provides completion context info
 *
 * Cursor context provides information about a given cursor position within an elements document.
 * CursorContext is obtained from a parsed document, which extracts all the available information
 * from the AST.
 *
 * The class provides additional information depending on the context:
 *
 *
 *
 * Within an import:
 *
 * \code
 * import path.to.import
 *                  ^ context = CompletionContext::InImport | CompletionContext::InElements
 *                    expressionPath = {RangeFor<path>, RangeFor<to>, RangeFor<im>}
 * \endcode
 *
 * \code
 * import path.to.import
 *                ^ context = CompletionContext::InImport | CompletionContext::InElements
 *                  expressionPath = {RangeFor<path>, RangeFor<to>, Range()}
 * \endcode
 *
 * \code
 * import path.to.import
 *        ^ context = CompletionContext::InImport | CompletionContext::InElements
 *          expressionPath = {}
 * \endcode
 *
 * \code
 * import path.to.import
 *     ^ context = 0
 * \endcode
 *
 *
 *
 *
 * Within a string
 *
 * \code
 * var a = "example"
 *           ^ context = CompletionContext::InString
 * \endcode
 *
 *
 *
 *
 * Left of a declaration
 *
 * \code
 * component T < Element{ prop : 200 }
 *                          ^ context = CompletionContext::InLeftOfDeclaration | CompletionContext::InElements
 *                            expressionPath = {RangeFor<pr>}
 *                            objectType = RangeFor<Element>
 * \endcode
 *
 * \code
 * component T < Element{ int prop : 200 }
 *                              ^ context = CompletionContext::InLeftOfDeclaration | CompletionContext::InElements
 *                                expressionPath = {RangeFor<pr>}
 *                                propertyDeclaredType = RangeFor<int>
 *                                objectType = RangeFor<Element>
 * \endcode
 *
 * \code
 * component T < Element{ on complete : function(){ ... } }
 *                              ^ context = CompletionContext::InLeftOfDeclaration | CompletionContext::InElements
 *                                expressionPath = {RangeFor<com>}
 *                                propertyDeclaredType = RangeFor<on>
 *                                objectType = RangeFor<Element>
 * \endcode
 *
 * Note that for new component expressions, the objectType is 'T', not 'Element'
 *
 * \code
 * T{ prop : 200 }
 *      ^ context = CompletionContext::InLeftOfDeclaration | CompletionContext::InElements
 *        expressionPath = {RangeFor<pr>}
 *        objectType = RangeFor<T>
 * \endcode
 *
 * Imported types have a separate property that needs to be assigned, called objectImportNamespace:
 *
 * \code
 * import view as View
 * View.T{ prop : 200}
 *           ^ context = CompletionContext::InLeftOfDeclaration | CompletionContext::InElements
 *             expressionPath = {RangeFor<pr>}
 *             objectType = RangeFor<T>
 *             objectImportNamespace = RangeFor<View>
 * \endcode
 *
 *
 *
 *
 * Right of a declaration
 *
 * \code
 * T{ prop : this.font.size }
 *                      ^ context = CompletionContext::InRightOfDeclaration | CompletionContext::InElements
 *                        expressionPath = {RangeFor<this>, RangeFor<font>, RangeFor<s>}
 *                        objectType = RangeFor<T>
 *                        propertyPath = {RangeFor<prop>}
 * \endcode
 *
 *
 * \code
 * T{ int declaredProp : this.font.size }
 *                      ^ context = CompletionContext::InRightOfDeclaration | CompletionContext::InElements
 *                        expressionPath = {}
 *                        objectType = RangeFor<T>
 *                        propertyPath = {RangeFor<declaredProp>}
 *                        propertyDeclaredType = RangeFor<int>
 * \endcode
 *
 * \code // TODO
 * T{ fn declaredFunction(){ return 0 }
 *                              ^ context = CompletionContext::InRightOfDeclaration | CompletionContext::InElements
 *                                expressionPath = {RangeFor<ret>}
 *                                objectType = RangeFor<T>
 *                                propertyPath = {RangeFor<declaredFunction>}
 *                                propertyDeclaredType = RangeFor<fn>
 * \endcode
 *
 */

namespace lv{ namespace el{

CursorContext::CursorContext(int context,
        const std::vector<SourceRange> &expressionPath,
        const std::vector<SourceRange> &propertyPath,
        const SourceRange &propertyDeclaredType,
        const SourceRange &objectType,
        const SourceRange &objectImportNamespace)
    : m_context(context)
    , m_expressionPath(expressionPath)
    , m_propertyPath(propertyPath)
    , m_propertyDeclaredType(propertyDeclaredType)
    , m_objectType(objectType)
    , m_objectImportNamespace(objectImportNamespace)
{
}

CursorContext::~CursorContext(){
}

int CursorContext::context() const
{
    return m_context;
}

Utf8 CursorContext::contextString() const{
    std::string base;
    if ( m_context & CursorContext::InImport)
        base += "InImport";
    if ( m_context & CursorContext::InElements)
        base += base.empty() ? "InElements" : " | InElements";
    if ( m_context & CursorContext::InLeftOfDeclaration)
        base += base.empty() ? "InLeftOfDeclaration" : " | InLeftOfDeclaration";
    if ( m_context & CursorContext::InRightOfDeclaration)
        base += base.empty() ? "InRightOfDeclaration" : " | InRightOfDeclaration";
    if ( m_context & CursorContext::InStringLiteral)
        base += base.empty() ? "InString" : " | InString";
    return Utf8(base);
}

}} // namespace lv, el
