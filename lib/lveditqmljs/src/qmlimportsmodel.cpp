#include "qmlimportsmodel.h"
#include "live/codeqmlhandler.h"
namespace lv{

QmlImportsModel::QmlImportsModel(QObject *parent): QAbstractListModel(parent)
{
    m_roles[Module] = "module";
    m_roles[Version] = "version";
    m_roles[Qualifier] = "qualifier";
    m_roles[Line] = "line";
}

int QmlImportsModel::firstBlock()
{
    if (m_data.empty()) return -1;
    return m_data[0].line;
}

int QmlImportsModel::lastBlock()
{
    if (m_data.empty()) return -1;
    return m_data[m_data.size()-1].line;
}

void QmlImportsModel::commit(QString m, QString v, QString q)
{
    if (m == "" || v == "") return;
    auto handler = static_cast<CodeQmlHandler*>(parent());
    if (!handler) return;

    QString line = QString() + "import " + m + " " + v;
    if (q != "") line += " as " + q;
    handler->addLineAtPosition(line, lastBlock()+1);

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(ItemData(m,v,q,lastBlock()+1));
    endInsertRows();


    emit countChanged();
}

void QmlImportsModel::erase(int pos)
{
    int i = 0;
    auto handler = static_cast<CodeQmlHandler*>(parent());
    if (!handler) return;
    for (;i < m_data.size(); ++i)
        if (m_data[i].line == pos) break;

    handler->removeLineAtPosition(pos);

    beginRemoveRows(QModelIndex(), i, i);
    m_data.erase(m_data.begin() + i);
    endRemoveRows();

    for (int k = i; k<m_data.size();++k)
        --m_data[k].line;

    emit countChanged();
}

QVariant QmlImportsModel::data(const QModelIndex &index, int role) const{
    unsigned dataIndex = index.row();
    if ( role == Module ){
        return m_data[dataIndex].module;
    } else if ( role == Version ){
        return m_data[dataIndex].version;
    } else if ( role == Qualifier ){
        return m_data[dataIndex].qualifier;
    } else if (role == Line){
        return m_data[dataIndex].line;
    }
    return QVariant();
}

void QmlImportsModel::addItem(const QString &m, const QString &v, const QString &q, int l)
{
    m_data.push_back(ItemData(m, v, q, l));
}

QmlImportsModel::ItemData::ItemData(const QString &m, const QString &v, const QString &q, int l)
    : module(m), version(v), qualifier(q), line(l) {}

}
