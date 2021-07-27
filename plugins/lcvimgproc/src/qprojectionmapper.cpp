#include "qprojectionmapper.h"
#include "qgeometry.h"
#include "qprojectionsurface.h"
#include "qqmlengine.h"
#include "qqmlcontext.h"
#include "qqml.h"
#include "live/exception.h"
#include "live/qmlerror.h"

QProjectionMapper::QProjectionMapper(QObject *parent)
    : QObject(parent)
    , m_focusSurface(nullptr)
    , m_backgroundImage(nullptr)
    , m_result(nullptr)
    , m_liveResult(nullptr)
    , m_basis(nullptr)
{

}

QProjectionMapper::~QProjectionMapper()
{

}

QMat *QProjectionMapper::backgroundImage() const
{
    return m_backgroundImage;
}

void QProjectionMapper::setBackgroundImage(QMat *backgroundImage)
{
    if (m_backgroundImage == backgroundImage)
        return;
    m_backgroundImage = backgroundImage;
    emit backgroundImageChanged();

}

void QProjectionMapper::removeSurface(QProjectionSurface *surface)
{
    unsigned idx = 0;
    for (; idx < m_surfaces.size(); ++idx)
        if (m_surfaces[idx] == surface) break;

    if (idx == m_surfaces.size()) return;

    m_surfaces.erase(m_surfaces.begin() + idx);

    updateModel();

}

void QProjectionMapper::addSurface(QProjectionSurface *surface)
{
    unsigned idx = 0;
    for (; idx < m_surfaces.size(); ++idx)
        if (m_surfaces[idx] == surface) return;

    surface->setParent(this);
    m_surfaces.push_back(surface);

    updateModel();
}

int QProjectionMapper::numOfSurfaces()
{
    return m_surfaces.size();
}

QProjectionSurface *QProjectionMapper::surfaceAt(int idx)
{
    if (idx < 0 || idx >= m_surfaces.size()) return nullptr;
    return m_surfaces[idx];
}

void QProjectionMapper::createResult()
{
    m_result = applySurfaces();
    emit resultChanged();

    m_focusSurface = nullptr;
    emit focusSurfaceChanged();
    delete m_basis;
}

void QProjectionMapper::updateModel()
{
    if (m_surfaces.size() == 0){
        m_model = QJSValue();
        return;
    }


    auto engine = qmlEngine(this);
    if (!engine) return;
    QJSValue result = engine->newArray(m_surfaces.size());
    for ( int i = 0; i < m_surfaces.size(); ++i ){
        auto id = qmlContext(m_surfaces[i])->nameForObject(m_surfaces[i]);
        QJSValue surf = engine->newObject();
        surf.setProperty("surface", engine->newQObject(m_surfaces[i]));
        surf.setProperty("id", id);
        result.setProperty(i, surf);
    }
    m_model = result;
    emit modelChanged();
}

void QProjectionMapper::surfacesAppend(QQmlListProperty<QObject> *list, QObject *v)
{
    QProjectionMapper* that = reinterpret_cast<QProjectionMapper*>(list->data);

    QProjectionSurface* surface = qobject_cast<QProjectionSurface*>(v);
    if (!surface){
        THROW_QMLERROR(lv::Exception, "ProjectionMapper: Trying to append a child that's not a ProjectionSurface.", lv::Exception::toCode("~Surface"), that);
        return;
    }

    that->addSurface(surface);
}

int QProjectionMapper::surfacesCount(QQmlListProperty<QObject> *list)
{
    QProjectionMapper* that = reinterpret_cast<QProjectionMapper*>(list->data);
    return that->numOfSurfaces();
}

QObject* QProjectionMapper::surfacesAt(QQmlListProperty<QObject> *list, int idx)
{
    QProjectionMapper* that = reinterpret_cast<QProjectionMapper*>(list->data);
    return that->surfaceAt(idx);
}

void QProjectionMapper::surfacesClear(QQmlListProperty<QObject> *list)
{
    QProjectionMapper* that = reinterpret_cast<QProjectionMapper*>(list->data);

    while (that->numOfSurfaces() > 0)
        that->removeSurface(that->surfaceAt(0));
}

QMat *QProjectionMapper::applySurfaces(bool skipFocused, int idx)
{
    if (!m_backgroundImage) return nullptr;
    QGeometry g;
    QMat* temp = m_backgroundImage->clone();
    for (unsigned i = 0; i < m_surfaces.size(); ++i){
        if (skipFocused && i == idx) continue;
        if (m_surfaces[i]->destination().isNull()) continue;
        QMat* wrap = g.perspectiveProjection(m_surfaces[i]->input(), temp, m_surfaces[i]->destination());
        delete temp;
        temp = wrap;
    }
    return temp;
}

QProjectionSurface *QProjectionMapper::focusSurface() const
{
    return m_focusSurface;
}


QQmlListProperty<QObject> QProjectionMapper::surfaces()
{
    return QQmlListProperty<QObject>(
        this,
        this,
        &QProjectionMapper::surfacesAppend,
        &QProjectionMapper::surfacesCount,
        &QProjectionMapper::surfacesAt,
        &QProjectionMapper::surfacesClear
    );
}

QMat *QProjectionMapper::liveResult() const
{
    return m_liveResult;
}

QJSValue QProjectionMapper::model() const
{
    return m_model;
}

void QProjectionMapper::modifyLiveResult(QJSValue points)
{
    if (!m_focusSurface || !m_focusSurface->input() || !m_basis) return;
    QGeometry g;
    m_liveResult = g.perspectiveProjection(m_focusSurface->input(), m_basis, points);
    emit liveResultChanged();
}

QMat *QProjectionMapper::result() const
{
    return m_result;
}

QProjectionSurface*  QProjectionMapper::selectFocusSurface(int idx)
{
    if (m_surfaces.empty() || idx >= m_surfaces.size()) return nullptr;
    m_focusSurface = m_surfaces[idx];
    emit focusSurfaceChanged();

    m_basis = applySurfaces(true, idx);
    m_result = m_basis;
    emit resultChanged();

    return m_focusSurface;


}

