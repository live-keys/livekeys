#ifndef TEXTEDITNODEDEBUGMODEL_H
#define TEXTEDITNODEDEBUGMODEL_H

#include "qabstractitemmodel.h"

#include "lveditorglobal.h"

#ifdef LV_EDITOR_DEBUG

namespace lv {

class TextEdit;

class TextEditNodeDebugModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles{
        BlockNumber = Qt::UserRole + 1,
        LineText,
        BlockText,
        HiddenByCollapse,
        HiddenByPalette,
        Offset
    };

    class Entry{
    public:
        Entry(int bn, QString lt, QString bt, bool col = false, bool pal = false, int off = 0):
            m_blockNumber(bn), m_lineText(lt), m_blockText(bt), m_hiddenByCollapse(col),
            m_hiddenByPalette(pal), m_offset(off) {}

        Entry() : Entry(0, "", "") {}
        QString toString(){
            std::string s = std::to_string(m_blockNumber) + " "
                    + m_lineText.toStdString() + " "
                    + m_blockText.toStdString() + " "
                    + (m_hiddenByCollapse ? "true " : "false ")
                    + (m_hiddenByPalette ? "true " : "false ")
                    + std::to_string(m_offset);

            return QString(s.c_str());
        }

        int m_blockNumber;
        QString m_lineText;
        QString m_blockText;
        bool m_hiddenByCollapse;
        bool m_hiddenByPalette;
        int m_offset;
    };

    TextEditNodeDebugModel(TextEdit* parent=nullptr);


    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void updateModel(int pos, int size);
private:
    TextEdit* m_parentTextEdit;
    std::vector<Entry> m_entries;
    unsigned m_size;
    QHash<int, QByteArray> m_roles;
};

inline QHash<int, QByteArray> TextEditNodeDebugModel::roleNames() const{
    return m_roles;
}

inline int TextEditNodeDebugModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(m_size);
}

} // namespace

#endif // TEXTEDITNODEDEBUGMODEL_H

#endif
