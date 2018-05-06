#include "livepalettelist.h"
#include "livepalettecontainer.h"
#include <QQmlEngine>

namespace lv{

LivePaletteList::LivePaletteList(LivePaletteContainer *container, QObject *parent)
    : QAbstractListModel(parent)
    , m_paletteContainer(container)
{

}

LivePaletteList::~LivePaletteList(){
}

QVariant LivePaletteList::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_palettes.size() )
        return QVariant();
    if ( role == Qt::UserRole )
        return m_paletteContainer->paletteName(m_palettes[index.row()]);
    return QVariant();
}

QHash<int, QByteArray> LivePaletteList::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "name";
    return roles;
}

void LivePaletteList::append(LivePaletteLoader *loader){
    m_palettes.append(loader);
}

LivePalette* LivePaletteList::loadAt(int index){
    if ( index < m_palettes.size() ){
        LivePalette* lp = m_paletteContainer->createPalette(m_palettes[index]);
        qmlEngine(this)->setObjectOwnership(lp, QQmlEngine::CppOwnership);
        return lp;
    }
    return 0;
}



}// namespace
