#include "startupmodel.h"

namespace lv {

StartupModel::StartupModel(): m_numberOfTitles(9)
{
    m_roles[IsGroupTitle]   = "isGroupTitle";
    m_roles[Name]           = "name";
    m_roles[Label]          = "label";
    m_roles[Path]           = "path";
    m_roles[Description]    = "description";
}

StartupModel::~StartupModel()
{

}

QVariant StartupModel::data(const QModelIndex &index, int role) const
{
    unsigned row = static_cast<unsigned>(index.row());

    if ( row < m_entries.size() ){
        if ( role == IsGroupTitle ){
            return m_entries[row].m_isGroupTitle;
        } else if ( role == Name ){
            return m_entries[row].m_name;
        } else if ( role == Label ){
            return m_entries[row].m_label;
        }  else if ( role == Path ){
            return m_entries[row].m_path;
        } else if ( role == Description ){
            return m_entries[row].m_description;
        }
    }
    return QVariant();
}

int StartupModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(m_entries.size());
}

QHash<int, QByteArray> StartupModel::roleNames() const
{
    return m_roles;
}

void StartupModel::addStartupEntry(StartupModel::StartupEntry e)
{
    beginInsertRows(QModelIndex(), static_cast<int>(m_entries.size()), static_cast<int>(m_entries.size()));
    m_entries.push_back(e);
    if (e.m_isGroupTitle) ++m_numberOfTitles;
    endInsertRows();
}

void StartupModel::removeEntryAt(int i)
{
    beginRemoveRows(QModelIndex(), i, i);
    if (m_entries[i].m_isGroupTitle) --m_numberOfTitles;
    m_entries.erase(m_entries.begin() + i);
    endRemoveRows();
}

void StartupModel::addEntryAt(StartupEntry e, int i)
{
    beginInsertRows(QModelIndex(), i, i);
    if (e.m_isGroupTitle) ++m_numberOfTitles;
    m_entries.insert(m_entries.begin()+i,e);
    endInsertRows();
}

lv::StartupModel::StartupEntry lv::StartupModel::entryAt(int i)
{
    if (i < 0 || i >= static_cast<int>(m_entries.size()) )
        return StartupEntry(false, "", "", "");
    return m_entries[i];
}

void lv::StartupModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, static_cast<int>(m_entries.size() - 1));
    m_entries.clear();
    m_numberOfTitles = 0;
    endRemoveRows();
}

}
