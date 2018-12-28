#include "palettelist.h"
#include "palettecontainer.h"
#include <QQmlEngine>

namespace lv{

PaletteList::PaletteList(PaletteContainer *container, QObject *parent)
    : QAbstractListModel(parent)
    , m_position(-1)
    , m_paletteContainer(container)
{

}

PaletteList::~PaletteList(){
}

QVariant PaletteList::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_palettes.size() )
        return QVariant();
    if ( role == Qt::UserRole )
        return m_paletteContainer->paletteName(m_palettes[index.row()]);
    return QVariant();
}

QHash<int, QByteArray> PaletteList::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "name";
    return roles;
}

void PaletteList::append(PaletteLoader *loader){
    m_palettes.append(loader);
}

CodePalette* PaletteList::loadAt(int index){
    if ( index < m_palettes.size() ){
        CodePalette* lp = m_paletteContainer->createPalette(m_palettes[index]);
        qmlEngine(this)->setObjectOwnership(lp, QQmlEngine::CppOwnership);
        return lp;
    }
    return 0;
}



}// namespace
