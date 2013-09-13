#ifndef QMATEMPTY_HPP
#define QMATEMPTY_HPP

#include "QMatDisplay.hpp"

class QMatEmpty : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QSize      matSize  READ matSize  WRITE setMatSize  NOTIFY matSizeChanged)
    Q_PROPERTY(QColor     color    READ color    WRITE setColor    NOTIFY colorChanged)
    Q_PROPERTY(int        channels READ channels WRITE setChannels NOTIFY channelsChanged)
    Q_PROPERTY(QMat::Type type     READ type     WRITE setType     NOTIFY typeChanged)

public:
    explicit QMatEmpty(QQuickItem *parent = 0);
    ~QMatEmpty();

    const QSize& matSize() const;
    void setMatSize(const QSize& size);

    const QColor& color() const;
    void setColor(const QColor& color);

    int channels() const;
    void setChannels(int channels);

    QMat::Type type() const;
    void setType(QMat::Type type);

signals:
    void matSizeChanged();
    void colorChanged();
    void channelsChanged();
    void typeChanged();

private:
    QSize      m_matSize;
    QColor     m_color;
    int        m_channels;
    QMat::Type m_type;
    cv::Scalar m_colorScalar;

};

inline const QSize &QMatEmpty::matSize() const{
    return m_matSize;
}

inline void QMatEmpty::setMatSize(const QSize &size){
    if ( m_matSize != size ){
        m_matSize = size;
        emit matSizeChanged();
        output()->data()->create(cv::Size(m_matSize.width(), m_matSize.height()), CV_MAKETYPE(m_type, m_channels));
        output()->data()->setTo(m_colorScalar);
        emit outChanged();
        update();
    }
}

inline const QColor &QMatEmpty::color() const{
    return m_color;
}

inline void QMatEmpty::setColor(const QColor &color){
    if ( m_color != color ){
        m_color = color;

        qDebug() << m_color;
        qDebug() << m_channels;

        switch ( m_channels ){
        case 1 : m_colorScalar = cv::Scalar(color.red());break;
        case 2 : m_colorScalar = cv::Scalar(color.red(), color.green()); break;
        case 3 : m_colorScalar = cv::Scalar(color.red(), color.green(), color.blue()); break;
        case 4 : m_colorScalar = cv::Scalar(color.red(), color.green(), color.blue(), color.alpha()); break;
        }

        emit colorChanged();
        output()->data()->setTo(m_colorScalar);
        emit outChanged();
        update();
    }
}

inline int QMatEmpty::channels() const{
    return m_channels;
}

inline void QMatEmpty::setChannels(int channels){
    if ( m_channels != channels ){
        m_channels = channels;
        emit channelsChanged();
        output()->data()->create(cv::Size(m_matSize.width(), m_matSize.height()), CV_MAKETYPE(m_type, m_channels));
        output()->data()->setTo(m_colorScalar);
        emit outChanged();
        update();
    }
}

inline QMat::Type QMatEmpty::type() const{
    return m_type;
}

inline void QMatEmpty::setType(QMat::Type type){
    if ( m_type == type ){
        m_type = type;
        emit typeChanged();
        output()->data()->create(cv::Size(m_matSize.width(), m_matSize.height()), CV_MAKETYPE(m_type, m_channels));
        output()->data()->setTo(m_colorScalar);
        emit outChanged();
        update();
    }
}

#endif // QMATEMPTY_HPP
