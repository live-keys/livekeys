#ifndef QPROJECTIONMAPPER_H
#define QPROJECTIONMAPPER_H

#include <QObject>
#include <QJSValue>
#include "qprojectionsurface.h"
#include <QQmlListProperty>

class QMat;

class QProjectionMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMat*                    backgroundImage     READ backgroundImage    WRITE   setBackgroundImage      NOTIFY backgroundImageChanged)
    Q_PROPERTY(QMat*                    result              READ result             NOTIFY  resultChanged)
    Q_PROPERTY(QMat*                    liveResult          READ liveResult         NOTIFY  liveResultChanged)
    Q_PROPERTY(QJSValue                 model               READ model              NOTIFY  modelChanged)
    Q_PROPERTY(QProjectionSurface*      focusSurface        READ focusSurface       NOTIFY focusSurfaceChanged)
    
    Q_PROPERTY(QQmlListProperty<QObject> surfaces READ surfaces)
    Q_CLASSINFO("DefaultProperty", "surfaces")
public:
    explicit QProjectionMapper(QObject *parent = nullptr);
    ~QProjectionMapper();

    QMat *backgroundImage() const;
    void setBackgroundImage(QMat *backgroundImage);

    void removeSurface(QProjectionSurface* surface);
    void addSurface(QProjectionSurface* surface);
    int numOfSurfaces();
    QProjectionSurface* surfaceAt(int idx);
    QMat *result() const;


    QMat *liveResult() const;

    QJSValue model() const;
    QQmlListProperty<QObject> surfaces();

    QProjectionSurface *focusSurface() const;

public slots:
    void modifyLiveResult(QJSValue points);
    void createResult();
    QProjectionSurface* selectFocusSurface(int idx);

signals:
    void backgroundImageChanged();
    void resultChanged();
    void liveResultChanged();
    void modelChanged();
    void focusSurfaceChanged();
private:
    void updateModel();
    static void surfacesAppend(QQmlListProperty<QObject> *p, QObject *v);
    static int surfacesCount(QQmlListProperty<QObject> *p);
    static QObject* surfacesAt(QQmlListProperty<QObject> *p, int idx);
    static void surfacesClear(QQmlListProperty<QObject> *p);
    QMat* applySurfaces(bool skipFocused = false, int idx = -1);

    std::vector<QProjectionSurface*> m_surfaces;

    QList<QProjectionSurface*> m_surfacess;
    QProjectionSurface* m_focusSurface;
    QMat* m_backgroundImage;
    QMat* m_result;
    QMat* m_liveResult;
    QMat* m_basis;
    QJSValue m_model;

};

#endif // QPROJECTIONMAPPER_H
