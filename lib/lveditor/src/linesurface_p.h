#ifndef LINESURFACE_P_H
#define LINESURFACE_P_H

#include "linesurface.h"
#include "private/qquickimplicitsizeitem_p_p.h"
#include "private/qquickitem_p.h"
#include "documenthandler.h"
#include <QtQml/qqml.h>
#include <QtCore/qlist.h>
#include <climits>

class QTextLayout;

namespace lv {

class TextControl;
class TextNode;
class TextNodeEngine;
class LineManager;

class LV_EDITOR_EXPORT LineSurfacePrivate : public QQuickImplicitSizeItemPrivate
{
public:
    Q_DECLARE_PUBLIC(LineSurface)

    typedef LineSurface Public;

    class Node
    {
    public:
        explicit Node(int startPos, TextNode* node)
            : m_startPos(startPos), m_node(node), m_dirty(false) { }
        TextNode* textNode() const { return m_node; }
        void moveStartPos(int delta) { Q_ASSERT(m_startPos + delta > 0); m_startPos += delta; }
        int startPos() const { return m_startPos; }
        void setDirty() {
            m_dirty = true; }
        bool dirty() const { return m_dirty; }

    private:
        int m_startPos;
        TextNode* m_node;
        bool m_dirty;
    };
    typedef QList<Node*>::iterator TextNodeIterator;

    class ExtraData
    {
    public:
        ExtraData();

        qreal padding;
        qreal topPadding;
        qreal leftPadding;
        qreal rightPadding;
        qreal bottomPadding;
        bool explicitTopPadding : 1;
        bool explicitLeftPadding : 1;
        bool explicitRightPadding : 1;
        bool explicitBottomPadding : 1;
        bool implicitResize : 1;
    };
    QLazilyAllocated<ExtraData> extra;
    void implicitWidthChanged() Q_DECL_OVERRIDE;
    void implicitHeightChanged() Q_DECL_OVERRIDE;

    LineSurfacePrivate()
        : readOnly(false), color(QRgb(0xFF000000)), selectionColor(QRgb(0xFF000080)), selectedTextColor(QRgb(0xFFFFFFFF))
        , textMargin(0.0), xoff(0), yoff(0)
        , font(sourceFont), documentHandler(nullptr), cursorComponent(nullptr), cursorItem(nullptr), document(nullptr),  control(nullptr)
        , lastSelectionStart(0), lastSelectionEnd(0), lineCount(0)
        , clearSelectionOnFocus(false)
        , lastHighlightChangeStart(INT_MAX)
        , lastHighlightChangeEnd(0)
        , fragmentStart(-1)
        , fragmentEnd(-1)
        , hAlign(LineSurface::AlignLeft), vAlign(LineSurface::AlignTop)
        , wrapMode(LineSurface::NoWrap)
#if defined(QT_QUICK_DEFAULT_TEXT_RENDER_TYPE)
        , renderType(LVTextEdit::QT_QUICK_DEFAULT_TEXT_RENDER_TYPE)
#else
        , renderType(LineSurface::QtRendering)
#endif
        , contentDirection(Qt::LayoutDirectionAuto)
        , mouseSelectionMode(LineSurface::SelectCharacters)
#ifndef QT_NO_IM
        , inputMethodHints(Qt::ImhNone)
#endif
        , updateType(UpdatePaintNode)
        , highlightingInProgress(false)
        , dirty(false), richText(false), cursorVisible(false), cursorPending(false)
        , focusOnPress(true), persistentSelection(false), requireImplicitWidth(false)
        , selectByMouse(false), canPaste(false), canPasteValid(false), hAlignImplicit(true)
        , textCached(true), inLayout(false), selectByKeyboard(false), selectByKeyboardSet(false)
        , hadSelection(false)
    {
    }

    ~LineSurfacePrivate()
    {
        qDebug() << QThread::currentThreadId();
        qDeleteAll(textNodeMap);
    }


    static LineSurfacePrivate *get(LineSurface *item) {
        return static_cast<LineSurfacePrivate *>(QObjectPrivate::get(item)); }

    void init();
    void setTextDocument(QTextDocument* d);
    void unsetTextDocument();
    void resetInputMethod();
    void updateDefaultTextOption();
    void relayoutDocument();
    bool setHAlign(LineSurface::HAlignment, bool forceAlign = false);
    void mirrorChange() Q_DECL_OVERRIDE;
    qreal getImplicitWidth() const Q_DECL_OVERRIDE;
    Qt::LayoutDirection textDirection(const QString &text) const;
    bool isLinkHoveredConnected();

    void setNativeCursorEnabled(bool) {}
    void handleFocusEvent(QFocusEvent *event);
    void addCurrentTextNodeToRoot(TextNodeEngine *, QSGTransformNode *, TextNode*, TextNodeIterator&, int startPos);
    TextNode* createTextNode();

#ifndef QT_NO_IM
    Qt::InputMethodHints effectiveInputMethodHints() const;
#endif

    inline qreal padding() const { return extra.isAllocated() ? extra->padding : 0.0; }
    void setTopPadding(qreal value, bool reset = false);
    void setLeftPadding(qreal value, bool reset = false);
    void setRightPadding(qreal value, bool reset = false);
    void setBottomPadding(qreal value, bool reset = false);

    bool isImplicitResizeEnabled() const;
    void setImplicitResizeEnabled(bool enabled);

    bool readOnly;
    QColor color;
    QColor selectionColor;
    QColor selectedTextColor;

    QSizeF contentSize;

    qreal textMargin;
    qreal xoff;
    qreal yoff;

    QString text;
    QUrl baseUrl;
    QFont sourceFont;
    QFont font;

    lv::DocumentHandler* documentHandler;
    TextEdit* textEdit;

    QQmlComponent* cursorComponent;
    QQuickItem* cursorItem;
    QTextDocument *document;
    TextControl *control;
    QList<Node*> textNodeMap;

    int lastSelectionStart;
    int lastSelectionEnd;
    int lineCount;
    bool clearSelectionOnFocus;

    enum UpdateType {
        UpdateNone,
        UpdateOnlyPreprocess,
        UpdatePaintNode
    };

    int lastHighlightChangeStart;
    int lastHighlightChangeEnd;

    int fragmentStart;
    int fragmentEnd;

    LineSurface::HAlignment hAlign;
    LineSurface::VAlignment vAlign;
    LineSurface::WrapMode wrapMode;
    LineSurface::RenderType renderType;
    Qt::LayoutDirection contentDirection;
    LineSurface::SelectionMode mouseSelectionMode;
#ifndef QT_NO_IM
    Qt::InputMethodHints inputMethodHints;
#endif
    UpdateType updateType;

    bool highlightingInProgress;

    bool dirty : 1;
    bool richText : 1;
    bool cursorVisible : 1;
    bool cursorPending : 1;
    bool focusOnPress : 1;
    bool persistentSelection : 1;
    bool requireImplicitWidth:1;
    bool selectByMouse:1;
    bool canPaste:1;
    bool canPasteValid:1;
    bool hAlignImplicit:1;
    bool textCached:1;
    bool inLayout:1;
    bool selectByKeyboard:1;
    bool selectByKeyboardSet:1;
    bool hadSelection : 1;

    int prevLineNumber;
    int lineNumber;
    int dirtyPos;
    int deltaLineNumber;
    LineManager *lineManager;
};

}

#endif // LINESURFACE_P_H
