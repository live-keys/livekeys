#include "qqmlidvisitor.h"
#include "qmljs/parser/qmljsast_p.h"

namespace lcv{

QString DescribeValueVisitor::describe(const QmlJS::Value *value, int depth)
{
    DescribeValueVisitor describer(-depth, 0, 2);
    return describer(value);
}

DescribeValueVisitor::DescribeValueVisitor(int startDepth, int startIndent, int indentIncrement)
    : m_depth(startDepth)
    , m_indent(startIndent)
    , m_indentIncrement(indentIncrement)
{ }

DescribeValueVisitor::~DescribeValueVisitor()
{
}

QString DescribeValueVisitor::operator()(const QmlJS::Value *value)
{
    if (value)
        value->accept(this);
    else
        dump("**NULL**");
    return description();
}

void DescribeValueVisitor::basicDump(const char *baseName, const QmlJS::Value *value, bool opensContext)
{
    dump(QLatin1String(baseName));
    dump(QString::fromLatin1("@%1").arg((quintptr)(void *)value, 0, 16));
    QString fileName;
    int line, column;
    if (value && value->getSourceLocation(&fileName, &line, &column))
        dump(QString::fromLatin1(" - %1:%2:%3").arg(fileName)
                 .arg(line).arg(column));
    if (opensContext)
        openContext();
}

void DescribeValueVisitor::dump(const QString &toAdd)
{
    m_description += toAdd;
    m_emptyContext = false;
}

void DescribeValueVisitor::dump(const char *toAdd)
{
    m_description += QLatin1String(toAdd);
    m_emptyContext = false;
}

void DescribeValueVisitor::dumpNewline()
{
    dump("\n");
    int indent = m_indent;
    QString indentStr = QLatin1String("          ");
    while (indent > 10) {
       dump(indentStr);
       indent -= 10;
    }
    if (indent > 0)
        dump(indentStr.left(indent));
}

void DescribeValueVisitor::openContext(const char *openStr)
{
    dump(openStr);
    m_indent += m_indentIncrement;
    m_emptyContext = true;
}

void DescribeValueVisitor::closeContext(const char *closeStr)
{
    m_indent -= m_indentIncrement;
    if (m_emptyContext) {
        dump(" ");
    } else if (strlen(closeStr)) {
        dumpNewline();
    }
    dump(closeStr);
}

void DescribeValueVisitor::visit(const QmlJS::NullValue *value)
{
    basicDump("NullValue", value, false);
}

void DescribeValueVisitor::visit(const QmlJS::UndefinedValue *value)
{
    basicDump("UndefinedValue", value, false);
}

void DescribeValueVisitor::visit(const QmlJS::UnknownValue *value)
{
    basicDump("UnknownValue", value, false);
}

void DescribeValueVisitor::visit(const QmlJS::NumberValue *value)
{
    if (const QmlJS::QmlEnumValue *v = value->asQmlEnumValue()) {
        basicDump("QmlEnumValue", v, true);
        dumpNewline();
        dump(QString::fromLatin1("%2, ").arg((quintptr)(void *)v)
                .arg(v->name()));
        openContext("[");
        foreach (const QString &key, v->keys()) {
            dumpNewline();
            dump(key);
        }
        closeContext("]");
        dumpNewline();
        m_indent -= m_indentIncrement;
        closeContext();
    } else if (const QmlJS::IntValue *v = value->asIntValue()) {
        basicDump("IntValue", v, false);
    } else if (const QmlJS::RealValue *v = value->asRealValue()) {
        basicDump("RealValue", v, false);
    } else {
        basicDump("NumberValue", value, false);
    }
}

void DescribeValueVisitor::visit(const QmlJS::BooleanValue *value)
{
    basicDump("BooleanValue", value, false);
}

void DescribeValueVisitor::visit(const QmlJS::StringValue *value)
{
    if (const QmlJS::UrlValue *v = value->asUrlValue())
        basicDump("UrlValue", v, false);
    else
        basicDump("StringValue", value, false);
}

class PrintMembers : public QmlJS::MemberProcessor {
public:
    DescribeValueVisitor &d;
    PrintMembers(DescribeValueVisitor &describer)
        : d(describer) { }

    bool dump(const QString &name, const QmlJS::Value *value)
    {
        d.dumpNewline();
        d.dump(name);
        d.openContext(":");
        value->accept(&d);
        d.closeContext("");
        return true;
    }

    bool processProperty(const QString &name, const QmlJS::Value *value, const QmlJS::PropertyInfo &pInfo) override
    {
        d.dumpNewline();
        d.dump(name);
        d.openContext(":");
        d.dump("<");
        d.dump(pInfo.toString());
        d.dump(">");
        d.dumpNewline(); // avoid?
        value->accept(&d);
        d.closeContext("");
        return true;
    }
    bool processEnumerator(const QString &name, const QmlJS::Value *value) override
    {
        return dump(name, value);
    }
    bool processSignal(const QString &name, const QmlJS::Value *value) override
    {
        return dump(name, value);
    }
    bool processSlot(const QString &name, const QmlJS::Value *value) override
    {
        return dump(name, value);
    }
    bool processGeneratedSlot(const QString &name, const QmlJS::Value *value) override
    {
        return dump(name, value);
    }
};

void DescribeValueVisitor::visit(const QmlJS::ObjectValue *value)
{
    bool printDetail = (++m_depth <= 0 && ! m_visited.contains(value));
    m_visited.insert(value);
    if (const QmlJS::ASTObjectValue *v = value->asAstObjectValue()) {
        basicDump("ASTObjectValue", value, printDetail);
        if (printDetail) {
            if (v->typeName()) {
                dumpNewline();
                dump("typeName:");
                dump(v->typeName()->name.toString());
            }
            dumpNewline();
            dump("defaultPropertyName:");
            dump(v->defaultPropertyName());
        }
    } else if (const QmlJS::FunctionValue *f = value->asFunctionValue()) {
        if (const QmlJS::ASTFunctionValue *v = f->asAstFunctionValue()) {
            basicDump("ASTFunctionValue", v, printDetail);
            if (printDetail && v->ast()) {
                dumpNewline();
                dump("name:");
                dump(v->ast()->name.toString());
            }
        } else if (const QmlJS::ASTSignal *v = f->asAstSignal()) {
            basicDump("ASTSignal", v, printDetail);
            if (printDetail) {
                if (v->ast()) {
                    dumpNewline();
                    dump("name:");
                    dump(v->ast()->name.toString());
                }
                dumpNewline();
                dump("slotName:");
                dump(v->slotName());
            }
        } else if (f->asFunction()) {
            basicDump("Function", f, printDetail);
        } else if (f->asMetaFunction()) {
            basicDump("MetaFunction", f, printDetail);
        } else {
            basicDump("FunctionValue", f, printDetail);
        }
        if (printDetail) {
            dumpNewline();
            dump("returnValue:");
            (*this)(f->returnValue());
            dump("arguments:");
            openContext("[");
            for (int i = 1; i < f->namedArgumentCount() - f->optionalNamedArgumentCount(); ++i) {
                dumpNewline();
                (*this)(f->argument(i));
                dump(" ");
                dump(f->argumentName(i));
            }
            if (f->optionalNamedArgumentCount()) {
                dumpNewline();
                dump("// optional arguments");
                dumpNewline();
            }
            for (int i = f->namedArgumentCount() - f->optionalNamedArgumentCount();
                 i < f->namedArgumentCount(); ++i) {
                dumpNewline();
                (*this)(f->argument(i));
                dump(" ");
                dump(f->argumentName(i));
            }
            closeContext("]");
            dumpNewline();
            if (f->isVariadic())
                dump("isVariadic: true");
        }
    } else if (const QmlJS::CppComponentValue *v = value->asCppComponentValue()) {
        basicDump("CppComponentValue", value, printDetail);
        if (printDetail) {
            LanguageUtils::FakeMetaObject::ConstPtr metaObject = v->metaObject();
            dumpNewline();
            dump("metaObject:");
            if (metaObject.isNull())
                dump("**NULL**");
            else
                metaObject->describe(true, m_indent + m_indentIncrement);
            dumpNewline();
            dump("moduleName:");
            dump(v->moduleName());
            dumpNewline();
            dump("componentVersion:");
            dump(v->componentVersion().toString());
            dumpNewline();
            dump("importVersion:");
            dump(v->importVersion().toString());
            dumpNewline();
            dump("defaultPropertyName:");
            dump(v->defaultPropertyName());

            /*QString propertyType(const QString &propertyName) const;
            bool isListProperty(const QString &name) const;
            bool isWritable(const QString &propertyName) const;
            bool isPointer(const QString &propertyName) const;
            bool hasLocalProperty(const QString &typeName) const;
            bool hasProperty(const QString &typeName) const;*/

        }
    } else if (value->asJSImportScope()) {
        basicDump("JSImportScope", value, printDetail);
    } else if (value->asTypeScope()) {
        basicDump("TypeScope", value, printDetail);
    } else {
        basicDump("ObjectValue", value, printDetail);
    }
    if (printDetail) {
        if (value) {
            dumpNewline();
            dump("className:");
            dump(value->className());
            dumpNewline();
            dump("members:");
            openContext("[");
            PrintMembers printMembers(*this);
            value->processMembers(&printMembers);
            closeContext("]");
            dumpNewline();
            dump("prototype:");
            (*this)(value->prototype());
        }
        closeContext();
    }
    --m_depth;
}

void DescribeValueVisitor::visit(const QmlJS::FunctionValue *f)
{
    const QmlJS::ObjectValue *o = f;
    visit(o);
}

void DescribeValueVisitor::visit(const QmlJS::Reference *value)
{
    bool printDetail = (++m_depth <= 0 && ! m_visited.contains(value));
    m_visited.insert(value);
    if (const QmlJS::ASTPropertyReference *v = value->asAstPropertyReference()) {
        basicDump("ASTPropertyReference", v, printDetail);
        if (printDetail) {
            if (QmlJS::AST::UiPublicMember *ast = v->ast()) {
                dumpNewline();
                dump("property:");
                dump(ast->name.toString());
            }
            dumpNewline();
            dump("onChangedSlotName:");
            dump(v->onChangedSlotName());
        }
    } else if (const QmlJS::ASTVariableReference *v = value->asAstVariableReference()) {
        basicDump("ASTVariableReference", v, printDetail);
        const QmlJS::AST::VariableDeclaration *var = v->ast();
        if (printDetail && var) {
            dumpNewline();
            dump("variable:");
            dump(var->name.toString());
        }
    } else if (const QmlJS::QmlPrototypeReference *v = value->asQmlPrototypeReference()) {
        basicDump("QmlPrototypeReference", v, printDetail);
        const QmlJS::AST::UiQualifiedId *qmlTypeName = v->qmlTypeName();
        if (printDetail && qmlTypeName) {
            dumpNewline();
            dump("qmlTypeName:");
            dump(qmlTypeName->name.toString());
        }
    } else if (value->asQtObjectPrototypeReference()) {
        basicDump("QtObjectPrototypeReference", value, printDetail);
    } else {
        basicDump("Reference", value, printDetail);
    }
    if (printDetail) {
        QmlJS::ValueOwner *vOwner = value->valueOwner();
        dumpNewline();
        dump(QString::fromLatin1("valueOwner@%1").arg((quintptr)(void *)vOwner, 0, 16));
        closeContext();
    }
    --m_depth;
}

void DescribeValueVisitor::visit(const QmlJS::ColorValue *value)
{
    basicDump("ColorValue", value, false);
}

void DescribeValueVisitor::visit(const QmlJS::AnchorLineValue *value)
{
    basicDump("AnchorLineValue", value, false);
}

QString DescribeValueVisitor::description() const
{
    return m_description;
}

}// namespace
