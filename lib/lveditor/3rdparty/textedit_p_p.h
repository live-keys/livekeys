/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQuick module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef LVTEXTEDIT_P_P_H
#define LVTEXTEDIT_P_P_H

#include "textedit_p.h"
#include "private/qquickimplicitsizeitem_p_p.h"
#include "private/qquickitem_p.h"
#include "documenthandler.h"
#include <QtQml/qqml.h>
#include <QtCore/qlist.h>
#include <climits>
#include "palettemanager.h"
#include "linecontrol.h"
#include <deque>
#include <set>

class QTextLayout;

namespace lv {

class PaletteManager;
class TextControl;
class TextNode;
class TextNodeEngine;
class TextDocumentLayout;

class TextEditPrivate : public QQuickImplicitSizeItemPrivate
{
public:
    Q_DECLARE_PUBLIC(TextEdit)

    typedef TextEdit Public;


    class NodeAction {
    public:
        enum NodeActionType {
            Delete, Create, Shift, Refresh
        };

        NodeAction(NodeActionType t, QString debug = QString(""))
            : type(t)
            , debugMessage(debug)
            , yShift(0)
            , offset(0) {}

        NodeActionType type;
        std::set<int> nodeNumbers;
        QString debugMessage;
        int yShift;
        int offset;
    };

    class Node
    {
    public:
        explicit Node(int startPos, int blockNumber, TextNode* node)
            : m_startPos(startPos), m_blockNumber(blockNumber), m_node(node), m_dirty(false) { }
        TextNode* textNode() const { return m_node; }
        void moveStartPos(int delta) { Q_ASSERT(m_startPos + delta > 0); m_startPos += delta; }
        void moveBlockNumber(int delta) { m_blockNumber += delta; }
        int startPos() const { return m_startPos; }
        int blockNumber() const { return m_blockNumber; }
        void setDirty() {
            m_dirty = true; }
        bool dirty() const { return m_dirty; }

    private:
        int m_startPos;
        int m_blockNumber;
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

    TextEditPrivate()
        : readOnly(false), color(QRgb(0xFF000000)), selectionColor(QRgb(0xFF000080)), selectedTextColor(QRgb(0xFFFFFFFF))
        , textMargin(0.0), xoff(0), yoff(0)
        , font(sourceFont), documentHandler(nullptr), cursorComponent(nullptr), cursorItem(nullptr), document(nullptr), control(nullptr)
        /*, paletteManager(new PaletteManager)*/, lineControl(nullptr), lineSurface(nullptr)
        , lastSelectionStart(0), lastSelectionEnd(0), lineCount(0)
        , clearSelectionOnFocus(false)
        , lastHighlightChangeStart(INT_MAX)
        , lastHighlightChangeEnd(0)
        , fragmentStartPalette(nullptr)
        , fragmentEndPalette(nullptr)
        , fragmentStart(-1), fragmentEnd(-1)
        , dirtyPosition(0), paintedWidth(0), paintedHeight(0)
        , hAlign(TextEdit::AlignLeft), vAlign(TextEdit::AlignTop)
        , format(TextEdit::PlainText), wrapMode(TextEdit::NoWrap)
#if defined(QT_QUICK_DEFAULT_TEXT_RENDER_TYPE)
        , renderType(LVTextEdit::QT_QUICK_DEFAULT_TEXT_RENDER_TYPE)
#else
        , renderType(TextEdit::QtRendering)
#endif
        , contentDirection(Qt::LayoutDirectionAuto)
        , mouseSelectionMode(TextEdit::SelectCharacters)
#ifndef QT_NO_IM
        , inputMethodHints(Qt::ImhNone)
#endif
        , updateType(UpdatePaintNode)
        , highlightingInProgress(false)
        , viewport(QRect(0,0,0,0))
        , totalHeight(0)
#ifdef LV_EDITOR_DEBUG
        , debugModel(nullptr)
        , debugView(nullptr)
#endif
        , dirty(false), richText(false), cursorVisible(false), cursorPending(false)
        , focusOnPress(true), persistentSelection(false), requireImplicitWidth(false)
        , selectByMouse(false), canPaste(false), canPasteValid(false), hAlignImplicit(true)
        , textCached(true), inLayout(false), selectByKeyboard(false), selectByKeyboardSet(false)
        , hadSelection(false), invalidUntilTheEnd(false)
    {}

    ~TextEditPrivate()
    {
        qDeleteAll(textNodeMap);
    }


    static TextEditPrivate *get(TextEdit *item) {
        return static_cast<TextEditPrivate *>(QObjectPrivate::get(item)); }

    void init();
    void setTextDocument(QTextDocument* d);
    void unsetTextDocument();
    void resetInputMethod();
    void updateDefaultTextOption();
    void relayoutDocument();
    bool determineHorizontalAlignment();
    bool setHAlign(TextEdit::HAlignment, bool forceAlign = false);
    void mirrorChange() Q_DECL_OVERRIDE;
    qreal getImplicitWidth() const Q_DECL_OVERRIDE;
    Qt::LayoutDirection textDirection(const QString &text) const;
    bool isLinkHoveredConnected();

    void setNativeCursorEnabled(bool) {}
    void handleFocusEvent(QFocusEvent *event);
    void addCurrentTextNodeToRoot(TextNodeEngine *, QSGTransformNode *, TextNode*, TextNodeIterator&, int startPos, int blockNumber);
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

    QQmlComponent* cursorComponent;
    QQuickItem* cursorItem;
    QTextDocument *document;
    TextControl *control;
    QList<Node*> textNodeMap;

    PaletteManager *paletteManager;
    LineControl* lineControl;
    LineSurface* lineSurface;

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

    QQuickItem* fragmentStartPalette;
    QQuickItem* fragmentEndPalette;
    int fragmentStart, fragmentEnd;
    int dirtyPosition;

    qreal paintedWidth;
    qreal paintedHeight;

    TextEdit::HAlignment hAlign;
    TextEdit::VAlignment vAlign;
    TextEdit::TextFormat format;
    TextEdit::WrapMode wrapMode;
    TextEdit::RenderType renderType;
    Qt::LayoutDirection contentDirection;
    TextEdit::SelectionMode mouseSelectionMode;
#ifndef QT_NO_IM
    Qt::InputMethodHints inputMethodHints;
#endif
    UpdateType updateType;

    bool highlightingInProgress;

    QRect viewport;
    int totalHeight;
    std::vector<VisibleSection> sectionsForViewport;
    std::deque<NodeAction> actionQueue;
    std::vector<std::pair<int, QQuickItem*>> displayedPalettes;

#ifdef LV_EDITOR_DEBUG
    TextEditNodeDebugModel* debugModel;
    QQuickItem* debugView;
#endif
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
    bool invalidUntilTheEnd: 1;
};

}

#endif // LVTEXTEDIT_P_P_H
