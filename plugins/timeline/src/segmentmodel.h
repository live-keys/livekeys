#ifndef LVSEGMENTMODEL_H
#define LVSEGMENTMODEL_H

#include <QObject>

#include "qabstractrangemodel.h"

namespace lv{

class Segment;

// SegmentModelIterator
// --------------------

/// \private
class SegmentModelIterator : public QAbstractRangeModelIterator{

public:
    SegmentModelIterator(
            QList<Segment*>::iterator it,
            QList<Segment*>::iterator endIt,
            qint64 endPosition)
        : m_endIt(endIt)
        , m_currentIt(it)
        , m_endPosition(endPosition)
    {}

    bool isEnd() const;
    void nextItem();

    QVariant data(int role = 0);

private:
    QList<Segment*>::iterator m_endIt;
    QList<Segment*>::iterator m_currentIt;
    qint64 m_endPosition;

};

// SegmentModel
// ------------

class SegmentModelPrivate;

/// \private
class SegmentModel : public QAbstractRangeModel{

    Q_OBJECT
    Q_PROPERTY(double snapThreshold READ snapThreshold WRITE setSnapThreshold NOTIFY snapThrehsoldChanged)

public:
    typedef QVariant (*ItemDataFactoryFunction)();

    enum Role{
        SegmentRole = QAbstractRangeModel::LengthRole + 1
    };

public:
    explicit SegmentModel(QObject *parent = nullptr);
    ~SegmentModel();

    QAbstractRangeModelIterator* dataBetween(qint64 startPosition, qint64 endPosition);

    void setItemPosition(qint64 itemPosition, qint64 itemLength, int positionIndex, qint64 itemNewPosition);
    void setItemLength(qint64 itemPosition, qint64 itemLength, int positionIndex, qint64 newLength);
    void setItemData(qint64 itemPosition, qint64 itemLength, int positionIndex, int role, const QVariant& value);

    QHash<int, QByteArray> roleNames() const;


    int totalSegments() const;
    Segment* segmentAt(int index);
    void clearSegments();

    QPair<int, int> indexesBetween(qint64 location1, qint64 location2);
    Segment* segmentThatWraps(qint64 position);

    double snapThreshold() const;

    void addSegment(qint64 position, qint64 length);
    void removeItem(qint64 position, qint64 length, qint64 relativeIndex);
    bool addSegment(lv::Segment* segment);
    lv::Segment *takeSegment(lv::Segment* segment);

public slots:
    qint64 availableSpace(qint64 position);
    void setSnapThreshold(double snapThreshold);

signals:
    void snapThrehsoldChanged();

private:
    int insertItemImpl(Segment* item);

    SegmentModel(const SegmentModel&);
    SegmentModel& operator = (const SegmentModel&);

    SegmentModelPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(SegmentModel)
};

}// namespace

#endif // LVSEGMENTMODEL_H
