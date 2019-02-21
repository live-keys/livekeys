#ifndef LVMEMORY_H
#define LVMEMORY_H

#include "live/lvviewglobal.h"
#include "live/viewcontext.h"
#include "live/shared.h"
#include "live/visuallog.h"
#include <QObject>
#include <unordered_map>

namespace lv{

class LV_VIEW_EXPORT Memory : public QObject{

    Q_OBJECT

private:
    template<typename T, typename TI>
    class Cell{
    public:
        std::list<TI*> objects;
        int            requests;
        size_t         allocationSize;

        Cell() : requests(0), allocationSize(0){}

        bool hasObject(){ return objects.size() > 0; }
        TI* takeObject(){ TI* t = objects.front(); objects.pop_front(); return t; }
    };

    class ContainerBase{
    public:
        virtual ~ContainerBase(){}
    };

    template<typename T, typename TI>
    class Container : public ContainerBase{
    public:
        std::unordered_map<qint64, Memory::Cell<T, TI>* > m_cells;

        ~Container(){}
    };

public:
    explicit Memory(QObject *parent = nullptr);
    ~Memory();

    template<typename T, typename TI, typename ...Args>
    static TI* alloc(Args... args){
        if ( container<T, TI>().m_cells.empty() )
            return T::memoryAlloc(args...);
        qint64 index = T::memoryIndex(args...);
        auto it = container<T, TI>().m_cells.find(index);
        if ( it == container<T, TI>().m_cells.end() ) {
            return T::memoryAlloc(args...);
        }
        Cell<T, TI>* cell = it->second;
        return cell->hasObject() ? cell->takeObject() : T::memoryAlloc(args...);
    }

    template<typename T, typename TI, typename ...Args>
    static void free(T* parent, TI* arg){
        if ( container<T, TI>().m_cells.empty() )
            T::free(arg);
        qint64 index = T::memoryIndex(parent);
        auto it = container<T, TI>().m_cells.find(index);
        if ( it == container<T, TI>().m_cells.end() ) {
            T::free(arg);
        }
        Cell<T, TI>* cell = it->second;
        if ( cell->allocationSize > cell->objects.size() ){
            cell->objects.push_back(arg);
        }
    }

    template<typename T, typename TI, typename ...Args>
    static void reserve(const T* parent, size_t size){
        qint64 index = T::memoryIndex(parent);
        auto it = container<T, TI>().m_cells.find(index);
        if ( it == container<T, TI>().m_cells.end() ) {
            Cell<T, TI>* cell = new Cell<T, TI>();
            cell->allocationSize = size;
            Container<T, TI>().m_cells[index] = cell;
        }
    }

    static Memory* i(){ return ViewContext::instance().memory(); }
    static void gc();

public slots:
    void reloc();
    void recycleSize(Shared* o, int size) const;

private:
    template<typename T, typename TI>
    static Container<T, TI>& container(){
        static Container<T, TI> instance;
        return instance;
    }

    static std::vector<ContainerBase*>& containers(){
        static std::vector<ContainerBase*> containers;
        return containers;
    }
};

}// namespace

#endif // LVMEMORY_H
