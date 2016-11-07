#ifndef QQMLIDVISITOR_H
#define QQMLIDVISITOR_H

#include "qmljs/qmljsinterpreter.h"
#include "qmljs/parser/qmljsast_p.h"
#include <QMap>

namespace lcv{

class DescribeValueVisitor : public QmlJS::ValueVisitor{

public:
    static QString describe(const QmlJS::Value *value, int depth = 1);

    DescribeValueVisitor(int detailDepth = 1, int startIndent = 0, int indentIncrement = 2);
    virtual ~DescribeValueVisitor();

    QString operator()(const QmlJS::Value *value);
    void visit(const QmlJS::NullValue *) override;
    void visit(const QmlJS::UndefinedValue *) override;
    void visit(const QmlJS::UnknownValue *) override;
    void visit(const QmlJS::NumberValue *) override;
    void visit(const QmlJS::BooleanValue *) override;
    void visit(const QmlJS::StringValue *) override;
    void visit(const QmlJS::ObjectValue *) override;
    void visit(const QmlJS::FunctionValue *) override;
    void visit(const QmlJS::Reference *) override;
    void visit(const QmlJS::ColorValue *) override;
    void visit(const QmlJS::AnchorLineValue *) override;
    QString description() const;
    void basicDump(const char *baseName, const QmlJS::Value *value, bool opensContext);
    void dumpNewline();
    void openContext(const char *openStr = "{");
    void closeContext(const char *closeStr = "}");
    void dump(const char *toAdd);
    void dump(const QString &toAdd);

private:
    int m_depth;
    int m_indent;
    int m_indentIncrement;
    bool m_emptyContext;
    QSet<const QmlJS::Value*> m_visited;
    QString m_description;
};

class QIdExtractor : public QmlJS::MemberProcessor{

public:
    bool processProperty(const QString &name, const QmlJS::Value *, const QmlJS::PropertyInfo &) override{
        m_ids << name;
        return true;
    }

    QStringList ids() const{ return m_ids; }

private:
    QStringList m_ids;
};

class QIdValueExtractor : public QmlJS::MemberProcessor{

public:
    QIdValueExtractor(const QString& name)
        : m_name(name)
        , m_value(0)
    {
    }

    bool processProperty(const QString &name, const QmlJS::Value *value, const QmlJS::PropertyInfo &) override{
        if ( name == m_name ){
            m_value = value;
            return false;
        }
        return true;
    }

    const QmlJS::Value* value(){ return m_value; }

private:
    QString m_name;
    const QmlJS::Value* m_value;
};

class QValueMemberExtractor : public QmlJS::MemberProcessor{
public:
    QValueMemberExtractor() : parent(0)
    {}

    bool processProperty(const QString &name, const QmlJS::Value *value, const QmlJS::PropertyInfo&) override{
        QString type = "object";
        if ( const QmlJS::ASTPropertyReference* vr = value->asAstPropertyReference() ){
            type = (vr->ast() != 0) ? vr->ast()->memberType.toString() : QString("object");
        } else if ( const QmlJS::ASTFunctionValue* fv = value->asAstFunctionValue() ){
            type = "function";//TODO
        }
        members[name] = type;
        return false;
    }
    bool processEnumerator(const QString &, const QmlJS::Value *) override
    {
        return false;
    }
    bool processSignal(const QString &name, const QmlJS::Value *value) override
    {
        //TODO
//        membersignals[name]
//        qDebug() << "Signal" << name << value;
        return false;
    }
    bool processSlot(const QString &name, const QmlJS::Value *value) override
    {
        //TODO
//        qDebug() << "Slot" << name << value;
        return false;
    }
    bool processGeneratedSlot(const QString &name, const QmlJS::Value *value) override
    {
        //TODO
//        qDebug() << "Slot" << name << value;
//        qDebug() << name << value;
        return false;
    }

private:
    const QmlJS::Value* parent;
    QMap<QString, QString> members;
    QMap<QString, QString> memberslots;
    QMap<QString, QString> membersignals;
};

}// namespace

#endif // QQMLIDVISITOR_H
