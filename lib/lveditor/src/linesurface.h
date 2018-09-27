#ifndef LINESURFACE_H
#define LINESURFACE_H

#include <QtGui/qtextoption.h>
#include "live/lveditorglobal.h"
#include "textedit_p.h"
#include "qquickpainteditem.h"

class QTextBlock;

namespace lv {

class LineSurfacePrivate;

class LV_EDITOR_EXPORT LineSurface : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
public:
    LineSurface(QQuickItem *parent=nullptr);
    QFont font() const;
    void setFont(const QFont &font);
    QColor color() const;
    void setColor(const QColor &c);
    Q_INVOKABLE void setComponents(lv::TextEdit* te);

Q_SIGNALS:
    void colorChanged(const QColor &color);
    void fontChanged(const QFont &font);
public Q_SLOTS:
    void setDirtyBlockPosition(int pos);
    void textDocumentFinished();
private Q_SLOTS:
    void updateSize();
    void triggerPreprocess();

private:
    void showHideLines(bool show, int pos, int num);
    void replaceTextInBlock(int blockNumber, std::string s);

    void linesAdded();
    void linesRemoved();
    void updateLineDocument();
    void changeLastCharInBlock(int blockNumber, char c);

    void collapseLines(int pos, int num);
    void expandLines(int pos, int num);
    void expandCollapseSkeleton(int pos, int num, QString &replacement, bool show);

    static inline int numberOfDigits(int i) {
        int res = 0;
        if (i < 10) return 2;
        while (i > 0)
        {
            res++;
            i/=10;
        }
        return res;
    }

protected:
    LineSurface(LineSurfacePrivate &dd, QQuickItem *parent = nullptr);


    // mouse filter?
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) Q_DECL_OVERRIDE;

    friend class TextUtil;
    friend class LineManager;
private:
    Q_DISABLE_COPY(LineSurface)
    Q_DECLARE_PRIVATE(LineSurface)
};

}

QML_DECLARE_TYPE(lv::LineSurface)


#endif // LINESURFACE_H
