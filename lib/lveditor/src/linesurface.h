#ifndef LINESURFACE_H
#define LINESURFACE_H


#include "private/qquickimplicitsizeitem_p.h"
#include <QtGui/qtextoption.h>
#include "live/lveditorglobal.h"
#include "textedit_p.h"
#include "qquickpainteditem.h"

class QTextBlock;

namespace lv {

class LineSurfacePrivate;

class LV_EDITOR_EXPORT LineSurface : public QQuickImplicitSizeItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
public:
    LineSurface(QQuickImplicitSizeItem *parent=nullptr);
    QFont font() const;
    void setFont(const QFont &font);
    QColor color() const;
    void setColor(const QColor &c);
    void componentComplete() Q_DECL_OVERRIDE;
    Q_INVOKABLE void setComponents(lv::TextEdit* te);

Q_SIGNALS:

    void textChanged();
    void contentSizeChanged();
    void colorChanged(const QColor &color);
    void selectedTextColorChanged(const QColor &color);
    void fontChanged(const QFont &font);
    void wrapModeChanged();
    void lineCountChanged();
    void readOnlyChanged(bool isReadOnly);
    void linkActivated(const QString &link);
    Q_REVISION(2) void linkHovered(const QString &link);
    void effectiveHorizontalAlignmentChanged();
    void textDocumentChanged();
    Q_REVISION(6) void editingFinished();
    void dirtyBlockPosition(int pos);
    void stateChangeSignal(int blockNum);
    void textDocumentFinishedUpdating();

public Q_SLOTS:
    void setDirtyBlockPosition(int pos);
    void singleShotUpdate();
    void insert(int position, const QString &text);
    void remove(int start, int end);
    Q_REVISION(2) void append(const QString &text);
    Q_REVISION(7) void clear();
    void textDocumentFinished();


private Q_SLOTS:
    void q_textChanged();
    void q_contentsChange(int, int, int);
    void updateWholeDocument();
    void invalidateBlock(const QTextBlock &block);
    void q_updateAlignment();
    void updateSize();
    void triggerPreprocess();
    void highlightingDone(const QRectF &);

private:
    void markDirtyNodesForRange(int start, int end, int charDelta);
    void updateTotalLines();
    void invalidateFontCaches();
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
    LineSurface(LineSurfacePrivate &dd, QQuickImplicitSizeItem *parent = nullptr);

    void geometryChanged(const QRectF &newGeometry,
                         const QRectF &oldGeometry) Q_DECL_OVERRIDE;

    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;

    // mouse filter?
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) Q_DECL_OVERRIDE;
    void updatePolish() Q_DECL_OVERRIDE;

    friend class TextUtil;
    friend class LineManager;
private:
    Q_DISABLE_COPY(LineSurface)
    Q_DECLARE_PRIVATE(LineSurface)
};

}

QML_DECLARE_TYPE(lv::LineSurface)


#endif // LINESURFACE_H
