#ifndef LVLINECONTROLTEST_H
#define LVLINECONTROLTEST_H

#include <QObject>
#include "testrunner.h"
#include "live/linecontrol.h"

class LvLineControlTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit LvLineControlTest(QObject* parent = nullptr);
    ~LvLineControlTest() {}
private slots:
    void initTestCase();
    void addingSingleCollapse();
    void addingSinglePalette();
    void closeCollapsed();
    void addACollapseAfterPalette();
    void resizePalette();
    void removePalette();
    void addLines();
    void removeLines();

    void checkVisibleWithNoSections();

    void checkBeforeCollapsedSection();
    void checkWithCollapsedSection();
    void checkAfterCollapsedSection();

    void checkDocumentSmallerThanViewport();

    void checkBeforePalette();
    void checkWithPalette();
    void checkWithPalettePartially1();
    void checkWithPalettePartially2();
    void checkAfterPalette();

    void checkWithFragmentStart();
    void checkFragmentStartAndBigViewport();
    void checkFragmentStartWithCollapsed();

    void checkWithFragmentEnd();
    void checkFragmentEndAndBigViewport();
    void checkFragmentEndWithPalette();

    void checkWithTwoCollapses();
    void checkSecondCollapse();
    void checkWithCollapsesAndFragment();

    void checkWithTwoNeighboringPaletes();

    void cleanupTestCase();

public slots:
private:
    void clearLineControl();

    lv::LineControl* m_lineControl;
    int m_blockHeight;
    QQuickItem *palettePtr1, *palettePtr2;
};

#endif // LVLINECONTROLTEST_H
