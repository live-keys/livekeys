#ifndef LVTIMELINEHEADERMODEL_H
#define LVTIMELINEHEADERMODEL_H

#include "qabstractrangemodel.h"
#include <functional>

namespace lv{

/// \private
class TimelineHeaderModel : public QAbstractRangeModel{

    Q_OBJECT
    Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)

public:
    class Iterator : public QAbstractRangeModelIterator{
    public:
        Iterator(
            double fps,
            double scale,
            qint64 from,
            qint64 to);

        bool isEnd() const;
        void nextItem();
        QVariant data(int role = 0);

    private:
        qint64 m_markDivision;
        qint64 m_labelDivision;
        qint64 m_current;
        double m_scale;
        qint64 m_from;
        qint64 m_to;
    };

public:
    enum Role{
        HasLabel = QAbstractRangeModel::LengthRole + 1,
        Label,
        IsDelimiter
    };

    static const std::vector<std::pair<
        double,
        std::vector<std::pair<double, std::function<std::pair<int, int>(double, double)> > >
    > >& scaleConfiguration();
    static std::pair<int, int> findScaleConfiguration(double fps, double scale);

public:
    TimelineHeaderModel(QObject* parent = nullptr);
    ~TimelineHeaderModel() override;

    QAbstractRangeModelIterator* dataBetween(qint64 startPosition, qint64 endPosition) override;

    void setItemPosition(qint64, qint64, int, qint64) override{}
    void setItemLength(qint64, qint64, int, qint64) override{}
    void setItemData(qint64, qint64, int, int, const QVariant&) override{}

    virtual QHash<int, QByteArray> roleNames() const override;
    double scale() const;

public slots:
    void setScale(double arg);

signals:
    void scaleChanged();

private:
    double m_scale;
};

inline double TimelineHeaderModel::scale() const{
    return m_scale;
}

}// namespace

#endif // LVTIMELINEHEADERMODEL_H
