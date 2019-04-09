#include "texteditnodedebugmodel.h"
#include "textedit_p.h"

#ifdef LV_EDITOR_DEBUG


namespace lv {

TextEditNodeDebugModel::TextEditNodeDebugModel(TextEdit* parent):
    QAbstractListModel(parent), m_parentTextEdit(parent), m_size(0)
{
    m_roles[TextEditNodeDebugModel::BlockNumber]        = "blockNumber";
    m_roles[TextEditNodeDebugModel::LineText]           = "lineText";
    m_roles[TextEditNodeDebugModel::BlockText]          = "blockText";
    m_roles[TextEditNodeDebugModel::HiddenByCollapse]   = "hiddenByCollapse";
    m_roles[TextEditNodeDebugModel::HiddenByPalette]    = "hiddenByPalette";
    m_roles[TextEditNodeDebugModel::Offset]             = "offset";
}

QVariant TextEditNodeDebugModel::data(const QModelIndex &index, int role) const
{
    unsigned row = static_cast<unsigned>(index.row());

    switch (role)
    {
    case TextEditNodeDebugModel::BlockNumber:
        return m_entries[row].m_blockNumber;
    case TextEditNodeDebugModel::LineText:
        return m_entries[row].m_lineText;
    case TextEditNodeDebugModel::BlockText:
        return m_entries[row].m_blockText;
    case TextEditNodeDebugModel::HiddenByCollapse:
        return m_entries[row].m_hiddenByCollapse;
    case TextEditNodeDebugModel::HiddenByPalette:
        return m_entries[row].m_hiddenByPalette;
    case TextEditNodeDebugModel::Offset:
        return m_entries[row].m_offset;
    default:
        return QVariant();
    }
}

void TextEditNodeDebugModel::updateModel(int pos, int size)
{
    if (pos < m_entries.size())
    {
        beginRemoveRows(QModelIndex(), pos, m_entries.size()-1);
        endRemoveRows();
    }
    m_size = pos;

    if (size != m_entries.size())
        m_entries.resize(size);

    beginInsertRows(QModelIndex(), pos, size-1);
    for (int i = pos; i < size; ++i)
    {
        m_entries[i] = m_parentTextEdit->getDebugEntry(i);
    }
    endInsertRows();

    m_size = size;
}

} // namespace

#endif
