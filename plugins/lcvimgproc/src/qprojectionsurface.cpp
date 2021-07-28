#include "qprojectionsurface.h"
#include "qmat.h"
#include "qprojectionmapper.h"

QProjectionSurface::QProjectionSurface(QObject *parent)
    : QObject(parent)
    , m_destination(QJSValue::NullValue)
    , m_input(nullptr)
{
}

QProjectionSurface::~QProjectionSurface()
{
}

QMat *QProjectionSurface::input() const
{
    return m_input;
}

void QProjectionSurface::setInput(QMat *input)
{
    if (m_input == input)
        return;
    m_input = input;
    emit inputChanged();

}

QProjectionMapper *QProjectionSurface::mapper() const
{
    return qobject_cast<QProjectionMapper*>(parent());
}

QJSValue QProjectionSurface::destination() const
{
    return m_destination;
}

void QProjectionSurface::setDestination(const QJSValue &destination)
{
    if (m_destination.equals(destination))
        return;
    m_destination = destination;
    emit destinationChanged();
}
