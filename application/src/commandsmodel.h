#ifndef COMMANDSMODEL_H
#define COMMANDSMODEL_H

#include <QAbstractListModel>
#include "live/lvbaseglobal.h"
#include <map>
#include <vector>

namespace lv {

class Commands;
class KeyMap;

class CommandsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles{
        Command = Qt::UserRole + 4,
        Description,
        Shortcuts
    };

    CommandsModel(Commands* commands, KeyMap* keymap);
    ~CommandsModel() {}

    class CommandEntry
    {
    public:
        QString m_command;
        QString m_description;
        QString m_shortcuts;

        CommandEntry(QString comm = "", QString desc = "", QString shorts = ""):
            m_command(comm), m_description(desc), m_shortcuts(shorts) {}
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &) const;

public Q_SLOTS:
    void updateAvailableCommands();

private:
    void updateFilter();
    void setFilter(const QString &filter);

    std::vector<CommandEntry> m_filteredCommands;
    std::map<QString, CommandEntry> m_commandEntries;
    QString m_filter;
    Commands* m_commands;
    KeyMap* m_keymap;
    bool m_isIndexing;
    QHash<int, QByteArray> m_roles;

    Q_DISABLE_COPY(CommandsModel)
};


} // namespace
#endif // COMMANDSMODEL_H
