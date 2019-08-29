#ifndef LVEDITORLAYER_H
#define LVEDITORLAYER_H

#include <QObject>
#include "lveditorglobal.h"
#include "live/layer.h"

namespace lv{

class LV_EDITOR_EXPORT EditorLayer : public Layer{

    Q_OBJECT
    Q_PROPERTY(QObject* environment READ environment WRITE setEnvironment NOTIFY environmentChanged)

public:
    explicit EditorLayer(QObject *parent = nullptr);
    ~EditorLayer();

    QObject* environment() const;
    void setEnvironment(QObject* environment);

signals:
    void environmentChanged();

private:
    QObject* m_environment;
};

inline QObject *EditorLayer::environment() const{
    return m_environment;
}

}// namespace

#endif // LVEDITORLAYER_H
