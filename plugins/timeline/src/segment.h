#ifndef LVSEGMENT_H
#define LVSEGMENT_H

#include <QObject>
#include <QColor>
#include "lvtimelineglobal.h"

class QQmlEngine;

namespace lv{

class Track;
class SegmentModel;
class MLNode;

class LV_TIMELINE_EXPORT Segment : public QObject{

    Q_OBJECT
    Q_PROPERTY(unsigned int position        READ position       WRITE setPosition        NOTIFY positionChanged)
    Q_PROPERTY(unsigned int length          READ length         WRITE setLength          NOTIFY lengthChanged)
    Q_PROPERTY(unsigned int maxStretchLeft  READ maxStretchLeft WRITE setMaxStretchLeft  NOTIFY maxStretchLeftChanged)
    Q_PROPERTY(unsigned int maxStretchRight READ maxStrechRight WRITE setMaxStretchRight NOTIFY maxStretchRightChanged)
    Q_PROPERTY(QString label                READ label          WRITE setLabel           NOTIFY labelChanged)
    Q_PROPERTY(QColor color                 READ color          WRITE setColor           NOTIFY colorChanged)

    friend class SegmentModel;

public:
    explicit Segment(QObject *parent = nullptr);
    Segment(unsigned int position, unsigned int length, QObject* parent = nullptr);
    virtual ~Segment();

    unsigned int position() const;
    unsigned int length() const;

    void setPosition(unsigned int arg);
    void setLength(unsigned int arg);

    virtual void assignTrack(Track* track);
    virtual void cursorEnter(qint64 position);
    virtual void cursorExit(qint64 position);
    virtual void cursorNext(qint64 position);
    virtual void cursorMove(qint64 position);
    virtual void cursorPass(qint64 position);
    virtual void serialize(QQmlEngine* engine, MLNode& node) const;
    virtual void deserialize(Track *track, QQmlEngine* engine, const MLNode& data);

    bool contains(qint64 position);

    bool isProcessing() const;

    const QString& label() const;
    void setLabel(const QString& label);

    const QColor& color() const;
    void setColor(const QColor& color);

    void setMaxStretchLeft(unsigned int maxStretchLeft);
    void setMaxStretchRight(unsigned int maxStretchRight);
    unsigned int maxStretchLeft() const;
    unsigned int maxStrechRight() const;

    Track* currentTrack() const;

public slots:
    void remove();
    lv::SegmentModel* segmentModel();

    virtual void stretchLeftTo(unsigned int position);
    virtual void stretchRightTo(unsigned int position);

signals:
    void positionChanged();
    void lengthChanged();
    void labelChanged();
    void colorChanged();
    void maxStretchLeftChanged();
    void maxStretchRightChanged();

protected:
    void setIsProcessing(bool isProcessing);

private:
    static void setPosition(Segment* segment, unsigned int position);
    static void setLength(Segment* segment, unsigned int length);

    Track*       m_track;

    unsigned int m_position;
    unsigned int m_length;
    bool         m_isProcessing;
    QString      m_label;
    QColor       m_color;
    unsigned int m_maxStretchLeft;
    unsigned int m_maxStretchRight;
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

inline unsigned int Segment::maxStretchLeft() const{
    return m_maxStretchLeft;
}

inline unsigned int Segment::maxStrechRight() const{
    return m_maxStretchRight;
}

inline void Segment::setMaxStretchLeft(unsigned int maxStretchLeft){
    if (m_maxStretchLeft == maxStretchLeft)
        return;

    m_maxStretchLeft = maxStretchLeft;
    emit maxStretchLeftChanged();
}

inline void Segment::setMaxStretchRight(unsigned int maxStretchRight){
    if (m_maxStretchRight == maxStretchRight)
        return;

    m_maxStretchRight = maxStretchRight;
    emit maxStretchRightChanged();
}

}// namespace

#endif // LVSEGMENT_H
