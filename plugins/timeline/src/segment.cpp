#include "segment.h"
#include "segmentmodel.h"
#include "track.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QtDebug>

#include "live/visuallogqt.h"

namespace lv{

Segment::Segment(QObject *parent)
    : QObject(parent)
    , m_position(0)
    , m_length(0)
{
}

Segment::Segment(unsigned int position, unsigned int length, QObject* parent)
    : QObject(parent)
    , m_position(position)
    , m_length(length)
{
}

Segment::~Segment(){
}

void Segment::setPosition(unsigned int arg){
    SegmentModel* model = qobject_cast<SegmentModel*>(parent());
    if (model){
        model->setItemPosition(m_position, m_length, 0, arg);
    } else {
        setPosition(this, arg);
    }
}


void Segment::setLength(unsigned int arg){
    SegmentModel* model = qobject_cast<SegmentModel*>(parent());
    if (model){
        model->setItemLength(m_position, m_length, 0, arg);
    } else {
        setLength(this, arg);
    }
}

void Segment::cursorEnter(qint64){}
void Segment::cursorNext(qint64){}
void Segment::cursorMove(qint64){}
void Segment::cursorExit(){}

bool Segment::contains(qint64 position){
    return (position >= m_position && position < m_position + m_length);
}

bool Segment::isAsync() const{
    return m_isAsync;
}

void Segment::remove(){
    segmentModel()->removeItem(position(), length(), 0);
}

SegmentModel* Segment::segmentModel(){
    return qobject_cast<SegmentModel*>(parent());
}

void Segment::setIsAsync(bool isAsync){
    m_isAsync = isAsync;
}

void Segment::setPosition(Segment *segment, unsigned int position){
    if (segment->m_position == position)
        return;

    segment->m_position = position;
    emit segment->positionChanged();
}

void Segment::setLength(Segment *segment, unsigned int length){
    if (segment->m_length == length)
        return;

    segment->m_length = length;
    emit segment->lengthChanged();
}

}// namespace
