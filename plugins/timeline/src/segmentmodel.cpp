#include "segmentmodel.h"
#include "segment.h"

#include "live/visuallogqt.h"

#include <QVariant>

namespace lv{

// SegmentModelIterator
// ----------------------------------------------------------------------------

bool SegmentModelIterator::isEnd() const{
    if ( m_currentIt == m_endIt )
        return true;
    if ( (*m_currentIt)->position() >= m_endPosition )
        return true;
    return false;
}

void SegmentModelIterator::nextItem(){
    if ( isEnd() )
        return;
    ++m_currentIt;
}

QVariant SegmentModelIterator::data(int role){
    if ( role == SegmentModel::PositionRole )
        return (*m_currentIt)->position();
    else if ( role == SegmentModel::LengthRole )
        return (*m_currentIt)->length();
    else if ( role == SegmentModel::SegmentRole )
        return QVariant::fromValue(*m_currentIt);
    return QVariant();
}


// SegmentModelPrivate
// ----------------------------------------------------------------------------

class SegmentModelPrivate{
public:
    SegmentModelPrivate() : lastChange(nullptr), itemDataFactory(nullptr), snapThreshold(2){}
    ~SegmentModelPrivate();

    class SegmentModelChange{
    public:
        qint64 startPosition;
        qint64 endPosition;
    };

    QList<Segment*> items;
    SegmentModelChange* lastChange;
    SegmentModelChange* change(qint64 startPosition, qint64 endPosition);

    SegmentModel::ItemDataFactoryFunction itemDataFactory;

    int searchFirstIndex(qint64 position);
    int searchFirstIndex(qint64 position, qint64 length);

    double snapThreshold;
};

SegmentModelPrivate::~SegmentModelPrivate(){
    for (QList<Segment*>::iterator it = items.begin(); it != items.end(); ++it ){
        delete *it;
    }
    items.clear();
}

SegmentModelPrivate::SegmentModelChange* SegmentModelPrivate::change(qint64 startPosition, qint64 endPosition){
    SegmentModelChange* change = new SegmentModelChange;
    change->startPosition = startPosition;
    change->endPosition   = endPosition;
    return change;
}

int SegmentModelPrivate::searchFirstIndex(qint64 position){
    if ( items.size() == 0 )
        return 0;
    if ( items.last()->position() < position )
        items.size();

    int first  = 0, last = items.size() - 1, middle = (first + last) / 2;

    while( first <= last ){
        if ( items[middle]->position() < position ){
            first = middle + 1;
        } else {
            last = middle - 1;
        }

        middle = (first + last) / 2;
    }

    if ( items[middle]->position() < position )
        return middle + 1;
    else
        return middle;
}

int SegmentModelPrivate::searchFirstIndex(qint64 position, qint64 length){
    int index = searchFirstIndex(position);
    while ( index < items.size() ){
        if ( items[index]->position() != position )
            return items.size();
        if ( items[index]->length() == length )
            return index;
        ++index;
    }
    return items.size();
}

// SegmentModel
// ----------------------------------------------------------------------------

SegmentModel::SegmentModel(QObject *parent)
    : QAbstractRangeModel(parent)
    , d_ptr(new SegmentModelPrivate)
{
}

SegmentModel::~SegmentModel(){
    delete d_ptr;
}

QAbstractRangeModelIterator* SegmentModel::dataBetween(qint64 startPosition, qint64 endPosition){
    Q_D(SegmentModel);

    int startIndex = d->searchFirstIndex(startPosition);
    while ( startIndex > 0 ){
        --startIndex;
        if ( d->items[startIndex]->position() + d->items[startIndex]->length() < startPosition ){
            ++startIndex;
            break;
        }
    }
    if ( startIndex < d->items.size() ){
        return new SegmentModelIterator(d->items.begin() + startIndex, d->items.end(), endPosition);
    }
    return new SegmentModelIterator(d->items.end(), d->items.end(), endPosition);
}

void SegmentModel::setItemPosition(qint64 itemPosition, qint64 itemLength, int indexOffset, qint64 itemNewPosition){
    Q_D(SegmentModel);

    int index = d->searchFirstIndex(itemPosition, itemLength) + indexOffset;
    if ( index >= d->items.size() )
        return;

    Segment* item = d->items[index];
    if ( item->position() != itemPosition )
        return;

    d->items.takeAt(index);
    Segment::setPosition(item, static_cast<unsigned int>(itemNewPosition));
    int newIndex = insertItemImpl(item);
    if ( newIndex == -1 ){
        Segment::setPosition(item, static_cast<unsigned int>(itemPosition));
        insertItemImpl(item);
        return;
    }

    QList<Segment*>::iterator itemIt = d->items.begin() + newIndex;
    SegmentModelIterator* modelIt = new SegmentModelIterator(itemIt, itemIt + 1, item->position() + 1);
    emit itemCoordinatesChanged(itemPosition, itemLength, indexOffset, modelIt, 0);
}

void SegmentModel::setItemLength(qint64 itemPosition, qint64 itemLength, int indexOffset, qint64 newLength){
    Q_D(SegmentModel);
    if ( itemLength == newLength )
        return;

    int positionIndex = d->searchFirstIndex(itemPosition);
    int index = positionIndex;
    while ( index < d->items.size() ){
        if ( d->items[index]->length() == itemLength )
            break;
        ++index;
    }

    index += indexOffset;
    if ( index >= d->items.size() )
        return;

    Segment* item = d->items[index];
    if ( item->position() != itemPosition )
        return;

    int nextIndex = index + 1;
    if ( index + 1 < d->items.size() ){
        Segment* nextSegm = d->items[nextIndex];
        if ( item->position() + newLength > nextSegm->position() ){
            qint64 diff = item->position() + item->length() - nextSegm->position();
            if ( diff <= d->snapThreshold ){
                newLength = nextSegm->position() - item->position();
            } else {
                return;
            }
        }
    }

    Segment::setLength(item, static_cast<unsigned int>(newLength));

    QList<Segment*>::iterator itemIt = d->items.begin() + index;
    SegmentModelIterator* modelIt = new SegmentModelIterator(itemIt, itemIt + 1, itemPosition + 1);
    emit itemCoordinatesChanged(itemPosition, itemLength, indexOffset, modelIt, indexOffset);
}

void SegmentModel::setItemData(
        qint64,
        qint64,
        int,
        int,
        const QVariant&)
{
}

QHash<int, QByteArray> SegmentModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[SegmentModel::SegmentRole] = "segment";
    return roles;
}

bool SegmentModel::addSegment(Segment *segment){
    if ( !segment )
        return -1;

    beginDataChange(segment->position(), segment->position() + 1);
    int index = insertItemImpl(segment);
    endDataChange();

    return index != -1;
}

int SegmentModel::insertItemImpl(Segment* item){
    Q_D(SegmentModel);
    int index = d->searchFirstIndex(item->position());

    while ( index < d->items.size() ){
        if ( d->items[index]->position() == item->position() && d->items[index]->length() < item->length() )
            ++index;
        else {
            break;
        }
    }

    int prevIndex = index - 1;
    int nextIndex = index;
    if ( prevIndex >= 0 ){ // check snap before
        Segment* prevSegm = d->items[prevIndex];
        if ( prevSegm->position() + prevSegm->length() > item->position() ){
            qint64 diff = prevSegm->position() + prevSegm->length() - item->position();
            if ( diff <= d->snapThreshold ){
                if ( prevSegm->position() + prevSegm->length() + item->length() > contentWidth() ){
                    return -1;
                }

                Segment::setPosition(item, prevSegm->position() + prevSegm->length());
                item->setPosition(prevSegm->position() + prevSegm->length());

                if ( nextIndex < d->items.size() ){
                    Segment* nextSegm = d->items[nextIndex];
                    if ( item->position() + item->length() > nextSegm->position() )
                        return -1;
                }
            } else {
                return -1;
            }
        }
    }
    if ( nextIndex < d->items.size() ){ // check snap before
        Segment* nextSegm = d->items[nextIndex];
        if ( item->position() + item->length() > nextSegm->position() ){
            qint64 diff = item->position() + item->length() - nextSegm->position();
            if ( diff <= d->snapThreshold ){
                if ( nextSegm->position() < item->length() ){
                    return -1;
                }

                Segment::setPosition(item, nextSegm->position() - item->length());
                if ( prevIndex >= 0 ){
                    Segment* prevSegm = d->items[prevIndex];
                    if ( prevSegm->position() + prevSegm->length() > item->position() )
                        return -1;
                }
            } else {
                return -1;
            }
        }
    }

    d->items.insert(index, item);
    item->setParent(this);
    return index;
}

int SegmentModel::totalSegments() const{
    Q_D(const SegmentModel);
    return d->items.size();
}

Segment *SegmentModel::segmentAt(int index){
    Q_D(SegmentModel);
    return d->items.at(index);
}

void SegmentModel::clearSegments(){
    Q_D(SegmentModel);
    beginDataChange(0, contentWidth());
    for ( Segment* s : d->items ){
        s->deleteLater();
    }
    d->items.clear();
    endDataChange();
}

Segment *SegmentModel::segmentThatWraps(qint64 position){
    Q_D(SegmentModel);
    int index = d->searchFirstIndex(position);
    while ( index > 0 ){
        --index;
        if ( d->items[index]->position() + d->items[index]->length() < position ){
            ++index;
            break;
        }
    }

    if ( index < d->items.size() ){
        Segment* segm = d->items[index];
        if ( segm->contains(position) )
            return segm;
    }

    return nullptr;
}

Segment* SegmentModel::takeSegment(Segment *segment){
    Q_D(SegmentModel);
    Segment* result = nullptr;
    int index = d->searchFirstIndex(segment->position());
    if ( index < d->items.size() ){
        if ( d->items[index] == segment ){
            beginDataChange(segment->position(), segment->position() + 1);
            d->items.removeAt(index);
            segment->setParent(nullptr);
            result = segment;
            endDataChange();
        }
    }
    return result;
}

void SegmentModel::addSegment(qint64 position, qint64 length){
    Segment* item = new Segment(static_cast<unsigned int>(position), static_cast<unsigned int>(length));
    addSegment(item);
}

void SegmentModel::removeItem(qint64 position, qint64 length, qint64 relativeIndex){
    Q_D(SegmentModel);
    int index = d->searchFirstIndex(position);
    while ( index < d->items.size() ){
        if ( d->items[index]->position() != position )
            return;

        if ( d->items[index]->length() == length && index + relativeIndex < d->items.size() ){
            index += relativeIndex;
            if ( d->items[index]->position() == position && d->items[index]->length() == length ){
                beginDataChange(position, position + 1);
                delete d->items[index];
                d->items.removeAt(index);
                endDataChange();
                return;
            }
        }
        ++index;
    }
}

qint64 SegmentModel::availableSpace(qint64 position){
    Q_D(SegmentModel);
    int index = d->searchFirstIndex(position);
    if ( index > 0 ){
        if ( d->items[index - 1]->position() + d->items[index - 1]->length() > position ){
            return 0;
        }
    }

    while ( index < d->items.size() ){
        if ( d->items[index]->position() > position ){
            return d->items[index]->position() - position;
        }
        ++index;
    }
    return contentWidth() - position;
}

double SegmentModel::snapThreshold() const{
    Q_D(const SegmentModel);
    return d->snapThreshold;
}


void SegmentModel::setSnapThreshold(double snapThreshold){
    Q_D(SegmentModel);
    if (qFuzzyCompare(d->snapThreshold, snapThreshold))
        return;

    d->snapThreshold = snapThreshold;
    emit snapThrehsoldChanged();
}


}// namespace
