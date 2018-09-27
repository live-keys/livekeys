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

    enum HAlignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter,
        AlignJustify = Qt::AlignJustify
    };
    Q_ENUM(HAlignment)

    enum VAlignment {
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignVCenter = Qt::AlignVCenter
    };
    Q_ENUM(VAlignment)

    enum WrapMode { NoWrap = QTextOption::NoWrap,
                    WordWrap = QTextOption::WordWrap,
                    WrapAnywhere = QTextOption::WrapAnywhere,
                    WrapAtWordBoundaryOrAnywhere = QTextOption::WrapAtWordBoundaryOrAnywhere, // COMPAT
                    Wrap = QTextOption::WrapAtWordBoundaryOrAnywhere
                  };
    Q_ENUM(WrapMode)

    enum SelectionMode {
        SelectCharacters,
        SelectWords
    };
    Q_ENUM(SelectionMode)

    enum RenderType { QtRendering,
                      NativeRendering
                    };
    Q_ENUM(RenderType)

    bool isCursorVisible();

    QFont font() const;
    void setFont(const QFont &font);

    QColor color() const;
    void setColor(const QColor &c);

    HAlignment hAlign() const;
    void setHAlign(HAlignment align);
    void resetHAlign();
    HAlignment effectiveHAlign() const;

    VAlignment vAlign() const;
    void setVAlign(VAlignment align);

    WrapMode wrapMode() const;
    void setWrapMode(WrapMode w);

    int lineCount() const;

    int length() const;

    QRectF cursorRectangle() const;

    void componentComplete() Q_DECL_OVERRIDE;

    LineSurfacePrivate* getPriv() { Q_D(LineSurface); return d; }

    Q_INVOKABLE void collapseLines(int pos, int num, QString &replacement);
    Q_INVOKABLE void expandLines(int pos, int num, QString &replacement);

    Q_INVOKABLE void setComponents(lv::TextEdit* te);

    void setTextDocument(QTextDocument* td);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QRectF clipRect() const Q_DECL_OVERRIDE;

    Q_INVOKABLE QString getText(int start, int end) const;
    Q_INVOKABLE QString getFormattedText(int start, int end) const;

Q_SIGNALS:

    void textChanged();
    void contentSizeChanged();
    void cursorRectangleChanged();
    void colorChanged(const QColor &color);
    void selectedTextColorChanged(const QColor &color);
    void fontChanged(const QFont &font);
    void horizontalAlignmentChanged(LineSurface::HAlignment alignment);
    void verticalAlignmentChanged(LineSurface::VAlignment alignment);
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
    bool isRightToLeft(int start, int end);
    void insert(int position, const QString &text);
    void remove(int start, int end);
    Q_REVISION(2) void append(const QString &text);
    Q_REVISION(7) void clear();
    void textDocumentFinished();


private Q_SLOTS:
    void q_textChanged();
    void q_contentsChange(int, int, int);
    void moveCursorDelegate();
    void createCursor();
    void updateWholeDocument();
    void invalidateBlock(const QTextBlock &block);
    void updateCursor();
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

    bool event(QEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;

    // mouse filter?
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
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
