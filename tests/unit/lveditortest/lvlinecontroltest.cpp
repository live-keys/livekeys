#include "lvlinecontroltest.h"
#include <QDebug>
#include <QQmlEngine>

Q_TEST_RUNNER_REGISTER(LvLineControlTest);

using namespace lv;

LvLineControlTest::LvLineControlTest(QObject *parent) : QObject(parent) {}

void LvLineControlTest::initTestCase()
{
    m_lineControl = new LineControl;
    m_blockHeight = 15;
    m_lineControl->setBlockHeight(m_blockHeight);
}


void LvLineControlTest::addingSingleCollapse()
{
    m_lineControl->addCollapse(10, 3);
    QCOMPARE(m_lineControl->sections().size(), 1);
    auto section = m_lineControl->sections()[0];
    QCOMPARE(section.type, LineControl::LineSection::Collapsed);
    QCOMPARE(section.range, 4);
    QCOMPARE(section.visibleRange, 1);
    QCOMPARE(section.position, 10);
    QCOMPARE(section.visiblePosition, 10);
}

void LvLineControlTest::addingSinglePalette()
{
    QQuickItem* item = new QQuickItem;
    item->setHeight(55);
    m_lineControl->addPalette(3, 2, item, 25, 45);
    QCOMPARE(m_lineControl->sections().size(),2);
    auto section1 = m_lineControl->sections()[0];
    auto section2 = m_lineControl->sections()[1];
    // confirm sortedness
    QCOMPARE(section1.type, LineControl::LineSection::Palette);
    QCOMPARE(section2.type, LineControl::LineSection::Collapsed);
    QCOMPARE(section1.range, 2);
    QCOMPARE(section1.visibleRange, 5);
    // check offset of section2
    QCOMPARE(section2.visiblePosition, 13);
}

void LvLineControlTest::closeCollapsed()
{
    m_lineControl->removeCollapse(10);
    QCOMPARE(m_lineControl->sections().size(), 1);
    auto section = m_lineControl->sections()[0];
    QCOMPARE(section.type, LineControl::LineSection::Palette);
}

void LvLineControlTest::addACollapseAfterPalette()
{
    // to be used for the next test!
    m_lineControl->addCollapse(100, 5);
    QCOMPARE(m_lineControl->sections().size(), 2);
    QCOMPARE(m_lineControl->sections()[1].type, LineControl::LineSection::Collapsed);

    QCOMPARE(m_lineControl->sections()[0].type, LineControl::LineSection::Palette);
    QCOMPARE(m_lineControl->sections()[1].visiblePosition, 103);
}

void LvLineControlTest::resizePalette()
{
    m_lineControl->sections()[0].palette->setHeight(100);
    m_lineControl->resizePalette(m_lineControl->sections()[0].palette);
    QCOMPARE(m_lineControl->sections()[0].visibleRange, 8);
    QCOMPARE(m_lineControl->sections()[1].position, 100);
    QCOMPARE(m_lineControl->sections()[1].visiblePosition, 106);
}

void LvLineControlTest::removePalette()
{
    auto ptr = m_lineControl->sections()[0].palette;
    m_lineControl->removePalette(m_lineControl->sections()[0].palette);
    delete ptr;
    QCOMPARE(m_lineControl->sections().size(), 1);
    QCOMPARE(m_lineControl->sections()[0].type, LineControl::LineSection::Collapsed);
    QCOMPARE(m_lineControl->sections()[0].visiblePosition, 100);
    QCOMPARE(m_lineControl->sections()[0].position, 100);
}

void LvLineControlTest::addLines()
{
    QQuickItem* item = new QQuickItem;
    item->setHeight(70);
    m_lineControl->addPalette(50, 2, item, 324, 400);


    m_lineControl->setDirtyPos(60);
    m_lineControl->deltaLines(5);
    auto section1 = m_lineControl->sections()[0];
    auto section2 = m_lineControl->sections()[1];

    QCOMPARE(section1.position, 50);
    QCOMPARE(section2.position, 105); // only the collapse shifted
}

void LvLineControlTest::removeLines()
{
    m_lineControl->setDirtyPos(40);
    m_lineControl->deltaLines(-3);
    auto section1 = m_lineControl->sections()[0];
    auto section2 = m_lineControl->sections()[1];

    QCOMPARE(section1.position, 47);
    QCOMPARE(section2.position, 102); // both shifted back

    QCOMPARE(section1.type, LineControl::LineSection::Palette);

    clearLineControl();
}

void LvLineControlTest::checkVisibleWithNoSections()
{
    auto result = m_lineControl->visibleSections(10, 20);

    QCOMPARE(result.size(), 1);
    QCOMPARE(result[0].start, 10);
    QCOMPARE(result[0].size, 11);
    QCOMPARE(result[0].palette, nullptr);

    m_lineControl->addCollapse(10, 4);
}

void LvLineControlTest::checkBeforeCollapsedSection()
{
    auto result = m_lineControl->visibleSections(5,7);

    QCOMPARE(result.size(), 1);
    QCOMPARE(result[0].start, 5);
    QCOMPARE(result[0].size, 3);
    QCOMPARE(result[0].palette, nullptr);

}

void LvLineControlTest::checkWithCollapsedSection()
{
    auto result = m_lineControl->visibleSections(9,19);
    QCOMPARE(result.size(), 3);



}





void LvLineControlTest::cleanupTestCase()
{
    clearLineControl();
    delete m_lineControl;
}

void LvLineControlTest::clearLineControl()
{
    m_lineControl->reset();
}
