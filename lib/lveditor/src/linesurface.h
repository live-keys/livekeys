#ifndef LINESURFACE_H
#define LINESURFACE_H


#include "private/qquickimplicitsizeitem_p.h"
#include <QtGui/qtextoption.h>
#include "documenthandler.h"
#include "qquickpainteditem.h"

class QTextBlock;

namespace lv {

class LineSurfacePrivate;

class LV_EDITOR_EXPORT LineSurface : public QQuickImplicitSizeItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor NOTIFY selectionColorChanged)
    Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor NOTIFY selectedTextColorChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(HAlignment effectiveHorizontalAlignment READ effectiveHAlign NOTIFY effectiveHorizontalAlignmentChanged)
    Q_PROPERTY(VAlignment verticalAlignment READ vAlign WRITE setVAlign NOTIFY verticalAlignmentChanged)
    Q_PROPERTY(WrapMode wrapMode READ wrapMode WRITE setWrapMode NOTIFY wrapModeChanged)
    Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged)
    Q_PROPERTY(int length READ length NOTIFY textChanged)
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentSizeChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight NOTIFY contentSizeChanged)
    Q_PROPERTY(qreal paintedWidth READ contentWidth NOTIFY contentSizeChanged)  // Compatibility
    Q_PROPERTY(qreal paintedHeight READ contentHeight NOTIFY contentSizeChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QQmlComponent* cursorDelegate READ cursorDelegate WRITE setCursorDelegate NOTIFY cursorDelegateChanged)
    Q_PROPERTY(bool activeFocusOnPress READ focusOnPress WRITE setFocusOnPress NOTIFY activeFocusOnPressChanged)
    Q_PROPERTY(bool persistentSelection READ persistentSelection WRITE setPersistentSelection NOTIFY persistentSelectionChanged)
    Q_PROPERTY(qreal textMargin READ textMargin WRITE setTextMargin NOTIFY textMarginChanged)
    Q_PROPERTY(Qt::InputMethodHints inputMethodHints READ inputMethodHints WRITE setInputMethodHints NOTIFY inputMethodHintsChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool inputMethodComposing READ isInputMethodComposing NOTIFY inputMethodComposingChanged)
    Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl RESET resetBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(RenderType renderType READ renderType WRITE setRenderType NOTIFY renderTypeChanged)
    Q_PROPERTY(QString hoveredLink READ hoveredLink NOTIFY linkHovered REVISION 2)
    Q_PROPERTY(qreal padding READ padding WRITE setPadding RESET resetPadding NOTIFY paddingChanged REVISION 6)
    Q_PROPERTY(qreal topPadding READ topPadding WRITE setTopPadding RESET resetTopPadding NOTIFY topPaddingChanged REVISION 6)
    Q_PROPERTY(qreal leftPadding READ leftPadding WRITE setLeftPadding RESET resetLeftPadding NOTIFY leftPaddingChanged REVISION 6)
    Q_PROPERTY(qreal rightPadding READ rightPadding WRITE setRightPadding RESET resetRightPadding NOTIFY rightPaddingChanged REVISION 6)
    Q_PROPERTY(qreal bottomPadding READ bottomPadding WRITE setBottomPadding RESET resetBottomPadding NOTIFY bottomPaddingChanged REVISION 6)
    Q_PROPERTY(lv::DocumentHandler* documentHandler READ documentHandler WRITE setDocumentHandler NOTIFY documentHandlerChanged)
    Q_PROPERTY(int fragmentStart READ fragmentStart WRITE setFragmentStart RESET resetFragmentStart NOTIFY fragmentStartChanged)
    Q_PROPERTY(int fragmentEnd READ fragmentEnd WRITE setFragmentEnd RESET resetFragmentEnd NOTIFY fragmentEndChanged)

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

    QColor selectionColor() const;
    void setSelectionColor(const QColor &c);

    QColor selectedTextColor() const;
    void setSelectedTextColor(const QColor &c);

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

    QQmlComponent* cursorDelegate() const;
    void setCursorDelegate(QQmlComponent*);

    bool focusOnPress() const;
    void setFocusOnPress(bool on);

    bool persistentSelection() const;
    void setPersistentSelection(bool on);

    qreal textMargin() const;
    void setTextMargin(qreal margin);

    Qt::InputMethodHints inputMethodHints() const;
    void setInputMethodHints(Qt::InputMethodHints hints);

    bool canUndo() const;
    bool canRedo() const;

    QRectF cursorRectangle() const;

    void componentComplete() Q_DECL_OVERRIDE;

    /* FROM EDIT */
    void setReadOnly(bool);
    bool isReadOnly() const;

    LineSurfacePrivate* getPriv() { Q_D(LineSurface); return d; }

    Q_INVOKABLE void collapseLines(int pos, int num, QString &replacement);
    Q_INVOKABLE void expandLines(int pos, int num, QString &replacement);

    Q_INVOKABLE void setComponents(lv::TextEdit* te);

    void setTextDocument(QTextDocument* td);

    qreal contentWidth() const;
    qreal contentHeight() const;

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &url);
    void resetBaseUrl();

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QRectF clipRect() const Q_DECL_OVERRIDE;

    bool isInputMethodComposing() const;

    RenderType renderType() const;
    void setRenderType(RenderType renderType);

    Q_INVOKABLE QString getText(int start, int end) const;
    Q_INVOKABLE QString getFormattedText(int start, int end) const;


    QString hoveredLink() const;

    qreal padding() const;
    void setPadding(qreal padding);
    void resetPadding();

    qreal topPadding() const;
    void setTopPadding(qreal padding);
    void resetTopPadding();

    qreal leftPadding() const;
    void setLeftPadding(qreal padding);
    void resetLeftPadding();

    qreal rightPadding() const;
    void setRightPadding(qreal padding);
    void resetRightPadding();

    qreal bottomPadding() const;
    void setBottomPadding(qreal padding);
    void resetBottomPadding();

    int fragmentStart() const;
    void setFragmentStart(int frStart);
    void resetFragmentStart();

    int fragmentEnd() const;
    void setFragmentEnd(int frStart);
    void resetFragmentEnd();

    lv::DocumentHandler* documentHandler();
    void setDocumentHandler(lv::DocumentHandler* dh);

Q_SIGNALS:

    void textChanged();
    void contentSizeChanged();
    void cursorRectangleChanged();
    void colorChanged(const QColor &color);
    void selectionColorChanged(const QColor &color);
    void selectedTextColorChanged(const QColor &color);
    void fontChanged(const QFont &font);
    void horizontalAlignmentChanged(LineSurface::HAlignment alignment);
    void verticalAlignmentChanged(LineSurface::VAlignment alignment);
    void wrapModeChanged();
    void lineCountChanged();
    void readOnlyChanged(bool isReadOnly);
    void cursorDelegateChanged();
    void activeFocusOnPressChanged(bool activeFocusOnPressed);
    void persistentSelectionChanged(bool isPersistentSelection);
    void textMarginChanged(qreal textMargin);
    void linkActivated(const QString &link);
    Q_REVISION(2) void linkHovered(const QString &link);
    void canUndoChanged();
    void canRedoChanged();
    void inputMethodComposingChanged();
    void effectiveHorizontalAlignmentChanged();
    void baseUrlChanged();
    void inputMethodHintsChanged();
    void renderTypeChanged();
    void textDocumentChanged();
    void documentHandlerChanged();
    Q_REVISION(6) void editingFinished();
    Q_REVISION(6) void paddingChanged();
    Q_REVISION(6) void topPaddingChanged();
    Q_REVISION(6) void leftPaddingChanged();
    Q_REVISION(6) void rightPaddingChanged();
    Q_REVISION(6) void bottomPaddingChanged();
    void dirtyBlockPosition(int pos);
    void stateChangeSignal(int blockNum);
    void textDocumentFinishedUpdating();

    void fragmentStartChanged();
    void fragmentEndChanged();

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
    void updateFragmentVisibility();

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
    friend class DocumentHandler;
    friend class LineManager;


private:
    Q_DISABLE_COPY(LineSurface)
    Q_DECLARE_PRIVATE(LineSurface)

};

}

QML_DECLARE_TYPE(lv::LineSurface)


#endif // LINESURFACE_H
