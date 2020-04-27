#include "memorytest.h"

#include "live/memory.h"
#include "live/viewengine.h"
#include "live/mlnode.h"
#include "live/viewcontext.h"
#include "live/exception.h"

Q_TEST_RUNNER_REGISTER(MemoryTest);

using namespace lv;

class MemoryTestStubResource{

public:
    static int numAllocations;

    MemoryTestStubResource(size_t size)
        : m_data(new char[size]), m_dataSize(size)
    {
        numAllocations++;
    }
    ~MemoryTestStubResource(){
        --numAllocations;
        delete []m_data;
    }

    char*  m_data;
    size_t m_dataSize;
};

int MemoryTestStubResource::numAllocations = 0;

class MemoryTestStub{

    friend class Memory;

public:
    MemoryTestStub(size_t size){
        m_stubPrivate = lv::Memory::alloc<MemoryTestStub, MemoryTestStubResource>(size, size);
    }
    ~MemoryTestStub(){
        lv::Memory::free<MemoryTestStub, MemoryTestStubResource>(this, m_stubPrivate);
    }

    static MemoryTestStubResource* memoryAlloc(size_t size){
        return new MemoryTestStubResource(size);
    }
    static size_t memorySize(MemoryTestStub* object){
        return object->m_stubPrivate->m_dataSize;
    }
    static void memoryFree(MemoryTestStubResource* object){
        delete object;
    }
    static bool memoryValidate(MemoryTestStubResource*, size_t){
        return true;
    }

private:
    MemoryTestStubResource* m_stubPrivate;

};

MemoryTest::MemoryTest(QObject *parent)
    : QObject(parent)
{
}

MemoryTest::~MemoryTest(){
}

void MemoryTest::initTestCase(){
    lv::Memory::setSize(5000);
}

void MemoryTest::testNoStorage(){
    QVERIFY(Memory::used() == 0);

    auto mts = new MemoryTestStub(2000);
    QVERIFY(MemoryTestStubResource::numAllocations == 1);
    QVERIFY(Memory::used() == 0);
    delete mts;

    QVERIFY(MemoryTestStubResource::numAllocations == 0);
    QVERIFY(Memory::used() == 0);

    Memory::clear();
}

void MemoryTest::testStorage(){
    QVERIFY(Memory::used() == 0);

    auto mts = new MemoryTestStub(2000);
    QVERIFY(MemoryTestStubResource::numAllocations == 1);
    QVERIFY(Memory::used() == 0);

    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);

    delete mts;

    QVERIFY(MemoryTestStubResource::numAllocations == 1);
    QVERIFY(Memory::used() == 2000);

    mts = new MemoryTestStub(2000);
    QVERIFY(MemoryTestStubResource::numAllocations == 1);
    QVERIFY(Memory::used() == 0);

    delete mts;

    Memory::clear();
}

void MemoryTest::testMultipleStorage(){
    QVERIFY(Memory::used() == 0);

    auto mts = new MemoryTestStub(2000);
    auto mts2 = new MemoryTestStub(2000);
    QVERIFY(MemoryTestStubResource::numAllocations == 2);
    QVERIFY(Memory::used() == 0);

    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);
    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);

    delete mts;
    delete mts2;

    QVERIFY(MemoryTestStubResource::numAllocations == 2);
    QVERIFY(Memory::used() == 4000);

    Memory::clear();
}

void MemoryTest::testStorageExceeded(){
    QVERIFY(Memory::used() == 0);

    auto mts = new MemoryTestStub(2000);
    auto mts2 = new MemoryTestStub(2000);
    auto mts3 = new MemoryTestStub(2000);
    QVERIFY(MemoryTestStubResource::numAllocations == 3);
    QVERIFY(Memory::used() == 0);

    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);
    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);
    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);

    delete mts;
    delete mts2;
    delete mts3;

    QVERIFY(MemoryTestStubResource::numAllocations == 1);
    QVERIFY(Memory::used() == 2000);

    Memory::clear();
}

void MemoryTest::benchmarkSimpleAllocation(){
    size_t singleAllocationSize = 1000000;
    QBENCHMARK{
        for ( int i = 0; i < 1000000; ++i ){
            auto mts2 = new MemoryTestStubResource(singleAllocationSize);
            delete mts2;
        }
    }
}

void MemoryTest::benchmarkMemoryAllocation(){
    size_t singleAllocationSize = 1000000;
    QBENCHMARK{
        for ( int i = 0; i < 1000000; ++i ){
            auto mts2 = new MemoryTestStub(singleAllocationSize);
            delete mts2;
        }
    }
}

void MemoryTest::benchmarkMemoryRecycling(){
    size_t singleAllocationSize = 1000000;

    Memory::setSize(singleAllocationSize * 2);

    auto mts = new MemoryTestStub(singleAllocationSize);
    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);
    Memory::reserve<MemoryTestStub, MemoryTestStubResource>(mts);
    delete mts;

    QBENCHMARK{
        for ( int i = 0; i < 1000000; ++i ){
            auto mts2 = new MemoryTestStub(singleAllocationSize);
            delete mts2;
        }
    }
}
