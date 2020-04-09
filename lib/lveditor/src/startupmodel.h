#ifndef STARTUPMODEL_H
#define STARTUPMODEL_H

#include <QAbstractListModel>

namespace lv {

class StartupModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int numberOfTitles READ numberOfTitles WRITE setNumberOfTitles NOTIFY numberOfTitlesChanged)
public:
    enum Roles{
        IsGroupTitle = Qt::UserRole + 1,
        Name,
        Label,
        Path
    };

    StartupModel();
    ~StartupModel();

    class StartupEntry
    {
    public:
        bool    m_isGroupTitle;
        QString m_name;
        QString m_label;
        QString m_path;

        StartupEntry(bool isTitle, QString name, QString label, QString path)
            : m_isGroupTitle(isTitle)
            , m_name(name)
            , m_label(label)
            , m_path(path) {}
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &) const;
    QHash<int, QByteArray> roleNames() const;

    void addStartupEntry(StartupEntry e);
    void removeEntryAt(int i);
    void addEntryAt(StartupEntry e, int i);
    StartupEntry entryAt(int i);
    void clear();

    int numberOfTitles();
    void setNumberOfTitles(int number);
signals:
    void numberOfTitlesChanged();
private:
    std::vector<StartupEntry> m_entries;
    QHash<int, QByteArray> m_roles;
    int m_numberOfTitles;

    Q_DISABLE_COPY(StartupModel)
};

inline int StartupModel::numberOfTitles()
{
    return m_numberOfTitles;
}


inline void StartupModel::setNumberOfTitles(int number)
{
    m_numberOfTitles = number;
}

}

#endif // STARTUPMODEL_H
