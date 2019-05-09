#ifndef LVWINDOWLAYER_H
#define LVWINDOWLAYER_H

#include <QObject>
#include "lvviewglobal.h"
#include "live/layer.h"

namespace lv{

class LV_VIEW_EXPORT WindowLayer : public Layer{

    Q_OBJECT

public:
    explicit WindowLayer(QObject *parent = nullptr);
    ~WindowLayer();

    void loadView(ViewEngine *engine, QObject* parent) Q_DECL_OVERRIDE;
    QObject* nextViewParent() Q_DECL_OVERRIDE;

public slots:

private:
    QObject* m_nextViewParent;

};

}// namespace

#endif // LVWINDOWLAYER_H
