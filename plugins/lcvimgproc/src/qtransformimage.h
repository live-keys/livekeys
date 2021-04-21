#ifndef QTRANSFORMIMAGE_H
#define QTRANSFORMIMAGE_H

#include <QObject>
#include <QJSValue>
#include <QQmlListProperty>

class QTransformImage : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject* input                  READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(QJSValue result                 READ result NOTIFY resultChanged)
    Q_PROPERTY(QQmlListProperty<QObject> items READ items CONSTANT)
    Q_CLASSINFO("DefaultProperty", "items")

public:
    explicit QTransformImage(QObject *parent = nullptr);

    QObject* input() const;
    void setInput(QObject* input);
    QQmlListProperty<QObject> items();

    static void appendObjectToList(QQmlListProperty<QObject>*, QObject*);
    static int objectCount(QQmlListProperty<QObject>*);
    static QObject* objectAt(QQmlListProperty<QObject>*, int);
    static void clearObjects(QQmlListProperty<QObject>*);

    QJSValue result() const;

    void run();

signals:
    void inputChanged();
    void resultChanged();

public slots:
    void exec();
    void __childDestroyed();
    void __childResultChanged();

private:
    QObject*        m_input;
    QList<QObject*> m_children;
    QJSValue        m_result;
};

inline QObject *QTransformImage::input() const{
    return m_input;
}

inline void QTransformImage::exec(){
    run();
}

#endif // QTRANSFORMIMAGE_H
