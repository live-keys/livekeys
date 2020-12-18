#ifndef LVPALETTELIST_H
#define LVPALETTELIST_H

#include <QObject>
#include <QAbstractListModel>
#include "lveditorglobal.h"
#include "live/codepalette.h"

namespace lv{

class PaletteLoader;
class PaletteContainer;
class LV_EDITOR_EXPORT PaletteList : public QAbstractListModel{

    Q_OBJECT

public:
    explicit PaletteList(PaletteContainer* container, QObject *parent = nullptr);
    ~PaletteList();

    void setPosition(int position);

    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent) const;
    QHash<int, QByteArray> roleNames() const;

    void append(PaletteLoader* loader);
    void remove(PaletteLoader* loader);
    PaletteLoader* loaderAt(int index);

public slots:
    int position() const;
    int size() const;
    lv::CodePalette *loadAt(int index);
    QJSValue contentAt(int index);
    void filterOut(const QStringList& names);
private:
    int                   m_position;
    PaletteContainer*     m_paletteContainer;
    QList<PaletteLoader*> m_palettes;
};

/** Sets position */
inline void PaletteList::setPosition(int position){
    m_position = position;
}

/**
 * \brief Override of the respective QAbstractListModel function
 */
inline int PaletteList::rowCount(const QModelIndex &) const{
    return size();
}

/**
 * \brief Remove all appearances of this loader in the palettes
 */
inline void PaletteList::remove(PaletteLoader *loader){
    m_palettes.removeAll(loader);
}

/**
 * \brief Returns loader at given index
 */
inline PaletteLoader *PaletteList::loaderAt(int index){
    return m_palettes.at(index);
}

/**
 * \brief Returns position of palette
 */
inline int PaletteList::position() const{
    return m_position;
}

/** Palettes size */
inline int PaletteList::size() const{
    return m_palettes.size();
}

}// namespace

#endif // LVPALETTELIST_H
