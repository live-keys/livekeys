#ifndef QMLDECLARATIONOBJECT_H
#define QMLDECLARATIONOBJECT_H

#include <QObject>
#include "live/qmldeclaration.h"
#include "live/qmlmetaextension.h"
#include "live/visuallog.h"

namespace lv{

class QmlDeclarationObject : public QObject, public QmlExtensionObjectI{

    Q_OBJECT
    META_EXTENSION(QmlDeclarationObject, nullptr);

public:
    class LogBehavior{
    public:
        static std::string typeId(const QmlDeclarationObject&){
            return "editqml@QmlDeclaration";
        }
        static std::string defaultViewDelegate(const QmlDeclarationObject&){
            return "";
        }
        static bool hasViewObject(){
            return false;
        }
        static bool hasTransport(){
            return false;
        }
        static lv::VisualLog::ViewObject* toView(const QmlDeclarationObject&){
            return nullptr;
        }
        static bool toTransport(const QmlDeclarationObject&, lv::MLNode&){
            return false;
        }
        static void toStream(lv::VisualLog& vl, const QmlDeclarationObject& d){
            vl << *d.internal();
        }
    };

public:
    explicit QmlDeclarationObject(const QmlDeclaration::Ptr& decl, QObject* parent = nullptr);
    ~QmlDeclarationObject();

    QmlDeclaration::Ptr internal() const;

    static void log(ViewEngine* ve, QObject* o, VisualLog& vl);

public slots:
    int position() const;
    int valuePosition() const;
    int valueLength() const;
    int length() const;

    bool hasObject() const;

    QString type() const;
    QString parentType() const;

    QString location() const;

signals:

private:
    Q_DISABLE_COPY(QmlDeclarationObject);

    QmlDeclaration::Ptr m_internal;
};

inline QmlDeclaration::Ptr QmlDeclarationObject::internal() const{
    return m_internal;
}

inline int QmlDeclarationObject::position() const{
    return m_internal->position();
}

inline int QmlDeclarationObject::valuePosition() const{
    return m_internal->valuePosition();
}

inline int QmlDeclarationObject::valueLength() const{
    return m_internal->valueLength();
}

inline int QmlDeclarationObject::length() const{
    return m_internal->length();
}

inline bool QmlDeclarationObject::hasObject() const{
    return m_internal->isForObject();
}

inline QString QmlDeclarationObject::type() const{
    return m_internal->type().join();
}

inline QString QmlDeclarationObject::parentType() const{
    return m_internal->parentType().join();
}


inline lv::VisualLog& operator << (lv::VisualLog& vl, const QmlDeclarationObject& v){
    return vl.behavior<QmlDeclarationObject::LogBehavior>(v);
}

}// namespace

#endif // QMLDECLARATIONOBJECT_H
