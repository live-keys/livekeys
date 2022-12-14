#ifndef LVQMLFUNCTIONOBJECTCALL_H
#define LVQMLFUNCTIONOBJECTCALL_H

#include <QQmlContext>
#include <QJSValue>
#include <QJSValueIterator>

#include "live/qmlpromise.h"
#include "live/qmlstreamprovider.h"
#include "live/qmlstream.h"

namespace lv{

class QmlFunctionObjectCall{

public:
    class Source{

        friend class QmlFunctionObjectCall;

    public:
        class PropertySource{
        public:
            PropertySource(const QString& pvalue, const QStringList& pbinding) : value(pvalue), binding(pbinding){}

            QString     value;
            QStringList binding;
        };

    public:
        typedef std::shared_ptr<Source>       Ptr;
        typedef std::shared_ptr<const Source> ConstPtr;

    public:
        static Ptr create(){ return Ptr(new Source); }

        const QString& objectType() const{ return m_objectType; }
        const QByteArray& imports() const{ return m_imports; }
        const QString& identifier() const{ return m_identifier; }
        const QString& path() const{ return m_path; }
        const std::map<QString, QmlFunctionObjectCall::Source::PropertySource>& propertyAssignments() const{ return m_propertyAssignments; }

    private:
        Source(){}

        QString     m_path;
        QByteArray  m_imports;
        QString     m_objectType;
        QString     m_identifier;
        std::map<QString, QmlFunctionObjectCall::Source::PropertySource> m_propertyAssignments;
    };

    class Dependency{
    public:
        QmlFunctionObjectCall* dependency;
        QString                dependencyId;
        QStringList            unwrapPath;
        int                    argIndex;
    };

    typedef std::vector<Source::ConstPtr> GraphSource;

    enum Type{
        Unknown = 0,
        ValueInput,
        ValueSink,
        Act,
        ValueFlowGraph
    };

    class GraphEvaluator;

public:
    static QmlFunctionObjectCall* createFromSource(
        ViewEngine* ve, const QmlFunctionObjectCall::Source::ConstPtr& source
    );
    static std::vector<Source::ConstPtr> extractGraphSource(
        const QString& path, const QByteArray& imports, const QString& code
    );
    static std::vector<QmlFunctionObjectCall*> mapSourceToObjects(
        ViewEngine* ve, const std::vector<Source::ConstPtr>& source
    );
    static std::vector<QmlFunctionObjectCall*> resolveDependencies(
        std::vector<QmlFunctionObjectCall *> dependencies
    );

    static QJSValue runGraph(ViewEngine* ve, const QJSValue& input, const std::vector<QmlFunctionObjectCall*>& graph);

    QObject* createObject(ViewEngine *ve);

    const Source::ConstPtr& source();
    const QString& identifier();
    const QString& objectType();
    Type type() const;

public:
    QmlFunctionObjectCall(const Source::ConstPtr& source);
    bool hasUnmetDependencies();

    Source::ConstPtr m_source;
    Type             m_type;
    QObject*         object;
    QJSValueList     arguments;
    std::map<QString, Dependency> propertyDependencies;
};

inline const QmlFunctionObjectCall::Source::ConstPtr &QmlFunctionObjectCall::source(){
    return m_source;
}

inline const QString &QmlFunctionObjectCall::identifier(){
    return m_source->identifier();
}

inline const QString &QmlFunctionObjectCall::objectType(){
    return m_source->objectType();
}

inline QmlFunctionObjectCall::Type QmlFunctionObjectCall::type() const{
    return m_type;
}

}// namespace

#endif // LVQMLFUNCTIONOBJECTCALL_H
