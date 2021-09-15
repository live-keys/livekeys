#ifndef LVTABLEFIELD_H
#define LVTABLEFIELD_H

#include <QString>
#include <QSharedPointer>

namespace lv{

class TableField{

public:
    typedef QSharedPointer<const TableField> ConstPtr;
    typedef QSharedPointer<TableField>       Ptr;

public:
    static TableField::Ptr create(const QString& name);

    const QString& name() const;

private:
    TableField(const QString& name);

    QString m_name;
};

inline const QString &TableField::name() const{
    return m_name;
}

}// namespace

#endif // LVTABLEFIELD_H
