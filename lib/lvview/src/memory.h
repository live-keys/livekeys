#ifndef LVMEMORY_H
#define LVMEMORY_H

#include "live/lvviewglobal.h"
#include "live/viewcontext.h"
#include "live/shared.h"
#include "live/visuallog.h"
#include <QObject>
#include <unordered_map>

namespace lv{

/// \private
class LV_VIEW_EXPORT Memory : public QObject{

    Q_OBJECT

private:
    class LV_VIEW_EXPORT CellBase{
    public:
        CellBase() : prev(nullptr), next(nullptr){}
        virtual ~CellBase(){};

        virtual void removeFromContainer(){}
        virtual bool isEmpty(){ return true; }
        virtual size_t freeSpace(size_t size){return size;}

    public:
        CellBase* prev;
        CellBase* next;
    };

    template<typename T, typename TI>
    class Cell : public CellBase{
    public:
        Cell() : CellBase(), requests(0), allocationSize(0){}
        ~Cell(){
            for ( auto it = objects.begin(); it != objects.end(); ++it ){
                T::memoryFree(*it);
            }
            objects.clear();
        }
        size_t freeSpace(size_t required) override{
            size_t totalSpaceRemoved = 0;
            auto it = objects.begin();
            while ( it != objects.end() ){
                delete *it;
                it = objects.erase(it);
                totalSpaceRemoved += allocationSize;
                if ( required < allocationSize ){
                    return totalSpaceRemoved;
                } else {
                    required -= allocationSize;
                }
            }
            return totalSpaceRemoved;
        }
        bool isEmpty() override{
            return objects.empty();
        }
        void removeFromContainer() override{
            auto it = container<T, TI>().m_cells.find(allocationSize);
            if ( it != container<T, TI>().m_cells.end() ){
                container<T, TI>().m_cells.erase(it);
            }
        }
        void addObject(TI* t){ objects.push_back(t); }
        bool hasObject(){ return objects.size() > 0; }
        TI* takeObject(){ TI* t = objects.front(); objects.pop_front(); return t; }

    public:
        std::list<TI*> objects;
        int            requests;
        size_t         allocationSize;
    };

    class LV_VIEW_EXPORT ContainerBase{
    public:
        virtual ~ContainerBase(){};
    };

    template<typename T, typename TI>
    class Container : public ContainerBase{
    public:
        bool isEmpty() const{ return m_cells.empty(); }
        Cell<T, TI>* findCell(size_t memorySize){
            auto it = m_cells.find(memorySize);
            return it == m_cells.end() ? nullptr : it->second;
        }
        Cell<T, TI>* addCell(size_t size){
            Cell<T, TI>* cell = new Cell<T, TI>;
            m_cells[size] = cell;
            return cell;
        }

        std::unordered_map<size_t, Memory::Cell<T, TI>* > m_cells;

        Container(){}
        ~Container(){}
    };

public:
    explicit Memory(QObject *parent = nullptr);
    ~Memory();

    template<typename T, typename TI, typename ...Args>
    static TI* alloc(size_t size, Args... args){
        if ( container<T, TI>().isEmpty() )
            return T::memoryAlloc(args...);

        Cell<T, TI>* cell = container<T, TI>().findCell(size);

        if ( !cell ){
            return T::memoryAlloc(args...);
        }

        cell->requests++;

        for ( auto oit = cell->objects.begin(); oit != cell->objects.end(); ++oit ){
            if ( T::memoryValidate(*oit, args...) ){
                TI* t = *oit;
                cell->objects.erase(oit);
                usedSpace() -= size;
                return t;
            }
        }
        return T::memoryAlloc(args...);
    }

    template<typename T, typename TI, typename ...Args>
    static void free(T* parent, TI* arg){
        if ( !totalSpace() ){
            T::memoryFree(arg);
            return;
        }

        size_t size = T::memorySize(parent);
        Cell<T, TI>* cell = container<T, TI>().findCell(size);

        if ( !cell  || cell->requests == 0){
            T::memoryFree(arg);
            return;
        }

        cell->addObject(arg);
        cell->requests--;
        bringCellToFront(cell);

        usedSpace() += size;

        if ( usedSpace() > totalSpace() ){
            clearSpaceUpTo(totalSpace() / 2);
        }
    }

    template<typename T, typename TI, typename ...Args>
    static void reserve(T* parent){
        qint64 index = T::memorySize(parent);
        Cell<T, TI>* cell = container<T, TI>().findCell(index);
        if ( !cell ){
            cell = new Cell<T, TI>();
            cell->allocationSize = index;
            cell->requests++;
            container<T, TI>().m_cells[index] = cell;
        } else {
            cell->requests++;
        }
    }

    static Memory* i(){ return ViewContext::instance().memory(); }
    static void gc();
    static void enable(size_t size){ setSize(size); }
    static void setSize(size_t size){ totalSpace() = size; }
    static size_t size(){ return totalSpace(); }
    static size_t used(){ return usedSpace(); }
    static void clear(){ clearSpaceUpTo(0);}


public slots:
    void reloc();
    void recycleSize(Shared* o, int size) const;

private:
    static void clearSpaceUpTo( size_t size){
        std::pair<CellBase*, CellBase*>& r = recents();
        CellBase* last = r.second;

        size_t amountToClear = usedSpace() > size ? usedSpace() - size : 0;

        while ( last ){
            size_t clearedSpace = last->freeSpace(amountToClear);
            usedSpace() -= clearedSpace;
            if ( clearedSpace > amountToClear )
                break;
            amountToClear -= clearedSpace;
            last = last->prev;
        }
    }

    static void addCell(CellBase* cell){
        std::pair<CellBase*, CellBase*>& r = recents();
        if ( r.first == nullptr ){
            r.first = cell;
            r.second = cell;
        } else {
            CellBase* currentFirst = r.first;
            currentFirst->next = cell;
            cell->prev = currentFirst;
            r.first = cell;
        }
    }

    static void bringCellToFront(CellBase* cell){
        takeCell(cell);
        addCell(cell);
    }

    static void takeCell(CellBase* cell){
        CellBase* prevCell = cell->prev;
        CellBase* nextCell = cell->next;
        std::pair<CellBase*, CellBase*>& r = recents();
        if ( r.first == cell )
            r.first = cell->next;
        if ( r.second == cell )
            r.second = cell->prev;
        if ( prevCell )
            prevCell->next = nextCell;
        if ( nextCell )
            nextCell->prev = prevCell;
        cell->prev = nullptr;
        cell->next = nullptr;
    }

    static void deleteCell(CellBase* cell){
        takeCell(cell);
        cell->removeFromContainer();
    }

    template<typename T, typename TI>
    static Container<T, TI>& container(){
        static Container<T, TI> instance;
        return instance;
    }

    static std::pair<CellBase*, CellBase*>& recents(){
        static std::pair<CellBase*, CellBase*> r = std::make_pair<CellBase*, CellBase*>(nullptr, nullptr);
        return r;
    }

    static size_t& totalSpace(){
        static size_t total = 0;
        return total;
    }
    static size_t& usedSpace(){
        static size_t used = 0;
        return used;
    }
};

}// namespace

#endif // LVMEMORY_H
