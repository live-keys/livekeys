#include "palettelist.h"
#include "palettecontainer.h"
#include <QQmlEngine>

/**
 * \class lv::PaletteList
 * \brief List of available palettes returned by a PaletteLoader
 *
 * \ingroup lveditor
 */
namespace lv{

/**
 * \brief Default constructor
 */
PaletteList::PaletteList(PaletteContainer *container, QObject *parent)
    : QAbstractListModel(parent)
    , m_position(-1)
    , m_paletteContainer(container)
{

}

/**
 * \brief Default destructor
 */
PaletteList::~PaletteList(){
}

/**
 * \brief Override of the respective QAbstractListModel function
 */
QVariant PaletteList::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_palettes.size() )
        return QVariant();
    if ( role == Qt::UserRole )
        return m_paletteContainer->paletteName(m_palettes[index.row()]);
    return QVariant();
}

/**
 * \brief Override of the respective QAbstractListModel function
 */
QHash<int, QByteArray> PaletteList::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "name";
    return roles;
}

/**
 * \brief Appends the loader to the list
 */
void PaletteList::append(PaletteLoader *loader){
    m_palettes.append(loader);
}

/**
 * \brief Creates a palette from a loader at the given index
 *
 * It's a cpp-owned object!
 */
CodePalette* PaletteList::loadAt(int index){
    if ( index < m_palettes.size() ){
        CodePalette* lp = m_paletteContainer->createPalette(m_palettes[index]);
        qmlEngine(this)->setObjectOwnership(lp, QQmlEngine::CppOwnership);
        return lp;
    }
    return 0;
}



}// namespace
