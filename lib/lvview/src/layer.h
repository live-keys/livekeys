#ifndef LVLAYER_H
#define LVLAYER_H

#include <QObject>
#include "lvviewglobal.h"

namespace lv{

class ViewEngine;
class LV_VIEW_EXPORT Layer : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit Layer(QObject *parent = nullptr);
    virtual ~Layer();

    Layer* parentLayer();

    bool hasView() const;

    const QString& name() const;
    void setName(const QString &name);

    virtual void loadView(ViewEngine *, QObject*){}
    virtual QObject* nextViewParent(){ return nullptr;}

protected:
    void setHasView(bool flag);

signals:
    void nameChanged(QString name);
    void viewReady(Layer* layer, QObject* view);

public slots:

private:
    QString m_name;
    bool    m_hasView;

};

inline void Layer::setHasView(bool flag){
    m_hasView = flag;
}

inline void Layer::setName(const QString& name){
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

inline bool Layer::hasView() const{
    return m_hasView;
}

inline const QString &Layer::name() const{
    return m_name;
}

}// namespace

#endif // LVLAYER_H
