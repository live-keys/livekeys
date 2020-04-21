#include "timelineheadermodel.h"
#include "timeline.h"
#include "live/visuallogqt.h"

#include <QVariant>

namespace lv{

// TimelineHeaderModel::Iterator
// ----------------------------------------------------------------------------

TimelineHeaderModel::Iterator::Iterator(
        double fps,
        double scale,
        qint64 from,
        qint64 to)
    : m_scale(scale)
    , m_from(from)
    , m_to(to)
{
    auto scaleConfig = findScaleConfiguration(fps, scale);
    m_markDivision = scaleConfig.first;
    m_labelDivision = scaleConfig.second;
    m_current = m_from - (m_from % m_markDivision);
}

bool TimelineHeaderModel::Iterator::isEnd() const{
    return m_current > m_to;
}

void TimelineHeaderModel::Iterator::nextItem(){
    m_current += m_markDivision;
}

QVariant TimelineHeaderModel::Iterator::data(int role){
    if ( role == TimelineHeaderModel::PositionRole)
        return m_current;
    else if (role == TimelineHeaderModel::LengthRole)
        return QVariant(1);
    else if (role == TimelineHeaderModel::HasLabel)
        return QVariant(m_current % m_labelDivision == 0);
    else if (role == TimelineHeaderModel::Label)
        return QVariant(m_current / m_scale);
    else if (role == TimelineHeaderModel::IsDelimiter)
        return QVariant(m_current == 0);
    return QVariant();
}

// TimelineHeaderModel
// ----------------------------------------------------------------------------

const std::vector<std::pair<
    double,
    std::vector<std::pair<double, std::function<std::pair<int, int> (double, double)> > >
> >
&TimelineHeaderModel::scaleConfiguration(){

    using P = std::pair<int, int>;

    static std::vector<std::pair<
            double,
            std::vector<std::pair<double, std::function<std::pair<int, int> (double, double)> > >
    > > scales = {
        {30, {
            {1, [](double fps, double){ return P(fps / 2, fps * 2);}},
            {5, [](double fps, double){ return P(5 * 5, 5 * fps);}},
            {10, [](double fps, double){ return P(2 * 5, 5 * fps);}}
        }}
    };

    return scales;
}

std::pair<int, int> TimelineHeaderModel::findScaleConfiguration(double fps, double scale){
    // find closest fps
    auto it = scaleConfiguration().begin();
    while ( it != scaleConfiguration().end() ){
        auto peekIt = it;
        if ( it->first >= fps )
            break;
        ++peekIt;
        if ( peekIt == scaleConfiguration().end() )
            break;
        ++it;
    }

    if ( it == scaleConfiguration().end() )
        return std::make_pair(1, 1);

    auto scaleIt = it->second.begin();
    while ( scaleIt != it->second.end() ){
        auto peekIt = scaleIt;
        if ( scaleIt->first >= scale )
            break;
        ++peekIt;
        if ( peekIt == it->second.end() )
            break;
        ++scaleIt;
    }

    if ( scaleIt == it->second.end() )
        return std::make_pair(1, 1);

    return scaleIt->second(it->first, scaleIt->first);
};

TimelineHeaderModel::TimelineHeaderModel(QObject *parent)
    : QAbstractRangeModel(parent)
    , m_scale(5)
{
}

TimelineHeaderModel::~TimelineHeaderModel(){
}

QAbstractRangeModelIterator *TimelineHeaderModel::dataBetween(qint64 startPosition, qint64 endPosition){
    Timeline* tline = qobject_cast<Timeline*>(parent());
    return new TimelineHeaderModel::Iterator(
        tline->fps(),
        m_scale,
        startPosition,
        endPosition
    );
}

QHash<int, QByteArray> TimelineHeaderModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[TimelineHeaderModel::HasLabel]    = "hasLabel";
    roles[TimelineHeaderModel::Label]       = "label";
    roles[TimelineHeaderModel::IsDelimiter] = "isDelimiter";
    return roles;
}

void TimelineHeaderModel::setScale(double arg){
    if (qFuzzyCompare(m_scale, arg))
        return;

    beginDataChange(0, contentWidth());

    m_scale = arg;
    emit scaleChanged();

    endDataChange();
}

}// namespace
