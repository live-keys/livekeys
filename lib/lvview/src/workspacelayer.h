#ifndef WORKSPACELAYER_H
#define WORKSPACELAYER_H

#include <QObject>
#include "lvviewglobal.h"
#include "live/layer.h"

namespace lv{

class LV_VIEW_EXPORT WorkspaceLayer : public Layer{

    Q_OBJECT

public:
    explicit WorkspaceLayer(QObject *parent = nullptr);
    ~WorkspaceLayer();

    void loadView(ViewEngine *engine, QObject *parent) Q_DECL_OVERRIDE;
    QObject* nextViewParent() Q_DECL_OVERRIDE;

public slots:

private:
    QObject* m_nextViewParent;
};

}// namespace

#endif // WORKSPACELAYER_H
