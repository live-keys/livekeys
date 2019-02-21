#ifndef LVMEMORY_H
#define LVMEMORY_H

#include "live/lvviewglobal.h"
#include "live/shared.h"
#include <QObject>
#include <unordered_map>

namespace lv{

class LV_VIEW_EXPORT Memory : public QObject{

    Q_OBJECT

    template<typename T, typename TI = T::InternalType>
    class Cell{
        std::list<TI*> objects;
        int            requests;
        size_t         allocationSize;

        Cell() : requests(0), allocationSize(0){}

        bool hasObject(){ return objects.size() > 0; }
        TI* takeObject(){ return objects.pop_front(); }
    };

    template<typename T, typename TI = T::InternalType>
    class Container{
        std::unordered_map<qint64, Memory::Cell<T, TI> > m_cells;
    };

public:
    explicit Memory(QObject *parent = nullptr);
    ~Memory();

    template<typename T, typename TI = T::InternalType, typename ...Args>
    TI* alloc(Args... args){
        if ( container<T, TI>().m_cells.empty() )
            return T::memoryAlloc(args...);
        qint64 index = T::memoryIndex(args...);
        auto it = container<T, TI>().m_cells.find(index);
        if ( it == container<T, TI>().m_cells.end() ) {
            return T::memoryAlloc(args...);
        }
        Cell<T, TI>* cell = *it;
        return cell->hasObject() ? cell->takeObject() : T::memoryAlloc(args...);
    }

    template<typename T, typename TI = T::InternalType, typename ...Args>
    void free(T* parent, TI* arg){
        if ( container<T, TI>().m_cells.empty() )
            T::memoryFree(arg);
        qint64 index = T::memoryIndex(parent);
        auto it = container<T, TI>().m_cells.find(index);
        if ( it == container<T, TI>().m_cells.end() ) {
            T::memoryFree(arg);
        }
        Cell<T, TI>* cell = *it;
        if ( cell->allocationsSize > cell->objects.size() ){
            cell->objects.push_back(arg);
        }
    }

    template<typename T, typename TI = T::InternalType, typename ...Args>
    void reserve(T* parent, size_t size){
        qint64 index = T::memoryIndex(parent);
        auto it = container<T, TI>().m_cells.find(index);
        if ( it == container<T, TI>().m_cells.end() ) {
            Cell<T, TI>* cell = new Cell<T, TI>();
            cell->allocationSize = size;
            Container<T, TI>().m_cells[index] = cell;
        }
        Cell<T, TI>* cell = *it;
        if ( cell->allocationsSize > cell->objects.size() ){
            cell->objects.push_back(arg);
        }
    }

    static void gc();

public slots:
    void reloc();

private:
    template<typename T, typename TI = T::InternalType>
    static Container<T, TI>& container(){
        static Container<T, TI> instance;
        return instance;
    }
};

}// namespace

#endif // LVMEMORY_H
