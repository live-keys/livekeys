#ifndef LVSEGMENT_H
#define LVSEGMENT_H

#include <QObject>
#include <QColor>

namespace lv{

class SegmentModel;

class Segment : public QObject{

    Q_OBJECT
    Q_PROPERTY(unsigned int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(unsigned int length   READ length   WRITE setLength   NOTIFY lengthChanged)
    Q_PROPERTY(QString label         READ label    WRITE setLabel    NOTIFY labelChanged)
    Q_PROPERTY(QColor color          READ color    WRITE setColor    NOTIFY colorChanged)

    friend class SegmentModel;

public:
    explicit Segment(QObject *parent = nullptr);
    Segment(unsigned int position, unsigned int length, QObject* parent = nullptr);
    virtual ~Segment();

    unsigned int position() const;
    unsigned int length() const;

    void setPosition(unsigned int arg);
    void setLength(unsigned int arg);

    virtual void cursorEnter(qint64 position);
    virtual void cursorExit();
    virtual void cursorNext(qint64 position);
    virtual void cursorMove(qint64 position);

    bool contains(qint64 position);

    bool isAsync() const;

    const QString& label() const;
    void setLabel(const QString& label);

    const QColor& color() const;
    void setColor(const QColor& color);

public slots:
    void remove();
    lv::SegmentModel* segmentModel();

signals:
    void positionChanged();
    void lengthChanged();
    void labelChanged();
    void colorChanged();

protected:
    void setIsAsync(bool isAsync);

private:
    static void setPosition(Segment* segment, unsigned int position);
    static void setLength(Segment* segment, unsigned int length);

    unsigned int m_position;
    unsigned int m_length;
    bool         m_isAsync;
    QString      m_label;
    QColor       m_color;
};

inline unsigned int Segment::position() const{
    return m_position;
}

inline unsigned int Segment::length() const{
    return m_length;
}

inline const QString &Segment::label() const{
    return m_label;
}

inline const QColor &Segment::color() const{
    return m_color;
}

inline void Segment::setLabel(const QString &label){
    if (m_label == label)
        return;

    m_label = label;
    emit labelChanged();
}

inline void Segment::setColor(const QColor &color){
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged();
}

}// namespace

#endif // LVSEGMENT_H
