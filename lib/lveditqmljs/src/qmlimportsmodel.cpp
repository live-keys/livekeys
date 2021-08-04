#include "qmlimportsmodel.h"
#include "live/codeqmlhandler.h"

namespace lv{

QmlImportsModel::QmlImportsModel(ViewEngine *engine, QObject *parent)
    : QAbstractListModel(parent)
    , m_engine(engine)
{
    m_roles[Module] = "module";
    m_roles[Version] = "version";
    m_roles[Qualifier] = "qualifier";
    m_roles[Line] = "line";
}

int QmlImportsModel::firstBlock()
{
    if (m_data.empty()) return -1;
    return m_data[0].location().line() - 1;
}

int QmlImportsModel::lastBlock()
{
    if (m_data.empty()) return -1;
    return m_data[m_data.size()-1].location().line() - 1;
}

void QmlImportsModel::commit(QString m, QString v, QString q)
{
    if (m == "" || v == "") return;
    auto handler = static_cast<CodeQmlHandler*>(parent());
    if (!handler) return;

    QString line = QString() + "import " + m + " " + v;
    if (q != "") line += " as " + q;

    handler->document()->addEditingState(ProjectDocument::Palette);
    handler->document()->addLineAtBlockNumber(line, lastBlock() + 1);
    handler->document()->removeEditingState(ProjectDocument::Palette);

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(createItem(m,v,q,lastBlock()+1));
    endInsertRows();

    handler->__updateScope();

    emit countChanged();
}

void QmlImportsModel::erase(int line)
{
    int i = 0;
    auto handler = static_cast<CodeQmlHandler*>(parent());
    if (!handler) return;
    for (;i < m_data.size(); ++i)
        if (m_data[i].location().line() == line)
            break;

    handler->document()->addEditingState(ProjectDocument::Palette);
    handler->document()->removeLineAtBlockNumber(line - 1);
    handler->document()->removeEditingState(ProjectDocument::Palette);

    beginRemoveRows(QModelIndex(), i, i);
    m_data.erase(m_data.begin() + i);
    endRemoveRows();

    handler->__updateScope();

    for (int k = i; k<m_data.size();++k){
        m_data[k].setLocation(Document::Location(m_data[k].location().line() - 1, m_data[k].location().column()));
    }

    emit countChanged();
}

QJSValue QmlImportsModel::getImportAtUri(const QString &uri){
    for ( const DocumentQmlInfo::Import& itemData : m_data ){
        if ( itemData.uri() == uri ){
            QQmlEngine* e = m_engine->engine();
            QJSValue result = e->newObject();
            result.setProperty("uri", itemData.uri());
            result.setProperty("as", itemData.as());
            result.setProperty("version", itemData.versionString());
            return result;
        }
    }
    return QJSValue();
}

QVariant QmlImportsModel::data(const QModelIndex &index, int role) const{
    int dataIndex = index.row();
    if ( dataIndex > m_data.count() )
        return QVariant();

    if ( role == Module ){
        return m_data[dataIndex].uri();
    } else if ( role == Version ){
        return m_data[dataIndex].versionString();
    } else if ( role == Qualifier ){
        return m_data[dataIndex].as();
    } else if (role == Line){
        return m_data[dataIndex].location().line();
    }
    return QVariant();
}

void QmlImportsModel::addItem(const QString &m, const QString &v, const QString &q, int l)
{
    m_data.push_back(createItem(m, v, q, l));
}

DocumentQmlInfo::Import QmlImportsModel::createItem(const QString &m, const QString &v, const QString &q, int l){
    QString major = "1"; QString minor = "0";
    QStringList version = v.split(".");
    if ( version.length() == 2 ){
        major = version[0];
        minor = version[1];
    }

    return DocumentQmlInfo::Import(
       DocumentQmlInfo::Import::Library,
       m, // path or name
       m, //name
       q, // as
       major.toInt(),
       minor.toInt(),
       Document::Location(l, 0)
    );
}

void QmlImportsModel::setImports(const QList<DocumentQmlInfo::Import> &imports){
    beginResetModel();
    m_data = imports;
    endResetModel();
}

} // namespace
