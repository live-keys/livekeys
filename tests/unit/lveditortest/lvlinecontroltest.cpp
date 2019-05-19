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

    auto ptr = section1.palette;
    clearLineControl();
    delete ptr;
}


void LvLineControlTest::checkVisibleWithNoSections()
{
    m_lineControl->deltaLines(30);

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

    QCOMPARE(result[0].start, 9);
    QCOMPARE(result[0].size, 1);
    QCOMPARE(result[0].palette, nullptr);

    QCOMPARE(result[1].start, 10);
    QCOMPARE(result[1].size, 1);
    QCOMPARE(result[1].palette, nullptr);

    QCOMPARE(result[2].start, 15);
    QCOMPARE(result[2].size, 9);
    QCOMPARE(result[2].palette, nullptr);
}


void LvLineControlTest::checkAfterCollapsedSection() {

    auto result = m_lineControl->visibleSections(23,28);
    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 27);
    QCOMPARE(result[0].size, 3); // not 6, since there's 30 blocks only!
    QCOMPARE(result[0].palette, nullptr);

    clearLineControl();
}

void LvLineControlTest::checkDocumentSmallerThanViewport()
{
    m_lineControl->deltaLines(5);
    auto result = m_lineControl->visibleSections(0, 10);

    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 0);
    QCOMPARE(result[0].size, 5);
    QCOMPARE(result[0].palette, nullptr);
}

void LvLineControlTest::checkBeforePalette()
{
    m_lineControl->deltaLines(25);

    palettePtr1 = new QQuickItem;
    palettePtr1->setHeight(45);
    m_lineControl->addPalette(10, 6, palettePtr1, 225, 425);

    auto result = m_lineControl->visibleSections(5,8);

    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 5);
    QCOMPARE(result[0].size, 4);
    QCOMPARE(result[0].palette, nullptr);
}

void LvLineControlTest::checkWithPalette()
{
    auto result = m_lineControl->visibleSections(8,20);

    QCOMPARE(result.size(), 3);

    QCOMPARE(result[0].start, 8);
    QCOMPARE(result[0].size, 2);
    QCOMPARE(result[0].palette, nullptr);

    QCOMPARE(result[1].start, 0); // we don't care about positioning this, it's already been taken care of!
    QCOMPARE(result[1].size, 4);
    QCOMPARE(result[1].palette, palettePtr1);

    QCOMPARE(result[2].start, 16);
    QCOMPARE(result[2].size, 7);
    QCOMPARE(result[2].palette, nullptr);
}

void LvLineControlTest::checkWithPalettePartially1()
{
    auto result = m_lineControl->visibleSections(8, 10);

    QCOMPARE(result.size(), 2);

    QCOMPARE(result[0].start, 8);
    QCOMPARE(result[0].size, 2);
    QCOMPARE(result[0].palette, nullptr);

    QCOMPARE(result[1].start, 0);
    QCOMPARE(result[1].size, 1);
    QCOMPARE(result[1].palette, palettePtr1);
}

void LvLineControlTest::checkWithPalettePartially2()
{
    auto result = m_lineControl->visibleSections(12, 24);

    QCOMPARE(result.size(), 2);

    QCOMPARE(result[0].start, 0);
    QCOMPARE(result[0].size, 2);
    QCOMPARE(result[0].palette, palettePtr1);

    QCOMPARE(result[1].start, 16);
    QCOMPARE(result[1].size, 11);
    QCOMPARE(result[1].palette, nullptr);
}

void LvLineControlTest::checkAfterPalette()
{
    auto result = m_lineControl->visibleSections(15, 20);

    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 17);
    QCOMPARE(result[0].size, 6);
    QCOMPARE(result[0].palette, nullptr);

    clearLineControl();
    delete palettePtr1;
}

void LvLineControlTest::checkWithFragmentStart()
{
    m_lineControl->deltaLines(30);

    palettePtr1 = new QQuickItem;
    palettePtr1->setHeight(0); // only fragments have 0 height
    m_lineControl->addPalette(0, 10, palettePtr1, 0, 500);

    auto result = m_lineControl->visibleSections(0, 5);

    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 10);
    QCOMPARE(result[0].size, 6);
    QCOMPARE(result[0].palette, nullptr);
}

void LvLineControlTest::checkFragmentStartAndBigViewport()
{
    auto result = m_lineControl->visibleSections(0, 25);

    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 10);
    QCOMPARE(result[0].size, 20);
    QCOMPARE(result[0].palette, nullptr);
}

void LvLineControlTest::checkFragmentStartWithCollapsed()
{
    m_lineControl->addCollapse(15, 4);

    auto result = m_lineControl->visibleSections(3, 15);

    QCOMPARE(result.size(), 3);

    QCOMPARE(result[0].start, 13);
    QCOMPARE(result[0].size, 2);
    QCOMPARE(result[0].palette, nullptr);

    QCOMPARE(result[1].start, 15);
    QCOMPARE(result[1].size, 1);
    QCOMPARE(result[1].palette, nullptr);

    QCOMPARE(result[2].start, 20);
    QCOMPARE(result[2].size, 10);
    QCOMPARE(result[2].palette, nullptr);

    clearLineControl();
    delete palettePtr1;
}

void LvLineControlTest::checkWithFragmentEnd()
{
    m_lineControl->deltaLines(30);

    palettePtr1 = new QQuickItem;
    palettePtr1->setHeight(0);
    m_lineControl->addPalette(20, 10, palettePtr1, 1000, 1300); // fragmentEnd

    auto result = m_lineControl->visibleSections(10, 25);

    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 10);
    QCOMPARE(result[0].size, 10);
    QCOMPARE(result[0].palette, nullptr);
}

void LvLineControlTest::checkFragmentEndAndBigViewport()
{
    auto result = m_lineControl->visibleSections(0, 40);

    QCOMPARE(result.size(), 1);

    QCOMPARE(result[0].start, 0);
    QCOMPARE(result[0].size, 20);
    QCOMPARE(result[0].palette, nullptr);
}

void LvLineControlTest::checkFragmentEndWithPalette()
{
    QQuickItem* item = new QQuickItem;
    item->setHeight(85);
    m_lineControl->addPalette(10, 5, item, 500, 800);

    auto result = m_lineControl->visibleSections(5, 25);

    QCOMPARE(result.size(), 3);

    QCOMPARE(result[0].start, 5);
    QCOMPARE(result[0].size, 5);
    QCOMPARE(result[0].palette, nullptr);

    QCOMPARE(result[1].start, 0);
    QCOMPARE(result[1].size, 7);
    QCOMPARE(result[1].palette, item);

    QCOMPARE(result[2].start, 15);
    QCOMPARE(result[2].size, 5);
    QCOMPARE(result[2].palette, nullptr);

    clearLineControl();
    delete item;

}

void LvLineControlTest::checkWithTwoCollapses()
{
    m_lineControl->deltaLines(30);

    m_lineControl->addCollapse(10, 3);
    m_lineControl->addCollapse(20, 5);

    auto result = m_lineControl->visibleSections(5, 25);

     QCOMPARE(result.size(), 5);

     QCOMPARE(result[0].start, 5);
     QCOMPARE(result[0].size, 5);

     QCOMPARE(result[1].start, 10);
     QCOMPARE(result[1].size, 1);

     QCOMPARE(result[2].start, 14);
     QCOMPARE(result[2].size, 6);

     QCOMPARE(result[3].start, 20);
     QCOMPARE(result[3].size, 1);

     QCOMPARE(result[4].start, 26);
     QCOMPARE(result[4].size, 4);
}

void LvLineControlTest::checkSecondCollapse()
{
    auto result = m_lineControl->visibleSections(14, 22);

    QCOMPARE(result.size(), 3);

    QCOMPARE(result[0].start, 17);
    QCOMPARE(result[0].size, 3);

    QCOMPARE(result[1].start, 20);
    QCOMPARE(result[1].size, 1);

    QCOMPARE(result[2].start, 26);
    QCOMPARE(result[2].size, 4);
}

void LvLineControlTest::checkWithCollapsesAndFragment()
{
    palettePtr1 = new QQuickItem;
    palettePtr1->setHeight(0);
    m_lineControl->addPalette(0, 5, palettePtr1, 0, 200);

    palettePtr2 = new QQuickItem;
    palettePtr1->setHeight(0);
    m_lineControl->addPalette(27, 3, palettePtr2, 1000,1100);

    auto result = m_lineControl->visibleSections(0, 25);

    QCOMPARE(result.size(), 5);

    QCOMPARE(result[0].start, 5);
    QCOMPARE(result[0].size, 5);

    QCOMPARE(result[1].start, 10);
    QCOMPARE(result[1].size, 1);

    QCOMPARE(result[2].start, 14);
    QCOMPARE(result[2].size, 6);

    QCOMPARE(result[3].start, 20);
    QCOMPARE(result[3].size, 1);

    QCOMPARE(result[4].start, 26);
    QCOMPARE(result[4].size, 1);

    clearLineControl();
    delete palettePtr1;
    delete palettePtr2;
}

void LvLineControlTest::checkWithTwoNeighboringPaletes()
{
    m_lineControl->deltaLines(30);

    palettePtr1 = new QQuickItem;
    palettePtr1->setHeight(45);
    m_lineControl->addPalette(10, 8, palettePtr1, 300, 450);

    palettePtr2 = new QQuickItem;
    palettePtr2->setHeight(76);
    m_lineControl->addPalette(18, 3, palettePtr2, 1000,1100);

    auto result = m_lineControl->visibleSections(5, 25);


    QCOMPARE(result.size(), 4);

    QCOMPARE(result[0].start, 5);
    QCOMPARE(result[0].size, 5);
    QCOMPARE(result[0].palette, nullptr);

    QCOMPARE(result[1].start, 0);
    QCOMPARE(result[1].size, 4);
    QCOMPARE(result[1].palette, palettePtr1);


    QCOMPARE(result[2].start, 0);
    QCOMPARE(result[2].size, 6);
    QCOMPARE(result[2].palette, palettePtr2);

    QCOMPARE(result[3].start, 21);
    QCOMPARE(result[3].size, 6);
    QCOMPARE(result[3].palette, nullptr);

    clearLineControl();

    delete palettePtr1;
    delete palettePtr2;
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
