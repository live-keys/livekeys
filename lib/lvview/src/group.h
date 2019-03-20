#ifndef LVGROUP_H
#define LVGROUP_H

#include "live/lvviewglobal.h"
#include "live/visuallogqt.h"

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlEngine>

namespace lv{

class MLNode;
class ViewEngine;

class LV_VIEW_EXPORT Group : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit Group(QObject *parent = nullptr);
    virtual ~Group();

    void classBegin(){}
    void componentComplete();

    static QByteArray typeDefaultValue(const QByteArray& typeString);
    static lv::Group* createWithProperties(QQmlEngine* engine, const std::map<QByteArray, QByteArray>& properties);

    static void serialize(lv::ViewEngine* engine, const QObject* o, MLNode &node);
    static QObject *deserialize(lv::ViewEngine* engine, const MLNode &node);


signals:
    void complete();

public slots:

private:
    bool m_isComponentComplete;

};

inline VisualLog& operator << (VisualLog& vl, const Group& v){
    vl.asObject("Group", v);

    if ( v.metaObject()->propertyCount() < 5 ){
        vl << "Group{";
        for ( int i = 0; i < v.metaObject()->propertyCount(); ++i ){
            vl << " " << v.metaObject()->property(i).name() << ":" << v.metaObject()->property(i).read(&v).toString() << ";";
        }
        vl << "}";
    } else {
        vl << "Group{\n";
        for ( int i = 0; i < v.metaObject()->propertyCount(); ++i ){
            vl << "  " << v.metaObject()->property(i).name() << ":" << v.metaObject()->property(i).read(&v).toString() << "\n";
        }
        vl << "}\n";
    }
    return vl;
}

}// namespace

#endif // GROUP_H
