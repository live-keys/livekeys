#ifndef LVTHEMECONTAINER_H
#define LVTHEMECONTAINER_H

#include <QObject>
#include <QAbstractListModel>
#include "live/lveditorglobal.h"

namespace lv{

class Theme;

/// \private
class ThemeContainer : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(lv::Theme* current READ current NOTIFY currentChanged)

public:
    enum Roles{
        Label = Qt::UserRole + 1
    };

public:
    explicit ThemeContainer(const QString& type = "", QObject *parent = 0);
    virtual ~ThemeContainer();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void addTheme(const QString& name, const QString& path);

    lv::Theme* current() const;

public slots:
    void setTheme(const QString& name);

signals:
    void currentChanged();

private:
    QString                m_type;
    QHash<int, QByteArray> m_roles;
    QMap<QString, QString> m_themes;
    lv::Theme*             m_current;
};

inline Theme *ThemeContainer::current() const{
    return m_current;
}

} // namespace

#endif // LVTHEMECONTAINER_H
