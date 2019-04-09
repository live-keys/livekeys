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
    void cleanupTestCase();

public slots:
private:
    lv::LineControl* m_lineControl;
    int m_blockHeight;
};

#endif // LVLINECONTROLTEST_H
