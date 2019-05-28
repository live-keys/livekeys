#ifndef LVWINDOWLAYER_H
#define LVWINDOWLAYER_H

#include <QObject>
#include "lvviewglobal.h"
#include "live/layer.h"

class QQuickWindow;

namespace lv{

class LV_VIEW_EXPORT WindowLayer : public Layer{

    Q_OBJECT
    Q_PROPERTY(QObject* dialogs READ dialogs NOTIFY dialogsChanged)
    Q_PROPERTY(QObject* handle  READ handle  NOTIFY handleChanged)

public:
    explicit WindowLayer(QObject *parent = nullptr);
    ~WindowLayer();

    void loadView(ViewEngine *engine, QObject* parent) Q_DECL_OVERRIDE;
    QObject* nextViewParent() Q_DECL_OVERRIDE;

    QObject* handle() const;
    QObject* dialogs() const;

public slots:
    void windowActiveChanged();

signals:
    void handleChanged();
    void dialogsChanged();
    void isActiveChanged(bool isActive);

private:
    QQuickWindow* m_window;
    QObject* m_nextViewParent;

    QObject* m_handle;
    QObject* m_dialogs;
};

inline QObject *WindowLayer::handle() const{
    return m_handle;
}

inline QObject *WindowLayer::dialogs() const{
    return m_dialogs;
}

}// namespace

#endif // LVWINDOWLAYER_H
