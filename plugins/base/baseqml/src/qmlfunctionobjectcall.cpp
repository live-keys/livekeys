#include "qmlfunctionobjectcall.h"
#include "qmlvalueflowgraph.h"
#include "live/visuallogqt.h"
#include "live/qmlact.h"

#include "live/documentqmlinfo.h"

namespace lv{


// class QmlFunctionObjectCall::GraphEvaluator
// ----------------------------------------------------------------------------

class QmlFunctionObjectCall::GraphEvaluator{

public:
    typedef std::shared_ptr<GraphEvaluator> Ptr;

public:
    ~GraphEvaluator();

    static QJSValue run(ViewEngine* ve, const QJSValue& input, const std::vector<QmlFunctionObjectCall*>& graph);

private:
    static QJSValue step(const GraphEvaluator::Ptr& ge);

    GraphEvaluator(ViewEngine* ve, const QJSValue& input, const std::vector<QmlFunctionObjectCall*>& graph);

    ViewEngine*                                   m_viewEngine;
    QJSValue                                      m_input;
    std::vector<QmlFunctionObjectCall*>           m_graph;
    std::vector<QmlFunctionObjectCall*>::iterator m_graphIt;
    std::map<QString, QJSValue>                   m_results;
    std::vector<QmlStreamProvider*>               m_resultStreams;
    QmlPromise*                                   m_waitOn;
    QmlPromise::Resolver*                         m_asyncResult;
};

QmlFunctionObjectCall::GraphEvaluator::GraphEvaluator(ViewEngine *ve, const QJSValue &input, const std::vector<QmlFunctionObjectCall *> &graph)
    : m_viewEngine(ve)
    , m_input(input)
    , m_graph(graph)
    , m_waitOn(nullptr)
    , m_asyncResult(nullptr)
{
    m_graphIt = m_graph.begin();
}

QmlFunctionObjectCall::GraphEvaluator::~GraphEvaluator(){
}

QJSValue QmlFunctionObjectCall::GraphEvaluator::run(
        ViewEngine *ve, const QJSValue &input, const std::vector<QmlFunctionObjectCall *> &graph)
{
    QmlFunctionObjectCall::GraphEvaluator::Ptr ge = QmlFunctionObjectCall::GraphEvaluator::Ptr(new QmlFunctionObjectCall::GraphEvaluator(ve, input, graph));
    return QmlFunctionObjectCall::GraphEvaluator::step(ge);
}

QJSValue QmlFunctionObjectCall::GraphEvaluator::step(const QmlFunctionObjectCall::GraphEvaluator::Ptr &ge){
    if ( ge->m_waitOn ){
        Shared::unref(ge->m_waitOn);
        ge->m_waitOn = nullptr;
    }
    while ( ge->m_graphIt != ge->m_graph.end() ){
        QmlFunctionObjectCall* fo = *ge->m_graphIt;
        if ( fo->type() == QmlFunctionObjectCall::ValueInput ){
            ge->m_results[fo->identifier()] = ge->m_input;
        } else if ( fo->type() == QmlFunctionObjectCall::ValueSink ){
            if ( fo->propertyDependencies.size() ){
                if ( ge->m_asyncResult ){
                    ge->m_asyncResult->resolve(ge->m_results[fo->propertyDependencies.begin()->second.dependencyId]);
                    return QJSValue();
                } else {
                    return ge->m_results[fo->propertyDependencies.begin()->second.dependencyId];
                }
            }
        } else {
            QJSValue res;
            QJSValueList argCopy = fo->arguments;
            for ( auto depIt = fo->propertyDependencies.begin(); depIt != fo->propertyDependencies.end(); ++depIt ){
                argCopy[depIt->second.argIndex] = ge->m_results[depIt->second.dependencyId];
            }

            if ( fo->type() == QmlFunctionObjectCall::Act ){
                QmlAct* act = qobject_cast<QmlAct*>(fo->object);
                res = act->apply(argCopy);
            } else if ( fo->type() == QmlFunctionObjectCall::ValueFlowGraph ){
                QmlValueFlowGraph* flow = qobject_cast<QmlValueFlowGraph*>(fo->object);
                res = flow->apply(argCopy);
            }

            if ( res.isError() ){
                Exception e = CREATE_EXCEPTION(lv::Exception, "Run Error: " + res.toString().toStdString(), Exception::toCode("~Run"));
                ge->m_viewEngine->throwError(&e);
                //TODO: Or promise reject
                return QJSValue();
            } else {
                QmlPromise* p = QmlPromise::fromValue(res);
                if ( p ){
                    ge->m_asyncResult = QmlPromise::Resolver::createWithPromise(ge->m_viewEngine);
                    ++ge->m_graphIt;
                    ge->m_waitOn = p;
                    Shared::ref(ge->m_waitOn);
                    p->then(
                        [ge, fo](QJSValue v){
                            ge->m_results[fo->identifier()] = v;
                            QmlFunctionObjectCall::GraphEvaluator::step(ge);
                        },
                        [ge](QJSValue v){ ge->m_graphIt = ge->m_graph.end(); ge->m_asyncResult->reject(v); }
                    );
                    for ( auto it = ge->m_resultStreams.begin(); it != ge->m_resultStreams.end(); ++it ){
                        (*it)->resume();
                    }
                    //TODO: Store these for deletion
                    ge->m_resultStreams.clear();

                    return ge->m_viewEngine->engine()->newQObject(ge->m_asyncResult->promise());
                } else {
                    QmlStream* stream = QmlStream::fromValue(res);
                    if ( stream ){
                        ge->m_resultStreams.push_back(stream->provider());
                    }
                    ge->m_results[fo->identifier()] = res;
                }
            }
        }
        ++ge->m_graphIt;
    }
    return QJSValue();
}


// class QmlFunctionObjectCall
// ----------------------------------------------------------------------------

QObject *QmlFunctionObjectCall::createObject(ViewEngine* ve){
    QByteArray source = m_source->imports() + "\n" + m_source->objectType().toUtf8() + "{\n";

    for ( auto pit = m_source->propertyAssignments().begin(); pit != m_source->propertyAssignments().end(); ++pit ){
        const QmlFunctionObjectCall::Source::PropertySource& ps = pit->second;
        if ( ps.binding.isEmpty() ){
            source += pit->first.toUtf8() + ":" + ps.value.toUtf8() + "\n";
        }
    }

    source += "}";
    QString creationPath = m_source->path().mid(0, m_source->path().length() - 4) + m_source->objectType() + ".qml";

    ViewEngine::ComponentResult::Ptr cr = ve->createObject(creationPath, source, nullptr);
    if ( cr->hasError() ){
        THROW_EXCEPTION(lv::Exception, QmlError::join(cr->errors).message().toStdString(), lv::Exception::toCode("Engine"));
    }

    object = cr->object;
    return object;
}

QmlFunctionObjectCall::QmlFunctionObjectCall(const QmlFunctionObjectCall::Source::ConstPtr &source)
    : m_source(source)
    , m_type(QmlFunctionObjectCall::Unknown)
    , object(nullptr)
{
}

bool QmlFunctionObjectCall::hasUnmetDependencies(){
    for ( auto it = propertyDependencies.begin(); it != propertyDependencies.end(); ++it ){
        if ( it->second.dependency == nullptr )
            return true;
    }
    return false;
}


QmlFunctionObjectCall *QmlFunctionObjectCall::createFromSource(ViewEngine* ve, const QmlFunctionObjectCall::Source::ConstPtr &fosource){
    QmlFunctionObjectCall* fo = new QmlFunctionObjectCall(fosource);

    for ( auto pit = fosource->propertyAssignments().begin(); pit != fosource->propertyAssignments().end(); ++pit ){
        const QmlFunctionObjectCall::Source::PropertySource& ps = pit->second;
        if ( ps.binding.length() > 1 ){
            QmlFunctionObjectCall::Dependency dep;
            dep.dependencyId = ps.binding[0];
            dep.unwrapPath = ps.binding.mid(1);
            dep.dependency = nullptr;
            dep.argIndex = -1;
            fo->propertyDependencies[pit->first] = dep;
        }
    }

    if ( fo->objectType() == "ValueFlowInput" ){
        fo->m_type = QmlFunctionObjectCall::ValueInput;
    } else if ( fo->objectType() == "ValueFlowSink" ){
        fo->m_type = QmlFunctionObjectCall::ValueSink;
    } else {
        fo->createObject(ve);
        QmlAct* act = qobject_cast<QmlAct*>(fo->object);
        if ( act ){
            fo->m_type = QmlFunctionObjectCall::Act;
            fo->arguments = act->argumentCallList();
            for ( int i = 0; i < fo->arguments.length(); ++i ){
                QJSValue val = fo->arguments[i];
                if ( val.isString() ){
                    QString valStr = val.toString();
                    if ( valStr.startsWith('$') ){
                        QString propName = valStr.mid(1);
                        auto foundDependency = fo->propertyDependencies.find(propName);
                        if ( foundDependency != fo->propertyDependencies.end() ){
                            foundDependency->second.argIndex = i;
                        } else {
                            fo->arguments[i] = ve->engine()->toScriptValue(QQmlProperty(act, propName).read());
                        }
                    }
                }
            }

        } else {
           QmlValueFlowGraph* flow = qobject_cast<QmlValueFlowGraph*>(fo->object);
           if ( !flow ){
               THROW_EXCEPTION(lv::Exception, Utf8("Only Act and ValueFlowGraph types are allowed in a ValueFlowGraph."), lv::Exception::toCode("~Act"));
           }
           fo->m_type = QmlFunctionObjectCall::ValueFlowGraph;

           QString propName = "input";
           QJSValue val;
           int index = 0;
           auto foundDependency = fo->propertyDependencies.find(propName);
           if ( foundDependency != fo->propertyDependencies.end() ){
               foundDependency->second.argIndex = index;
               fo->arguments.append("$input");
           } else {
               val = ve->engine()->toScriptValue(QQmlProperty(flow, propName).read());
               fo->arguments.append(val);
           }
        }
    }
    return fo;
}

std::vector<QmlFunctionObjectCall::Source::ConstPtr> QmlFunctionObjectCall::extractGraphSource(const QString &path, const QByteArray &imports, const QString &code){
    std::vector<QmlFunctionObjectCall::Source::ConstPtr> result;


    DocumentQmlInfo::Ptr dqi = DocumentQmlInfo::create(path);
    dqi->parse(code);
    dqi->createRanges();

    auto objects = dqi->createObjects();
    DocumentQmlValueObjects::RangeObject* valueFlowGraphValueFlow = objects->root();

    for ( int i = 0; i < valueFlowGraphValueFlow->children.size(); ++i ){
        DocumentQmlValueObjects::RangeObject* vfChild = valueFlowGraphValueFlow->children[i];

        QmlFunctionObjectCall::Source::Ptr fosource = QmlFunctionObjectCall::Source::create();
        QString objectType = code.mid(vfChild->begin, vfChild->identifierEnd - vfChild->begin);
        QStringList objectTypeSegm = objectType.split('.');

        fosource->m_objectType = objectTypeSegm.last();
        fosource->m_imports = imports;
        fosource->m_path = path;
        result.push_back(fosource);

        for ( int i = 0; i < vfChild->properties.size(); ++i ){
            DocumentQmlValueObjects::RangeProperty* rp = static_cast<DocumentQmlValueObjects::RangeProperty*>(vfChild->properties[i]);
            QString name = code.mid(rp->begin, rp->propertyEnd - rp->begin);
            if ( name == "id" ){
                fosource->m_identifier = code.mid(rp->valueBegin, rp->end - rp->valueBegin);
            } else {
                QString value = code.mid(rp->valueBegin, rp->end - rp->valueBegin);
                QStringList binding = DocumentQmlInfo::readConnection(value);
                QmlFunctionObjectCall::Source::PropertySource ps(value, binding);
                fosource->m_propertyAssignments.insert(std::make_pair(name, ps));
            }
        }
    }

    return result;
}

std::vector<QmlFunctionObjectCall *> QmlFunctionObjectCall::mapSourceToObjects(ViewEngine *ve, const std::vector<QmlFunctionObjectCall::Source::ConstPtr> &sources){
    std::vector<QmlFunctionObjectCall*> result;
    for ( size_t i = 0; i < sources.size(); ++i ){
        QmlFunctionObjectCall::Source::ConstPtr fosource = sources[i];
        QmlFunctionObjectCall* fo = QmlFunctionObjectCall::createFromSource(ve, fosource);
        result.push_back(fo);
    }
    return result;
}

std::vector<QmlFunctionObjectCall *> QmlFunctionObjectCall::resolveDependencies(std::vector<QmlFunctionObjectCall *> graph){
    std::vector<QmlFunctionObjectCall *> resolvedGraph;
    while ( graph.size() ){
        int added = 0;
        auto collectedIt = graph.begin();

        while ( collectedIt != graph.end() ){
            QmlFunctionObjectCall* fo = *collectedIt;
            if ( !fo->hasUnmetDependencies() ){
                resolvedGraph.push_back(fo);
                collectedIt = graph.erase(collectedIt);
                ++added;
            } else {
                for ( auto depit = fo->propertyDependencies.begin(); depit != fo->propertyDependencies.end(); ++depit ){
                    if ( depit->second.dependency == nullptr ){
                        QString dependencyId = depit->second.dependencyId;
                        for ( auto callIt = resolvedGraph.begin(); callIt != resolvedGraph.end(); ++callIt ){
                            if ( (*callIt)->identifier() == dependencyId ){
                                depit->second.dependency = *callIt;
                                break;
                            }
                        }
                    }
                }

                if ( !fo->hasUnmetDependencies() ){
                    resolvedGraph.push_back(fo);
                    collectedIt = graph.erase(collectedIt);
                    ++added;
                } else {
                    ++collectedIt;
                }
            }
        }
        if ( added == 0 ){
            THROW_EXCEPTION(lv::Exception, "Function graph might have dependency cycles, could not complete.", lv::Exception::toCode("~Dependency"));
            return std::vector<QmlFunctionObjectCall *>();
        }
    }
    return resolvedGraph;
}

QJSValue QmlFunctionObjectCall::runGraph(ViewEngine *ve, const QJSValue &input, const std::vector<QmlFunctionObjectCall *> &graph){
    return QmlFunctionObjectCall::GraphEvaluator::run(ve, input, graph);
}

}// namespace
