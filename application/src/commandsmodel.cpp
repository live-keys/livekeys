#include "commandsmodel.h"
#include <QDebug>
#include "live/keymap.h"
#include "commands.h"

namespace lv {

CommandsModel::CommandsModel(Commands* commands, KeyMap* keymap) : QAbstractListModel (commands),
    m_filter(""), m_commands(commands), m_keymap(keymap), m_isIndexing(false)
{
    m_roles[CommandsModel::Command]     = "command";
    m_roles[CommandsModel::Description] = "description";
    m_roles[CommandsModel::Shortcuts]   = "shortcuts";
}

QVariant CommandsModel::data(const QModelIndex &index, int role) const
{
    unsigned row = static_cast<unsigned>(index.row());

    if ( row < m_filteredCommands.size() ){
        if ( role == CommandsModel::Command ){
            return m_filteredCommands[row].m_command;
        } else if ( role == CommandsModel::Description ){
            return m_filteredCommands[row].m_description;
        } else if ( role == CommandsModel::Shortcuts ){
            return m_filteredCommands[row].m_shortcuts;
        }
    }
    return QVariant();
}

int CommandsModel::rowCount(const QModelIndex &) const{
    return static_cast<int>(m_filteredCommands.size());
}

void CommandsModel::setFilter(const QString &filter){
    if ( m_filter != filter ){
        m_filter = filter;

        if ( !m_isIndexing ){
            beginResetModel();
            updateFilter();
            endResetModel();
        }
    }
}

void CommandsModel::updateFilter()
{
    m_isIndexing = true;
    m_filteredCommands.clear();
    /*for (auto it: m_commands)
    {
        QString desc = it.second.m_description;
        if (m_filter.size() <= desc && desc.contains(m_filter, Qt::CaseInsensitive))
        {
            m_filteredCommands.push_back(it.second);
        }
    }*/
    m_isIndexing = false;
}


void CommandsModel::updateAvailableCommands()
{
    auto commandsMap = m_commands->m_commands;
    for (auto it = commandsMap.begin(); it != commandsMap.end(); ++it)
    {
        QString commandId = it.key();
        m_commandEntries[commandId] = CommandEntry(commandId, it.value()->description);
    }

    for (auto it = m_keymap->commandMap().begin(); it != m_keymap->commandMap().end(); ++it)
    {
        auto key = it.key();
        QString command = it.value().command;
        CommandEntry& entry = m_commandEntries[command];
        if (entry.m_shortcuts != "")
        {
            entry.m_shortcuts += ", ";
        }
        entry.m_shortcuts += m_keymap->getKeyCodeDescription(key);
    }
}


} // namespace
