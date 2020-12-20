#ifndef LVSVGVIEW_H
#define LVSVGVIEW_H

#include <QQuickPaintedItem>
#include <QSvgRenderer>

namespace lv{

class SvgView : public QQuickPaintedItem{

    Q_OBJECT
    Q_PROPERTY(QString path    READ path    WRITE setPath    NOTIFY pathChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)

public:
    SvgView();
    ~SvgView() override;

    void paint(QPainter *painter) override;

    const QString& path() const;
    const QString& content() const;

    void setPath(const QString& path);
    void setContent(const QString& content);

    void loadInRenderer();

protected:
    void componentComplete() override;

signals:
    void pathChanged();
    void contentChanged(QString content);

private:
    QRect        m_rendererViewPort;
    QSvgRenderer m_renderer;
    QString      m_path;
    QString      m_content;
};

inline const QString &SvgView::path() const{
    return m_path;
}

inline const QString &SvgView::content() const{
    return m_content;
}

inline void SvgView::setPath(const QString &path){
    if (m_path == path)
        return;

    m_path = path;
    emit pathChanged();
    if ( isComponentComplete() ){
        loadInRenderer();
        update();
    }
}

inline void SvgView::setContent(const QString& content){
    if (m_content == content)
        return;

    m_content = content;
    emit contentChanged(m_content);
    if ( isComponentComplete() ){
        loadInRenderer();
        update();
    }
}

}// namespace

#endif // LVSVGVIEW_H
