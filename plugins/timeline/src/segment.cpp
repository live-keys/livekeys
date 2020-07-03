#include "segment.h"
#include "segmentmodel.h"
#include "track.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QJSValue>
#include <limits>


namespace lv{

Segment::Segment(QObject *parent)
    : QObject(parent)
    , m_position(0)
    , m_length(0)
    , m_maxStretchLeft(std::numeric_limits<unsigned int>::max())
    , m_maxStretchRight(std::numeric_limits<unsigned int>::max())
{
}

Segment::Segment(unsigned int position, unsigned int length, QObject* parent)
    : QObject(parent)
    , m_position(position)
    , m_length(length)
    , m_maxStretchLeft(std::numeric_limits<unsigned int>::max())
    , m_maxStretchRight(std::numeric_limits<unsigned int>::max())
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

void Segment::assignTrack(Track*){}
void Segment::cursorEnter(qint64){}
void Segment::cursorNext(qint64){}
void Segment::cursorMove(qint64){}
void Segment::cursorPass(qint64){}

void Segment::serialize(QQmlEngine *, MLNode &node) const{
    node = MLNode(MLNode::Object);
    node["type"] = "Segment";
    node["position"] = static_cast<int>(m_position);
    node["length"] = static_cast<int>(m_length);

    if ( m_maxStretchLeft != std::numeric_limits<unsigned int>::max() )
        node["maxStretchLeft"] = static_cast<int>(m_maxStretchLeft);
    if ( m_maxStretchRight != std::numeric_limits<unsigned int>::max() )
        node["maxStretchRight"] = static_cast<int>(m_maxStretchRight);
    if ( !m_label.isEmpty() )
        node["label"] = m_label.toStdString();
    if ( m_color.isValid() )
        node["color"] = m_color.name().toStdString();
}

void Segment::deserialize(Track*, QQmlEngine *, const MLNode &node){
    setPosition(static_cast<unsigned int>(node["position"].asInt()));
    setLength(static_cast<unsigned int>(node["length"].asInt()));

    if ( node.hasKey("maxStretchLeft") )
        setMaxStretchLeft(static_cast<unsigned int>(node["maxStretchLeft"].asInt()));
    if ( node.hasKey("maxStretchRight") )
        setMaxStretchRight(static_cast<unsigned int>(node["maxStretchRight"].asInt()));
    if ( node.hasKey("label") )
        setLabel(QString::fromStdString(node["label"].asString()));
    if ( node.hasKey("color") )
        setColor(QColor(QString::fromStdString(node["color"].asString())));
}

void Segment::cursorExit(qint64){}

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

void Segment::stretchLeftTo(unsigned int position){
    unsigned int currentPosition = m_position;
    setPosition(position);
    setLength(currentPosition - m_position + m_length);
}

void Segment::stretchRightTo(unsigned int position){
    setLength(position - m_position);
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
