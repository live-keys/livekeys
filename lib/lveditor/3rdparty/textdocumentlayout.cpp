/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "textdocumentlayout.h"
#include "qtextlist.h"
#include "private/qtextengine_p.h"
#include "private/qabstracttextdocumentlayout_p.h"

#include <qpainter.h>
#include <qmath.h>
#include <qrect.h>
#include <qpalette.h>
#include <qdebug.h>
#include <qvarlengtharray.h>
#include <limits.h>
#include <qbasictimer.h>

#include <algorithm>
#include "linemanager.h"

#ifdef LAYOUT_DEBUG
#define LDEBUG qDebug()
#define INC_INDENT debug_indent += "  "
#define DEC_INDENT debug_indent = debug_indent.left(debug_indent.length()-2)
#else
#define LDEBUG if(0) qDebug()
#define INC_INDENT do {} while(0)
#define DEC_INDENT do {} while(0)
#endif



class QTextFrameIterator {
    QTextFrame *f;
    int b;
    int e;
    QTextFrame *cf;
    int cb;

    friend class QTextFrame;
    friend class lv::TextDocumentLayoutPrivate;
    QTextFrameIterator(QTextFrame *frame, int block, int begin, int end)
    {
        f = frame;
        b = begin;
        e = end;
        cf = nullptr;
        cb = block;
    }
public:
    static QTextFrameIterator findFrameBeginning(const QTextFrame *frame);
    QTextFrameIterator(){
        f = nullptr;
        b = 0;
        e = 0;
        cf = nullptr;
        cb = 0;
    }
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QTextFrameIterator(const QTextFrameIterator &other) Q_DECL_NOTHROW
    {
        f = other.f;
        b = other.b;
        e = other.e;
        cf = other.cf;
        cb = other.cb;
    } // = default
    QTextFrameIterator &operator=(const QTextFrameIterator &other) Q_DECL_NOTHROW
    {
        f = other.f;
        b = other.b;
        e = other.e;
        cf = other.cf;
        cb = other.cb;
        return *this;
    } // = default
    QTextFrameIterator(QTextFrameIterator &&other) Q_DECL_NOTHROW // = default
    { memcpy(static_cast<void *>(this), static_cast<void *>(&other), sizeof(QTextFrameIterator)); }
    QTextFrameIterator &operator=(QTextFrameIterator &&other) Q_DECL_NOTHROW // = default
    { memcpy(static_cast<void *>(this), static_cast<void *>(&other), sizeof(QTextFrameIterator)); return *this; }
#endif

    QTextFrame *parentFrame() const { return f; }

    QTextFrame *currentFrame() const
    {
        return cf;
    }
    QTextBlock currentBlock() const
    {
        if (!f)
            return QTextBlock();
        return QTextBlock(f->docHandle(), cb);
    }

    bool atEnd() const { return !cf && cb == e; }

    inline bool operator==(const QTextFrameIterator &o) const { return f == o.f && cf == o.cf && cb == o.cb; }
    inline bool operator!=(const QTextFrameIterator &o) const { return f != o.f || cf != o.cf || cb != o.cb; }
    QTextFrameIterator &operator++()
    {
        const QTextDocumentPrivate *priv = f->docHandle();
        QTextDocument* document = f->document();
        const QTextDocumentPrivate::BlockMap &map = priv->blockMap();
        if (cf) {
            int end = cf->lastPosition() + 1;
            cb = map.findNode(end);
            cf = 0;
        } else if (cb) {
            cb = map.next(cb);
            if (cb == e)
                return *this;

            if (!f->childFrames().isEmpty()) {
                int pos = map.position(cb);
                // check if we entered a frame
                QTextDocumentPrivate::FragmentIterator frag = priv->find(pos-1);
                if (priv->buffer().at(frag->stringPosition) != QChar::ParagraphSeparator) {

                    auto formats = priv->formatCollection();
                    int objectIndex = formats->format(frag->format).objectIndex();

                    QTextFrame *nf = qobject_cast<QTextFrame *>(document->object(objectIndex));
                    if (nf) {
                        if (priv->buffer().at(frag->stringPosition) == QTextBeginningOfFrame && nf != f) {
                            cf = nf;
                            cb = 0;
                        } else {
                            Q_ASSERT(priv->buffer().at(frag->stringPosition) != QTextEndOfFrame);
                        }
                    }
                }
            }
        }
        return *this;
    }
    inline QTextFrameIterator operator++(int) { QTextFrameIterator tmp = *this; operator++(); return tmp; }
    QTextFrameIterator &operator--()
    {
        const QTextDocumentPrivate *priv = f->docHandle();
        QTextDocument* document = f->document();
        const QTextDocumentPrivate::BlockMap &map = priv->blockMap();
        if (cf) {
            int start = cf->firstPosition() - 1;
            cb = map.findNode(start);
            cf = 0;
        } else {
            if (cb == b)
                goto end;
            if (cb != e) {
                int pos = map.position(cb);
                // check if we have to enter a frame
                QTextDocumentPrivate::FragmentIterator frag = priv->find(pos-1);
                if (priv->buffer().at(frag->stringPosition) != QChar::ParagraphSeparator) {

                    auto formats = priv->formatCollection();
                    int objectIndex = formats->format(frag->format).objectIndex();

                    QTextFrame *pf = qobject_cast<QTextFrame *>(document->object(objectIndex));

                    if (pf) {
                        if (priv->buffer().at(frag->stringPosition) == QTextBeginningOfFrame) {
                            Q_ASSERT(pf == f);
                        } else if (priv->buffer().at(frag->stringPosition) == QTextEndOfFrame) {
                            Q_ASSERT(pf != f);
                            cf = pf;
                            cb = 0;
                            goto end;
                        }
                    }
                }
            }
            cb = map.previous(cb);
        }
     end:
        return *this;
    }
    inline QTextFrameIterator operator--(int) { QTextFrameIterator tmp = *this; operator--(); return tmp; }
};

inline QTextFrameIterator QTextFrameIterator::findFrameBeginning(const QTextFrame *frame)
{
    const QTextDocumentPrivate *priv = frame->docHandle();
    int b = priv->blockMap().findNode(frame->firstPosition());
    int e = priv->blockMap().findNode(frame->lastPosition()+1);
    return QTextFrameIterator(const_cast<QTextFrame *>(frame), b, b, e);
}

Q_DECLARE_TYPEINFO(QTextFrameIterator, Q_MOVABLE_TYPE);

// ################ should probably add frameFormatChange notification!

struct TextLayoutStruct;

class TextFrameData : public QTextFrameLayoutData
{
public:
    TextFrameData();

    // relative to parent frame
    QFixedPoint position;
    QFixedSize size;

    // contents starts at (margin+border/margin+border)
    QFixed topMargin;
    QFixed bottomMargin;
    QFixed leftMargin;
    QFixed rightMargin;
    QFixed border;
    QFixed padding;
    QFixed contentsWidth;
    QFixed contentsHeight;
    QFixed oldContentsWidth;

    // accumulated margins
    QFixed effectiveTopMargin;
    QFixed effectiveBottomMargin;

    QFixed minimumWidth;
    QFixed maximumWidth;

    TextLayoutStruct *currentLayoutStruct;

    bool sizeDirty;
    bool layoutDirty;

    QVector<QPointer<QTextFrame> > floats;
};

TextFrameData::TextFrameData()
    : maximumWidth(QFIXED_MAX),
      currentLayoutStruct(0), sizeDirty(true), layoutDirty(true)
{
}

struct TextLayoutStruct {
    TextLayoutStruct() : maximumWidth(QFIXED_MAX), fullLayout(false)
    {}
    QTextFrame *frame;
    QFixed x_left;
    QFixed x_right;
    QFixed frameY; // absolute y position of the current frame
    QFixed y; // always relative to the current frame
    QFixed contentsWidth;
    QFixed minimumWidth;
    QFixed maximumWidth;
    bool fullLayout;
    QList<QTextFrame *> pendingFloats;
    QFixed pageHeight;
    QFixed pageBottom;
    QFixed pageTopMargin;
    QFixed pageBottomMargin;
    QRectF updateRect;
    QRectF updateRectForFloats;

    inline void addUpdateRectForFloat(const QRectF &rect) {
        if (updateRectForFloats.isValid())
            updateRectForFloats |= rect;
        else
            updateRectForFloats = rect;
    }

    inline QFixed absoluteY() const
    { return frameY + y; }

    inline int currentPage() const
    { return pageHeight == 0 ? 0 : (absoluteY() / pageHeight).truncate(); }

    inline void newPage()
    { if (pageHeight == QFIXED_MAX) return; pageBottom += pageHeight; y = pageBottom - pageHeight + pageBottomMargin + pageTopMargin - frameY; }
};

static TextFrameData *createData(QTextFrame *f)
{
    TextFrameData *data = new TextFrameData;
    f->setLayoutData(data);
    return data;
}

static inline TextFrameData *data(QTextFrame *f)
{
    TextFrameData *data = static_cast<TextFrameData *>(f->layoutData());
    if (!data)
        data = createData(f);
    return data;
}

static bool isFrameFromInlineObject(QTextFrame *f)
{
    return f->firstPosition() > f->lastPosition();
}

struct CheckPoint
{
    QFixed y;
    QFixed frameY; // absolute y position of the current frame
    int positionInFrame;
    QFixed minimumWidth;
    QFixed maximumWidth;
    QFixed contentsWidth;
};
Q_DECLARE_TYPEINFO(CheckPoint, Q_PRIMITIVE_TYPE);

static bool operator<(const CheckPoint &checkPoint, QFixed y)
{
    return checkPoint.y < y;
}

static bool operator<(const CheckPoint &checkPoint, int pos)
{
    return checkPoint.positionInFrame < pos;
}

#if defined(Q_CC_MSVC) && _MSC_VER < 1600
//The STL implementation of MSVC 2008 requires the definitions

static bool operator<(const CheckPoint &checkPoint1, const CheckPoint &checkPoint2)
{
    return checkPoint1.y < checkPoint2.y;
}

static bool operator<(QFixed y, const CheckPoint &checkPoint)
{
    return y < checkPoint.y;
}

static bool operator<(int pos, const CheckPoint &checkPoint)
{
    return pos < checkPoint.positionInFrame;
}

#endif

static void fillBackground(QPainter *p, const QRectF &rect, QBrush brush, const QPointF &origin, const QRectF &gradientRect = QRectF())
{
    p->save();
    if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
        if (!gradientRect.isNull()) {
            QTransform m;
            m.translate(gradientRect.left(), gradientRect.top());
            m.scale(gradientRect.width(), gradientRect.height());
            brush.setTransform(m);
            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
        }
    } else {
        p->setBrushOrigin(origin);
    }
    p->fillRect(rect, brush);
    p->restore();
}

namespace lv {

class TextDocumentLayoutPrivate : public QAbstractTextDocumentLayoutPrivate
{
    Q_DECLARE_PUBLIC(TextDocumentLayout)
public:
    TextDocumentLayoutPrivate();

    QTextOption::WrapMode wordWrapMode;
#ifdef LAYOUT_DEBUG
    mutable QString debug_indent;
#endif

    int fixedColumnWidth;
    int cursorWidth;

    QSizeF lastReportedSize;
    QRectF viewportRect;
    QRectF clipRect;

    mutable int currentLazyLayoutPosition;
    mutable int lazyLayoutStepSize;
    QBasicTimer layoutTimer;
    mutable QBasicTimer sizeChangedTimer;
    uint showLayoutProgress : 1;
    uint insideDocumentChange : 1;

    int lastPageCount;
    qreal idealWidth;
    bool contentHasAlignment;

    LineManager* lineManager;

    QFixed blockIndent(const QTextBlockFormat &blockFormat) const;

    void drawFrame(const QPointF &offset, QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context,
                   QTextFrame *f) const;
    void drawFlow(const QPointF &offset, QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context,
                  QTextFrameIterator it, const QList<QTextFrame *> &floats, QTextBlock *cursorBlockNeedingRepaint) const;
    void drawBlock(const QPointF &offset, QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context,
                   const QTextBlock &bl, bool inRootFrame) const;
    void drawListItem(const QPointF &offset, QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context,
                      const QTextBlock &bl, const QTextCharFormat *selectionFormat) const;
    void drawBorder(QPainter *painter, const QRectF &rect, qreal topMargin, qreal bottomMargin, qreal border,
                    const QBrush &brush, QTextFrameFormat::BorderStyle style) const;
    void drawFrameDecoration(QPainter *painter, QTextFrame *frame, TextFrameData *fd, const QRectF &clip, const QRectF &rect) const;

    enum HitPoint {
        PointBefore,
        PointAfter,
        PointInside,
        PointExact
    };
    HitPoint hitTest(QTextFrame *frame, const QFixedPoint &point, int *position, QTextLayout **l, Qt::HitTestAccuracy accuracy) const;
    HitPoint hitTest(QTextFrameIterator it, HitPoint hit, const QFixedPoint &p,
                     int *position, QTextLayout **l, Qt::HitTestAccuracy accuracy) const;
    HitPoint hitTest(const QTextBlock &bl, const QFixedPoint &point, int *position, QTextLayout **l, Qt::HitTestAccuracy accuracy) const;

    void positionFloat(QTextFrame *frame, QTextLine *currentLine = 0);

    // calls the next one
    QRectF layoutFrame(QTextFrame *f, int layoutFrom, int layoutTo, QFixed parentY = 0);
    QRectF layoutFrame(QTextFrame *f, int layoutFrom, int layoutTo, QFixed frameWidth, QFixed frameHeight, QFixed parentY = 0);

    void layoutBlock(const QTextBlock &bl, int blockPosition, const QTextBlockFormat &blockFormat,
                     TextLayoutStruct *layoutStruct, int layoutFrom, int layoutTo, const QTextBlockFormat *previousBlockFormat);
    void layoutFlow(QTextFrameIterator it, TextLayoutStruct *layoutStruct, int layoutFrom, int layoutTo, QFixed width = 0);

    void floatMargins(const QFixed &y, const TextLayoutStruct *layoutStruct, QFixed *left, QFixed *right) const;
    QFixed findY(QFixed yFrom, const TextLayoutStruct *layoutStruct, QFixed requiredWidth) const;

    QVector<CheckPoint> checkPoints;

    QTextFrameIterator frameIteratorForYPosition(QFixed y) const;
    QTextFrameIterator frameIteratorForTextPosition(int position) const;

    void ensureLayouted(QFixed y) const;
    void ensureLayoutedByPosition(int position) const;
    inline void ensureLayoutFinished() const
    { ensureLayoutedByPosition(INT_MAX); }
    void layoutStep() const;

    QRectF frameBoundingRectInternal(QTextFrame *frame) const;

    qreal scaleToDevice(qreal value) const;
    QFixed scaleToDevice(QFixed value) const;
};

TextDocumentLayoutPrivate::TextDocumentLayoutPrivate()
    : fixedColumnWidth(-1),
      cursorWidth(1),
      currentLazyLayoutPosition(-1),
      lazyLayoutStepSize(1000),
      lastPageCount(-1),
      lineManager(nullptr)
{
    showLayoutProgress = true;
    insideDocumentChange = false;
    idealWidth = 0;
    contentHasAlignment = false;
}


QTextFrameIterator TextDocumentLayoutPrivate::frameIteratorForYPosition(QFixed y) const
{
    QTextFrame *rootFrame = document->rootFrame();

    if (checkPoints.isEmpty()
        || y < 0 || y > data(rootFrame)->size.height)
    {
        return QTextFrameIterator::findFrameBeginning(rootFrame);
    }

    QVector<CheckPoint>::ConstIterator checkPoint = std::lower_bound(checkPoints.begin(), checkPoints.end(), y);
    if (checkPoint == checkPoints.end())
    {
        return QTextFrameIterator::findFrameBeginning(rootFrame);
    }

    if (checkPoint != checkPoints.begin())
        --checkPoint;

    const int position = rootFrame->firstPosition() + checkPoint->positionInFrame;
    return frameIteratorForTextPosition(position);
}

QTextFrameIterator TextDocumentLayoutPrivate::frameIteratorForTextPosition(int position) const
{
    QTextFrame *rootFrame = document->rootFrame();

    const QTextDocumentPrivate::BlockMap &map = docPrivate->blockMap();
    const int begin = map.findNode(rootFrame->firstPosition());
    const int end = map.findNode(rootFrame->lastPosition()+1);

    const int block = map.findNode(position);
    const int blockPos = map.position(block);

    QTextFrameIterator it(rootFrame, block, begin, end);

    QTextFrame *containingFrame = document->frameAt(blockPos);
    if (containingFrame != rootFrame) {
        while (containingFrame->parentFrame() != rootFrame) {
            containingFrame = containingFrame->parentFrame();
            Q_ASSERT(containingFrame);
        }

        it.cf = containingFrame;
        it.cb = 0;
    }

    return it;
}

TextDocumentLayoutPrivate::HitPoint
TextDocumentLayoutPrivate::hitTest(QTextFrame *frame, const QFixedPoint &point, int *position, QTextLayout **l, Qt::HitTestAccuracy accuracy) const
{
    TextFrameData *fd = data(frame);
    // #########
    if (fd->layoutDirty)
        return PointAfter;
    Q_ASSERT(!fd->layoutDirty);
    Q_ASSERT(!fd->sizeDirty);
    const QFixedPoint relativePoint(point.x - fd->position.x, point.y - fd->position.y);

    QTextFrame *rootFrame = document->rootFrame();

//     LDEBUG << "checking frame" << frame->firstPosition() << "point=" << point
//            << "position" << fd->position << "size" << fd->size;
    if (frame != rootFrame) {
        if (relativePoint.y < 0 || relativePoint.x < 0) {
            *position = frame->firstPosition() - 1;
//             LDEBUG << "before pos=" << *position;
            return PointBefore;
        } else if (relativePoint.y > fd->size.height || relativePoint.x > fd->size.width) {
            *position = frame->lastPosition() + 1;
//             LDEBUG << "after pos=" << *position;
            return PointAfter;
        }
    }

    if (isFrameFromInlineObject(frame)) {
        *position = frame->firstPosition() - 1;
        return PointExact;
    }

    const QList<QTextFrame *> childFrames = frame->childFrames();
    for (int i = 0; i < childFrames.size(); ++i) {
        QTextFrame *child = childFrames.at(i);
        if (isFrameFromInlineObject(child)
            && child->frameFormat().position() != QTextFrameFormat::InFlow
            && hitTest(child, relativePoint, position, l, accuracy) == PointExact)
        {
            return PointExact;
        }
    }

    QTextFrameIterator it = QTextFrameIterator::findFrameBeginning(frame);

    if (frame == rootFrame) {
        it = frameIteratorForYPosition(relativePoint.y);

        Q_ASSERT(it.parentFrame() == frame);
    }

    if (it.currentFrame())
        *position = it.currentFrame()->firstPosition();
    else
        *position = it.currentBlock().position();

    return hitTest(it, PointBefore, relativePoint, position, l, accuracy);
}

TextDocumentLayoutPrivate::HitPoint
TextDocumentLayoutPrivate::hitTest(QTextFrameIterator it, HitPoint hit, const QFixedPoint &p,
                                    int *position, QTextLayout **l, Qt::HitTestAccuracy accuracy) const
{
    INC_INDENT;

    for (; !it.atEnd(); ++it) {
        QTextFrame *c = it.currentFrame();
        HitPoint hp;
        int pos = -1;
        if (c) {
            hp = hitTest(c, p, &pos, l, accuracy);
        } else {
            hp = hitTest(it.currentBlock(), p, &pos, l, accuracy);
        }
        if (hp >= PointInside) {
            hit = hp;
            *position = pos;
            break;
        }
        if (hp == PointBefore && pos < *position) {
            *position = pos;
            hit = hp;
        } else if (hp == PointAfter && pos > *position) {
            *position = pos;
            hit = hp;
        }
    }

    DEC_INDENT;
//     LDEBUG << "inside=" << hit << " pos=" << *position;
    return hit;
}

TextDocumentLayoutPrivate::HitPoint
TextDocumentLayoutPrivate::hitTest(const QTextBlock &bl, const QFixedPoint &point, int *position, QTextLayout **l,
                                    Qt::HitTestAccuracy accuracy) const
{
    QTextLayout *tl = bl.layout();
    QRectF textrect = tl->boundingRect();
    textrect.translate(tl->position());
//     LDEBUG << "    checking block" << bl.position() << "point=" << point
//            << "    tlrect" << textrect;
    *position = bl.position();
    if (point.y.toReal() < textrect.top()) {
//             LDEBUG << "    before pos=" << *position;
        return PointBefore;
    } else if (point.y.toReal() > textrect.bottom()) {
        *position += bl.length();
//             LDEBUG << "    after pos=" << *position;
        return PointAfter;
    }

    QPointF pos = point.toPointF() - tl->position();

    // ### rtl?

    HitPoint hit = PointInside;
    *l = tl;
    int off = 0;
    for (int i = 0; i < tl->lineCount(); ++i) {
        QTextLine line = tl->lineAt(i);
        const QRectF lr = line.naturalTextRect();
        if (lr.top() > pos.y()) {
            off = qMin(off, line.textStart());
        } else if (lr.bottom() <= pos.y()) {
            off = qMax(off, line.textStart() + line.textLength());
        } else {
            if (lr.left() <= pos.x() && lr.right() >= pos.x())
                hit = PointExact;
            // when trying to hit an anchor we want it to hit not only in the left
            // half
            if (accuracy == Qt::ExactHit)
                off = line.xToCursor(pos.x(), QTextLine::CursorOnCharacter);
            else
                off = line.xToCursor(pos.x(), QTextLine::CursorBetweenCharacters);
            break;
        }
    }
    *position += off;

//     LDEBUG << "    inside=" << hit << " pos=" << *position;
    return hit;
}

// ### could be moved to QTextBlock
QFixed TextDocumentLayoutPrivate::blockIndent(const QTextBlockFormat &blockFormat) const
{
    qreal indent = blockFormat.indent();

    QTextObject *object = document->objectForFormat(blockFormat);
    if (object)
        indent += object->format().toListFormat().indent();

    if (qIsNull(indent))
        return 0;

    qreal scale = 1;
    if (paintDevice) {
        scale = qreal(paintDevice->logicalDpiY()) / qreal(qt_defaultDpi());
    }

    return QFixed::fromReal(indent * scale * document->indentWidth());
}

void TextDocumentLayoutPrivate::drawBorder(QPainter *painter, const QRectF &rect, qreal topMargin, qreal bottomMargin,
                                            qreal border, const QBrush &brush, QTextFrameFormat::BorderStyle style) const
{
    const qreal pageHeight = document->pageSize().height();
    const int topPage = pageHeight > 0 ? static_cast<int>(rect.top() / pageHeight) : 0;
    const int bottomPage = pageHeight > 0 ? static_cast<int>((rect.bottom() + border) / pageHeight) : 0;

    Q_UNUSED(style);

    bool turn_off_antialiasing = !(painter->renderHints() & QPainter::Antialiasing);
    painter->setRenderHint(QPainter::Antialiasing);

    for (int i = topPage; i <= bottomPage; ++i) {
        QRectF clipped = rect.toRect();

        if (topPage != bottomPage) {
            clipped.setTop(qMax(clipped.top(), i * pageHeight + topMargin - border));
            clipped.setBottom(qMin(clipped.bottom(), (i + 1) * pageHeight - bottomMargin));

            if (clipped.bottom() <= clipped.top())
                continue;
        }

        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(brush);
        painter->drawRect(QRectF(clipped.left(), clipped.top(), clipped.left() + border, clipped.bottom() + border));
        painter->drawRect(QRectF(clipped.left() + border, clipped.top(), clipped.right() + border, clipped.top() + border));
        painter->drawRect(QRectF(clipped.right(), clipped.top() + border, clipped.right() + border, clipped.bottom()));
        painter->drawRect(QRectF(clipped.left() + border, clipped.bottom(), clipped.right() + border, clipped.bottom() + border));
        painter->restore();
    }
    if (turn_off_antialiasing)
        painter->setRenderHint(QPainter::Antialiasing, false);
}

void TextDocumentLayoutPrivate::drawFrameDecoration(QPainter *painter, QTextFrame *frame, TextFrameData *fd, const QRectF &clip, const QRectF &rect) const
{

    const QBrush bg = frame->frameFormat().background();
    if (bg != Qt::NoBrush) {
        QRectF bgRect = rect;
        bgRect.adjust((fd->leftMargin + fd->border).toReal(),
                      (fd->topMargin + fd->border).toReal(),
                      - (fd->rightMargin + fd->border).toReal(),
                      - (fd->bottomMargin + fd->border).toReal());

        QRectF gradientRect; // invalid makes it default to bgRect
        QPointF origin = bgRect.topLeft();
        if (!frame->parentFrame()) {
            bgRect = clip;
            gradientRect.setWidth(painter->device()->width());
            gradientRect.setHeight(painter->device()->height());
        }
        fillBackground(painter, bgRect, bg, origin, gradientRect);
    }
    if (fd->border != 0) {
        painter->save();
        painter->setBrush(Qt::lightGray);
        painter->setPen(Qt::NoPen);

        const qreal leftEdge = rect.left() + fd->leftMargin.toReal();
        const qreal border = fd->border.toReal();
        const qreal topMargin = fd->topMargin.toReal();
        const qreal leftMargin = fd->leftMargin.toReal();
        const qreal bottomMargin = fd->bottomMargin.toReal();
        const qreal rightMargin = fd->rightMargin.toReal();
        const qreal w = rect.width() - 2 * border - leftMargin - rightMargin;
        const qreal h = rect.height() - 2 * border - topMargin - bottomMargin;

        drawBorder(painter, QRectF(leftEdge, rect.top() + topMargin, w + border, h + border),
                   fd->effectiveTopMargin.toReal(), fd->effectiveBottomMargin.toReal(),
                   border, frame->frameFormat().borderBrush(), frame->frameFormat().borderStyle());

        painter->restore();
    }
}

void TextDocumentLayoutPrivate::drawFrame(const QPointF &offset, QPainter *painter,
                                           const QAbstractTextDocumentLayout::PaintContext &context,
                                           QTextFrame *frame) const
{
    TextFrameData *fd = data(frame);
    // #######
    if (fd->layoutDirty)
        return;
    Q_ASSERT(!fd->sizeDirty);
    Q_ASSERT(!fd->layoutDirty);

    const QPointF off = offset + fd->position.toPointF();
    if (context.clip.isValid()
        && (off.y() > context.clip.bottom() || off.y() + fd->size.height.toReal() < context.clip.top()
            || off.x() > context.clip.right() || off.x() + fd->size.width.toReal() < context.clip.left()))
        return;

//     LDEBUG << debug_indent << "drawFrame" << frame->firstPosition() << "--" << frame->lastPosition() << "at" << offset;
//     INC_INDENT;
    QTextBlock cursorBlockNeedingRepaint;
    QPointF offsetOfRepaintedCursorBlock = off;

    const QRectF frameRect(off, fd->size.toSizeF());

    drawFrameDecoration(painter, frame, fd, context.clip, frameRect);

    QTextFrameIterator it = QTextFrameIterator::findFrameBeginning(frame);

    if (frame == document->rootFrame())
        it = frameIteratorForYPosition(QFixed::fromReal(context.clip.top()));

    QList<QTextFrame *> floats;
    const int numFloats = fd->floats.count();
    floats.reserve(numFloats);
    for (int i = 0; i < numFloats; ++i)
        floats.append(fd->floats.at(i));

    drawFlow(off, painter, context, it, floats, &cursorBlockNeedingRepaint);


    if (cursorBlockNeedingRepaint.isValid()) {
        const QPen oldPen = painter->pen();
        painter->setPen(context.palette.color(QPalette::Text));
        const int cursorPos = context.cursorPosition - cursorBlockNeedingRepaint.position();
        cursorBlockNeedingRepaint.layout()->drawCursor(painter, offsetOfRepaintedCursorBlock,
                                                       cursorPos, cursorWidth);
        painter->setPen(oldPen);
    }

//     DEC_INDENT;

    return;
}

void TextDocumentLayoutPrivate::drawFlow(const QPointF &offset, QPainter *painter, const QAbstractTextDocumentLayout::PaintContext &context,
                                          QTextFrameIterator it, const QList<QTextFrame *> &floats, QTextBlock *cursorBlockNeedingRepaint) const
{
    Q_Q(const TextDocumentLayout);
    const bool inRootFrame = (!it.atEnd() && it.parentFrame() && it.parentFrame()->parentFrame() == 0);

    QVector<CheckPoint>::ConstIterator lastVisibleCheckPoint = checkPoints.end();
    if (inRootFrame && context.clip.isValid()) {
        lastVisibleCheckPoint = std::lower_bound(checkPoints.begin(), checkPoints.end(), QFixed::fromReal(context.clip.bottom()));
    }

    QTextBlock previousBlock;
    QTextFrame *previousFrame = 0;

    for (; !it.atEnd(); ++it) {
        QTextFrame *c = it.currentFrame();

        if (inRootFrame && !checkPoints.isEmpty()) {
            int currentPosInDoc;
            if (c)
                currentPosInDoc = c->firstPosition();
            else
                currentPosInDoc = it.currentBlock().position();

            // if we're past what is already laid out then we're better off
            // not trying to draw things that may not be positioned correctly yet
            if (currentPosInDoc >= checkPoints.constLast().positionInFrame)
                break;

            if (lastVisibleCheckPoint != checkPoints.end()
                && context.clip.isValid()
                && currentPosInDoc >= lastVisibleCheckPoint->positionInFrame
               )
                break;
        }

        if (c)
            drawFrame(offset, painter, context, c);
        else {
            QAbstractTextDocumentLayout::PaintContext pc = context;
            drawBlock(offset, painter, pc, it.currentBlock(), inRootFrame);
        }

        previousBlock = it.currentBlock();
        previousFrame = c;
    }

    for (int i = 0; i < floats.count(); ++i) {
        QTextFrame *frame = floats.at(i);
        if (!isFrameFromInlineObject(frame)
            || frame->frameFormat().position() == QTextFrameFormat::InFlow)
            continue;

        const int pos = frame->firstPosition() - 1;
        QTextCharFormat format = const_cast<TextDocumentLayout *>(q)->format(pos);
        QTextObjectInterface *handler = q->handlerForObject(format.objectType());
        if (handler) {
            QRectF rect = frameBoundingRectInternal(frame);
            handler->drawObject(painter, rect, document, pos, format);
        }
    }
}

void TextDocumentLayoutPrivate::drawBlock(const QPointF &offset, QPainter *painter,
                                           const QAbstractTextDocumentLayout::PaintContext &context,
                                           const QTextBlock &bl, bool inRootFrame) const
{
    const QTextLayout *tl = bl.layout();
    QRectF r = tl->boundingRect();
    r.translate(offset + tl->position());
    if (!bl.isVisible() || (context.clip.isValid() && (r.bottom() < context.clip.y() || r.top() > context.clip.bottom())))
        return;
//      LDEBUG << debug_indent << "drawBlock" << bl.position() << "at" << offset << "br" << tl->boundingRect();

    QTextBlockFormat blockFormat = bl.blockFormat();

    QBrush bg = blockFormat.background();
    if (bg != Qt::NoBrush) {
        QRectF rect = r;

        // extend the background rectangle if we're in the root frame with NoWrap,
        // as the rect of the text block will then be only the width of the text
        // instead of the full page width
        if (inRootFrame && document->pageSize().width() <= 0) {
            const TextFrameData *fd = data(document->rootFrame());
            rect.setRight((fd->size.width - fd->rightMargin).toReal());
        }

        fillBackground(painter, rect, bg, r.topLeft());
    }

    QVector<QTextLayout::FormatRange> selections;
    int blpos = bl.position();
    int bllen = bl.length();
    const QTextCharFormat *selFormat = 0;
    for (int i = 0; i < context.selections.size(); ++i) {
        const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
        const int selStart = range.cursor.selectionStart() - blpos;
        const int selEnd = range.cursor.selectionEnd() - blpos;
        if (selStart < bllen && selEnd > 0
             && selEnd > selStart) {
            QTextLayout::FormatRange o;
            o.start = selStart;
            o.length = selEnd - selStart;
            o.format = range.format;
            selections.append(o);
        } else if (! range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
                   && bl.contains(range.cursor.position())) {
            // for full width selections we don't require an actual selection, just
            // a position to specify the line. that's more convenience in usage.
            QTextLayout::FormatRange o;
            QTextLine l = tl->lineForTextPosition(range.cursor.position() - blpos);
            o.start = l.textStart();
            o.length = l.textLength();
            if (o.start + o.length == bllen - 1)
                ++o.length; // include newline
            o.format = range.format;
            selections.append(o);
       }
        if (selStart < 0 && selEnd >= 1)
            selFormat = &range.format;
    }

    QTextObject *object = document->objectForFormat(bl.blockFormat());
    if (object && object->format().toListFormat().style() != QTextListFormat::ListStyleUndefined)
        drawListItem(offset, painter, context, bl, selFormat);

    QPen oldPen = painter->pen();
    painter->setPen(context.palette.color(QPalette::Text));

    tl->draw(painter, offset, selections, context.clip.isValid() ? (context.clip & clipRect) : clipRect);

    if ((context.cursorPosition >= blpos && context.cursorPosition < blpos + bllen)
        || (context.cursorPosition < -1 && !tl->preeditAreaText().isEmpty())) {
        int cpos = context.cursorPosition;
        if (cpos < -1)
            cpos = tl->preeditAreaPosition() - (cpos + 2);
        else
            cpos -= blpos;
        tl->drawCursor(painter, offset, cpos, cursorWidth);
    }

    if (blockFormat.hasProperty(QTextFormat::BlockTrailingHorizontalRulerWidth)) {
        const qreal width = blockFormat.lengthProperty(QTextFormat::BlockTrailingHorizontalRulerWidth).value(r.width());
        painter->setPen(context.palette.color(QPalette::Dark));
        qreal y = r.bottom();
        if (bl.length() == 1)
            y = r.top() + r.height() / 2;

        const qreal middleX = r.left() + r.width() / 2;
        painter->drawLine(QLineF(middleX - width / 2, y, middleX + width / 2, y));
    }

    painter->setPen(oldPen);
}


void TextDocumentLayoutPrivate::drawListItem(const QPointF &offset, QPainter *painter,
                                              const QAbstractTextDocumentLayout::PaintContext &context,
                                              const QTextBlock &bl, const QTextCharFormat *selectionFormat) const
{
    Q_Q(const TextDocumentLayout);
    const QTextBlockFormat blockFormat = bl.blockFormat();
    const QTextCharFormat charFormat = QTextCursor(bl).charFormat();
    QFont font(charFormat.font());
    if (q->paintDevice())
        font = QFont(font, q->paintDevice());

    const QFontMetrics fontMetrics(font);
    QTextObject * const object = document->objectForFormat(blockFormat);
    const QTextListFormat lf = object->format().toListFormat();
    int style = lf.style();
    QString itemText;
    QSizeF size;

    if (blockFormat.hasProperty(QTextFormat::ListStyle))
        style = QTextListFormat::Style(blockFormat.intProperty(QTextFormat::ListStyle));

    QTextLayout *layout = bl.layout();
    if (layout->lineCount() == 0)
        return;
    QTextLine firstLine = layout->lineAt(0);
    Q_ASSERT(firstLine.isValid());
    QPointF pos = (offset + layout->position()).toPoint();
    Qt::LayoutDirection dir = bl.textDirection();
    {
        QRectF textRect = firstLine.naturalTextRect();
        pos += textRect.topLeft().toPoint();
        if (dir == Qt::RightToLeft)
            pos.rx() += textRect.width();
    }

    switch (style) {
    case QTextListFormat::ListDecimal:
    case QTextListFormat::ListLowerAlpha:
    case QTextListFormat::ListUpperAlpha:
    case QTextListFormat::ListLowerRoman:
    case QTextListFormat::ListUpperRoman:
        itemText = static_cast<QTextList *>(object)->itemText(bl);
#if (QT_VERSION >= QT_VERSION_CHECK(5,10,0))
        size.setWidth(fontMetrics.horizontalAdvance(itemText));
#else
        size.setWidth(fontMetrics.width(itemText));
#endif
        size.setHeight(fontMetrics.height());
        break;

    case QTextListFormat::ListSquare:
    case QTextListFormat::ListCircle:
    case QTextListFormat::ListDisc:
        size.setWidth(fontMetrics.lineSpacing() / 3);
        size.setHeight(size.width());
        break;

    case QTextListFormat::ListStyleUndefined:
        return;
    default: return;
    }

    QRectF r(pos, size);
#if (QT_VERSION > QT_VERSION_CHECK(5,10,0))
    qreal xoff = fontMetrics.horizontalAdvance(QLatin1Char(' '));
#else
    qreal xoff = fontMetrics.width(QLatin1Char(' '));
#endif
    if (dir == Qt::LeftToRight)
        xoff = -xoff - size.width();
    r.translate( xoff, (fontMetrics.height() / 2) - (size.height() / 2));

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    if (selectionFormat) {
        painter->setPen(QPen(selectionFormat->foreground(), 0));
        painter->fillRect(r, selectionFormat->background());
    } else {
        QBrush fg = charFormat.foreground();
        if (fg == Qt::NoBrush)
            fg = context.palette.text();
        painter->setPen(QPen(fg, 0));
    }

    QBrush brush = context.palette.brush(QPalette::Text);

    switch (style) {
    case QTextListFormat::ListDecimal:
    case QTextListFormat::ListLowerAlpha:
    case QTextListFormat::ListUpperAlpha:
    case QTextListFormat::ListLowerRoman:
    case QTextListFormat::ListUpperRoman: {
        QTextLayout layout(itemText, font, q->paintDevice());
        layout.setCacheEnabled(true);
        QTextOption option(Qt::AlignLeft | Qt::AlignAbsolute);
        option.setTextDirection(dir);
        layout.setTextOption(option);
        layout.beginLayout();
        QTextLine line = layout.createLine();
        if (line.isValid())
            line.setLeadingIncluded(true);
        layout.endLayout();
        layout.draw(painter, QPointF(r.left(), pos.y()));
        break;
    }
    case QTextListFormat::ListSquare:
        painter->fillRect(r, brush);
        break;
    case QTextListFormat::ListCircle:
        painter->setPen(QPen(brush, 0));
        painter->drawEllipse(r.translated(0.5, 0.5)); // pixel align for sharper rendering
        break;
    case QTextListFormat::ListDisc:
        painter->setBrush(brush);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(r);
        break;
    case QTextListFormat::ListStyleUndefined:
        break;
    default:
        break;
    }

    painter->restore();
}

static QFixed flowPosition(const QTextFrameIterator &it)
{
    if (it.atEnd())
        return 0;

    if (it.currentFrame()) {
        return data(it.currentFrame())->position.y;
    } else {
        QTextBlock block = it.currentBlock();
        QTextLayout *layout = block.layout();
        if (layout->lineCount() == 0)
            return QFixed::fromReal(layout->position().y());
        else
            return QFixed::fromReal(layout->position().y() + layout->lineAt(0).y());
    }
}

static QFixed firstChildPos(const QTextFrame *f)
{
    return flowPosition(QTextFrameIterator::findFrameBeginning(f));
}

void TextDocumentLayoutPrivate::positionFloat(QTextFrame *frame, QTextLine *currentLine)
{
    TextFrameData *fd = data(frame);

    QTextFrame *parent = frame->parentFrame();
    Q_ASSERT(parent);
    TextFrameData *pd = data(parent);
    Q_ASSERT(pd && pd->currentLayoutStruct);

    TextLayoutStruct *layoutStruct = pd->currentLayoutStruct;

    if (!pd->floats.contains(frame))
        pd->floats.append(frame);
    fd->layoutDirty = true;
    Q_ASSERT(!fd->sizeDirty);

    QFixed y = layoutStruct->y;
    if (currentLine) {
        QFixed left, right;
        floatMargins(y, layoutStruct, &left, &right);
        if (right - left < QFixed::fromReal(currentLine->naturalTextWidth()) + fd->size.width) {
            layoutStruct->pendingFloats.append(frame);
            return;
        }
    }

    bool frameSpansIntoNextPage = (y + layoutStruct->frameY + fd->size.height > layoutStruct->pageBottom);
    if (frameSpansIntoNextPage && fd->size.height <= layoutStruct->pageHeight) {
        layoutStruct->newPage();
        y = layoutStruct->y;

        frameSpansIntoNextPage = false;
    }

    y = findY(y, layoutStruct, fd->size.width);

    QFixed left, right;
    floatMargins(y, layoutStruct, &left, &right);

    if (frame->frameFormat().position() == QTextFrameFormat::FloatLeft) {
        fd->position.x = left;
        fd->position.y = y;
    } else {
        fd->position.x = right - fd->size.width;
        fd->position.y = y;
    }

    layoutStruct->minimumWidth = qMax(layoutStruct->minimumWidth, fd->minimumWidth);
    layoutStruct->maximumWidth = qMin(layoutStruct->maximumWidth, fd->maximumWidth);

    fd->layoutDirty = false;

}

QRectF TextDocumentLayoutPrivate::layoutFrame(QTextFrame *f, int layoutFrom, int layoutTo, QFixed parentY)
{
    LDEBUG << "layoutFrame (pre)";
    Q_ASSERT(data(f)->sizeDirty);

    QTextFrameFormat fformat = f->frameFormat();

    QTextFrame *parent = f->parentFrame();
    const TextFrameData *pd = parent ? data(parent) : 0;

    const qreal maximumWidth = qMax(qreal(0), pd ? pd->contentsWidth.toReal() : document->pageSize().width());
    QFixed width = QFixed::fromReal(fformat.width().value(maximumWidth));
    if (fformat.width().type() == QTextLength::FixedLength)
        width = scaleToDevice(width);

    const QFixed maximumHeight = pd ? pd->contentsHeight : -1;
    const QFixed height = (maximumHeight != -1 || fformat.height().type() != QTextLength::PercentageLength)
                            ? QFixed::fromReal(fformat.height().value(maximumHeight.toReal()))
                            : -1;

    return layoutFrame(f, layoutFrom, layoutTo, width, height, parentY);
}

QRectF TextDocumentLayoutPrivate::layoutFrame(QTextFrame *f, int layoutFrom, int layoutTo, QFixed frameWidth, QFixed frameHeight, QFixed parentY)
{
    LDEBUG << "layoutFrame from=" << layoutFrom << "to=" << layoutTo;
    Q_ASSERT(data(f)->sizeDirty);

    TextFrameData *fd = data(f);
    QFixed newContentsWidth;

    bool fullLayout = false;
    {
        QTextFrameFormat fformat = f->frameFormat();
        // set sizes of this frame from the format
        QFixed tm = QFixed::fromReal(fformat.topMargin());
        if (tm != fd->topMargin) {
            fd->topMargin = tm;
            fullLayout = true;
        }
        QFixed bm = QFixed::fromReal(fformat.bottomMargin());
        if (bm != fd->bottomMargin) {
            fd->bottomMargin = bm;
            fullLayout = true;
        }
        fd->leftMargin = QFixed::fromReal(fformat.leftMargin());
        fd->rightMargin = QFixed::fromReal(fformat.rightMargin());
        QFixed b = QFixed::fromReal(fformat.border());
        if (b != fd->border) {
            fd->border = b;
            fullLayout = true;
        }
        QFixed p = QFixed::fromReal(fformat.padding());
        if (p != fd->padding) {
            fd->padding = p;
            fullLayout = true;
        }

        QTextFrame *parent = f->parentFrame();
        const TextFrameData *pd = parent ? data(parent) : 0;

        // accumulate top and bottom margins
        if (parent) {
            fd->effectiveTopMargin = pd->effectiveTopMargin + fd->topMargin + fd->border + fd->padding;
            fd->effectiveBottomMargin = pd->effectiveBottomMargin + fd->topMargin + fd->border + fd->padding;
        } else {
            fd->effectiveTopMargin = fd->topMargin + fd->border + fd->padding;
            fd->effectiveBottomMargin = fd->bottomMargin + fd->border + fd->padding;
        }

        newContentsWidth = frameWidth - 2*(fd->border + fd->padding)
                           - fd->leftMargin - fd->rightMargin;

        if (frameHeight != -1) {
            fd->contentsHeight = frameHeight - 2*(fd->border + fd->padding)
                                 - fd->topMargin - fd->bottomMargin;
        } else {
            fd->contentsHeight = frameHeight;
        }
    }

    if (isFrameFromInlineObject(f)) {
        // never reached, handled in resizeInlineObject/positionFloat instead
        return QRectF();
    }

    // set fd->contentsWidth temporarily, so that layoutFrame for the children
    // picks the right width. We'll initialize it properly at the end of this
    // function.
    fd->contentsWidth = newContentsWidth;

    TextLayoutStruct layoutStruct;
    layoutStruct.frame = f;
    layoutStruct.x_left = fd->leftMargin + fd->border + fd->padding;
    layoutStruct.x_right = layoutStruct.x_left + newContentsWidth;
    layoutStruct.y = fd->topMargin + fd->border + fd->padding;
    layoutStruct.frameY = parentY + fd->position.y;
    layoutStruct.contentsWidth = 0;
    layoutStruct.minimumWidth = 0;
    layoutStruct.maximumWidth = QFIXED_MAX;
    layoutStruct.fullLayout = fullLayout || (fd->oldContentsWidth != newContentsWidth);
    layoutStruct.updateRect = QRectF(QPointF(0, 0), QSizeF(qreal(INT_MAX), qreal(INT_MAX)));
    LDEBUG << "layoutStruct: x_left" << layoutStruct.x_left << "x_right" << layoutStruct.x_right
           << "fullLayout" << layoutStruct.fullLayout;
    fd->oldContentsWidth = newContentsWidth;

    layoutStruct.pageHeight = QFixed::fromReal(document->pageSize().height());
    if (layoutStruct.pageHeight < 0)
        layoutStruct.pageHeight = QFIXED_MAX;

    const int currentPage = layoutStruct.pageHeight == 0 ? 0 : (layoutStruct.frameY / layoutStruct.pageHeight).truncate();
    layoutStruct.pageTopMargin = fd->effectiveTopMargin;
    layoutStruct.pageBottomMargin = fd->effectiveBottomMargin;
    layoutStruct.pageBottom = (currentPage + 1) * layoutStruct.pageHeight - layoutStruct.pageBottomMargin;

    if (!f->parentFrame())
        idealWidth = 0; // reset

    QTextFrameIterator it = QTextFrameIterator::findFrameBeginning(f);
    layoutFlow(it, &layoutStruct, layoutFrom, layoutTo);

    QFixed maxChildFrameWidth = 0;
    QList<QTextFrame *> children = f->childFrames();
    for (int i = 0; i < children.size(); ++i) {
        QTextFrame *c = children.at(i);
        TextFrameData *cd = data(c);
        maxChildFrameWidth = qMax(maxChildFrameWidth, cd->size.width);
    }

    const QFixed marginWidth = 2*(fd->border + fd->padding) + fd->leftMargin + fd->rightMargin;
    if (!f->parentFrame()) {
        idealWidth = qMax(maxChildFrameWidth, layoutStruct.contentsWidth).toReal();
        idealWidth += marginWidth.toReal();
    }

    QFixed actualWidth = qMax(newContentsWidth, qMax(maxChildFrameWidth, layoutStruct.contentsWidth));
    fd->contentsWidth = actualWidth;
    if (newContentsWidth <= 0) { // nowrap layout?
        fd->contentsWidth = newContentsWidth;
    }

    fd->minimumWidth = layoutStruct.minimumWidth;
    fd->maximumWidth = layoutStruct.maximumWidth;

    fd->size.height = fd->contentsHeight == -1
                 ? layoutStruct.y + fd->border + fd->padding + fd->bottomMargin
                 : fd->contentsHeight + 2*(fd->border + fd->padding) + fd->topMargin + fd->bottomMargin;
    fd->size.width = actualWidth + marginWidth;
    fd->sizeDirty = false;
    if (layoutStruct.updateRectForFloats.isValid())
        layoutStruct.updateRect |= layoutStruct.updateRectForFloats;
    return layoutStruct.updateRect;
}

void TextDocumentLayoutPrivate::layoutFlow(QTextFrameIterator it, TextLayoutStruct *layoutStruct,
                                            int layoutFrom, int layoutTo, QFixed width)
{
    LDEBUG << "layoutFlow from=" << layoutFrom << "to=" << layoutTo;
    TextFrameData *fd = data(layoutStruct->frame);

    fd->currentLayoutStruct = layoutStruct;

    QTextFrameIterator previousIt;

    const bool inRootFrame = (it.parentFrame() == document->rootFrame());
    if (inRootFrame) {
        bool redoCheckPoints = layoutStruct->fullLayout || checkPoints.isEmpty();

        if (!redoCheckPoints) {
            QVector<CheckPoint>::Iterator checkPoint = std::lower_bound(checkPoints.begin(), checkPoints.end(), layoutFrom);
            if (checkPoint != checkPoints.end()) {
                if (checkPoint != checkPoints.begin())
                    --checkPoint;

                layoutStruct->y = checkPoint->y;
                layoutStruct->frameY = checkPoint->frameY;
                layoutStruct->minimumWidth = checkPoint->minimumWidth;
                layoutStruct->maximumWidth = checkPoint->maximumWidth;
                layoutStruct->contentsWidth = checkPoint->contentsWidth;

                if (layoutStruct->pageHeight > 0) {
                    int page = layoutStruct->currentPage();
                    layoutStruct->pageBottom = (page + 1) * layoutStruct->pageHeight - layoutStruct->pageBottomMargin;
                }

                it = frameIteratorForTextPosition(checkPoint->positionInFrame);
                checkPoints.resize(checkPoint - checkPoints.begin() + 1);

                if (checkPoint != checkPoints.begin()) {
                    previousIt = it;
                    --previousIt;
                }
            } else {
                redoCheckPoints = true;
            }
        }

        if (redoCheckPoints) {
            checkPoints.clear();
            CheckPoint cp;
            cp.y = layoutStruct->y;
            cp.frameY = layoutStruct->frameY;
            cp.positionInFrame = 0;
            cp.minimumWidth = layoutStruct->minimumWidth;
            cp.maximumWidth = layoutStruct->maximumWidth;
            cp.contentsWidth = layoutStruct->contentsWidth;
            checkPoints.append(cp);
        }
    }

    QTextBlockFormat previousBlockFormat = previousIt.currentBlock().blockFormat();

    QFixed maximumBlockWidth = 0;
    while (!it.atEnd()) {
        QTextFrame *c = it.currentFrame();

        int docPos;
        if (it.currentFrame())
            docPos = it.currentFrame()->firstPosition();
        else
            docPos = it.currentBlock().position();

        if (inRootFrame) {
            if (qAbs(layoutStruct->y - checkPoints.constLast().y) > 2000) {
                QFixed left, right;
                floatMargins(layoutStruct->y, layoutStruct, &left, &right);
                if (left == layoutStruct->x_left && right == layoutStruct->x_right) {
                    CheckPoint p;
                    p.y = layoutStruct->y;
                    p.frameY = layoutStruct->frameY;
                    p.positionInFrame = docPos;
                    p.minimumWidth = layoutStruct->minimumWidth;
                    p.maximumWidth = layoutStruct->maximumWidth;
                    p.contentsWidth = layoutStruct->contentsWidth;
                    checkPoints.append(p);

                    if (currentLazyLayoutPosition != -1
                        && docPos > currentLazyLayoutPosition + lazyLayoutStepSize)
                        break;

                }
            }
        }

        if (c) {
            // position child frame
            TextFrameData *cd = data(c);

            QTextFrameFormat fformat = c->frameFormat();

            if (fformat.position() == QTextFrameFormat::InFlow) {
                if (fformat.pageBreakPolicy() & QTextFormat::PageBreak_AlwaysBefore)
                    layoutStruct->newPage();

                QFixed left, right;
                floatMargins(layoutStruct->y, layoutStruct, &left, &right);
                left = qMax(left, layoutStruct->x_left);
                right = qMin(right, layoutStruct->x_right);

                if (right - left < cd->size.width) {
                    layoutStruct->y = findY(layoutStruct->y, layoutStruct, cd->size.width);
                    floatMargins(layoutStruct->y, layoutStruct, &left, &right);
                }

                QFixedPoint pos(left, layoutStruct->y);

                Qt::Alignment align = Qt::AlignLeft;

                // detect whether we have any alignment in the document that disallows optimizations,
                // such as not laying out the document again in a textedit with wrapping disabled.
                if (inRootFrame && !(align & Qt::AlignLeft))
                    contentHasAlignment = true;

                cd->position = pos;

                if (document->pageSize().height() > 0.0f)
                    cd->sizeDirty = true;

                if (cd->sizeDirty) {
                    if (width != 0)
                        layoutFrame(c, layoutFrom, layoutTo, width, -1, layoutStruct->frameY);
                    else
                        layoutFrame(c, layoutFrom, layoutTo, layoutStruct->frameY);

                    QFixed absoluteChildPos = pos.y + firstChildPos(c);
                    absoluteChildPos += layoutStruct->frameY;

                    // drop entire frame to next page if first child of frame is on next page
                    if (absoluteChildPos > layoutStruct->pageBottom) {
                        layoutStruct->newPage();
                        pos.y = layoutStruct->y;

                        cd->position = pos;
                        cd->sizeDirty = true;

                        if (width != 0)
                            layoutFrame(c, layoutFrom, layoutTo, width, -1, layoutStruct->frameY);
                        else
                            layoutFrame(c, layoutFrom, layoutTo, layoutStruct->frameY);
                    }
                }

                // align only if there is space for alignment
                if (right - left > cd->size.width) {
                    if (align & Qt::AlignRight)
                        pos.x += layoutStruct->x_right - cd->size.width;
                    else if (align & Qt::AlignHCenter)
                        pos.x += (layoutStruct->x_right - cd->size.width) / 2;
                }

                cd->position = pos;

                layoutStruct->y += cd->size.height;
                const int page = layoutStruct->currentPage();
                layoutStruct->pageBottom = (page + 1) * layoutStruct->pageHeight - layoutStruct->pageBottomMargin;

                cd->layoutDirty = false;

                if (c->frameFormat().pageBreakPolicy() & QTextFormat::PageBreak_AlwaysAfter)
                    layoutStruct->newPage();
            } else {
                QRectF oldFrameRect(cd->position.toPointF(), cd->size.toSizeF());
                QRectF updateRect;

                if (cd->sizeDirty)
                    updateRect = layoutFrame(c, layoutFrom, layoutTo);

                positionFloat(c);

                // If the size was made dirty when the position was set, layout again
                if (cd->sizeDirty)
                    updateRect = layoutFrame(c, layoutFrom, layoutTo);

                QRectF frameRect(cd->position.toPointF(), cd->size.toSizeF());

                if (frameRect == oldFrameRect && updateRect.isValid())
                    updateRect.translate(cd->position.toPointF());
                else
                    updateRect = frameRect;

                layoutStruct->addUpdateRectForFloat(updateRect);
                if (oldFrameRect.isValid())
                    layoutStruct->addUpdateRectForFloat(oldFrameRect);
            }

            layoutStruct->minimumWidth = qMax(layoutStruct->minimumWidth, cd->minimumWidth);
            layoutStruct->maximumWidth = qMin(layoutStruct->maximumWidth, cd->maximumWidth);

            previousIt = it;
            ++it;
        } else {
            QTextFrameIterator lastIt;
            if (!previousIt.atEnd() && previousIt != it)
                lastIt = previousIt;
            previousIt = it;
            QTextBlock block = it.currentBlock();
            ++it;

            const QTextBlockFormat blockFormat = block.blockFormat();

            if (blockFormat.pageBreakPolicy() & QTextFormat::PageBreak_AlwaysBefore)
                layoutStruct->newPage();

            const QFixed origY = layoutStruct->y;
            const QFixed origPageBottom = layoutStruct->pageBottom;
            const QFixed origMaximumWidth = layoutStruct->maximumWidth;
            layoutStruct->maximumWidth = 0;

            const QTextBlockFormat *previousBlockFormatPtr = 0;
            if (lastIt.currentBlock().isValid())
                previousBlockFormatPtr = &previousBlockFormat;

            // layout and position child block
            layoutBlock(block, docPos, blockFormat, layoutStruct, layoutFrom, layoutTo, previousBlockFormatPtr);

            // detect whether we have any alignment in the document that disallows optimizations,
            // such as not laying out the document again in a textedit with wrapping disabled.
            if (inRootFrame && !(block.layout()->textOption().alignment() & Qt::AlignLeft))
                contentHasAlignment = true;

            if (blockFormat.pageBreakPolicy() & QTextFormat::PageBreak_AlwaysAfter)
                layoutStruct->newPage();


            maximumBlockWidth = qMax(maximumBlockWidth, layoutStruct->maximumWidth);
            layoutStruct->maximumWidth = origMaximumWidth;
            previousBlockFormat = blockFormat;
        }
    }
    if (layoutStruct->maximumWidth == QFIXED_MAX && maximumBlockWidth > 0)
        layoutStruct->maximumWidth = maximumBlockWidth;
    else
        layoutStruct->maximumWidth = qMax(layoutStruct->maximumWidth, maximumBlockWidth);

    if (inRootFrame) {
        // we assume that any float is aligned in a way that disallows the optimizations that rely
        // on unaligned content.
        if (!fd->floats.isEmpty())
            contentHasAlignment = true;

        if (it.atEnd()) {
            currentLazyLayoutPosition = -1;
            CheckPoint cp;
            cp.y = layoutStruct->y;
            cp.positionInFrame = docPrivate->length();
            cp.minimumWidth = layoutStruct->minimumWidth;
            cp.maximumWidth = layoutStruct->maximumWidth;
            cp.contentsWidth = layoutStruct->contentsWidth;
            checkPoints.append(cp);
            checkPoints.reserve(checkPoints.size());
        } else {
            currentLazyLayoutPosition = checkPoints.constLast().positionInFrame;
            // #######
            //checkPoints.last().positionInFrame = q->document()->docHandle()->length();
        }
    }


    fd->currentLayoutStruct = 0;
}

static inline void getLineHeightParams(const QTextBlockFormat &blockFormat, const QTextLine &line, qreal scaling,
                                       QFixed *lineAdjustment, QFixed *lineBreakHeight, QFixed *lineHeight)
{
    *lineHeight = QFixed::fromReal(blockFormat.lineHeight(line.height(), scaling));

    if (blockFormat.lineHeightType() == QTextBlockFormat::FixedHeight || blockFormat.lineHeightType() == QTextBlockFormat::MinimumHeight) {
        *lineBreakHeight = *lineHeight;
        if (blockFormat.lineHeightType() == QTextBlockFormat::FixedHeight)
            *lineAdjustment = QFixed::fromReal(line.ascent() + qMax(line.leading(), qreal(0.0))) - ((*lineHeight * 4) / 5);
        else
            *lineAdjustment = QFixed::fromReal(line.height()) - *lineHeight;
    }
    else {
        *lineBreakHeight = QFixed::fromReal(line.height());
        *lineAdjustment = 0;
    }
}

void TextDocumentLayoutPrivate::layoutBlock(const QTextBlock &bl, int blockPosition, const QTextBlockFormat &blockFormat,
                                             TextLayoutStruct *layoutStruct, int layoutFrom, int layoutTo, const QTextBlockFormat *previousBlockFormat)
{
    Q_Q(TextDocumentLayout);
    if (!bl.isVisible())
        return;

    QTextLayout *tl = bl.layout();
    const int blockLength = bl.length();

    LDEBUG << "layoutBlock from=" << layoutFrom << "to=" << layoutTo;


    if (previousBlockFormat) {
        qreal margin = qMax(blockFormat.topMargin(), previousBlockFormat->bottomMargin());
        if (margin > 0 && q->paintDevice()) {
            margin *= qreal(q->paintDevice()->logicalDpiY()) / qreal(qt_defaultDpi());
        }
        layoutStruct->y += QFixed::fromReal(margin);
    }

    //TextFrameData *fd = data(layoutStruct->frame);

    Qt::LayoutDirection dir = bl.textDirection();

    QFixed extraMargin;
    if (docPrivate->defaultTextOption.flags() & QTextOption::AddSpaceForLineAndParagraphSeparators) {
        QFontMetricsF fm(bl.charFormat().font());
#if (QT_VERSION > QT_VERSION_CHECK(5,10,0))
        extraMargin = QFixed::fromReal(fm.horizontalAdvance(QChar(QChar(0x21B5))));
#else
        extraMargin = QFixed::fromReal(fm.width(QChar(QChar(0x21B5))));
#endif
    }

    const QFixed indent = this->blockIndent(blockFormat);
    const QFixed totalLeftMargin = QFixed::fromReal(blockFormat.leftMargin()) + (dir == Qt::RightToLeft ? extraMargin : indent);
    const QFixed totalRightMargin = QFixed::fromReal(blockFormat.rightMargin()) + (dir == Qt::RightToLeft ? indent : extraMargin);

    const QPointF oldPosition = tl->position();
    tl->setPosition(QPointF(layoutStruct->x_left.toReal(), layoutStruct->y.toReal()));

    if (layoutStruct->fullLayout
        || (blockPosition + blockLength > layoutFrom && blockPosition <= layoutTo)
        // force relayout if we cross a page boundary
        || (layoutStruct->pageHeight != QFIXED_MAX && layoutStruct->absoluteY() + QFixed::fromReal(tl->boundingRect().height()) > layoutStruct->pageBottom)) {

        LDEBUG << " do layout";
        QTextOption option = docPrivate->defaultTextOption;
        option.setTextDirection(dir);
        option.setTabs( blockFormat.tabPositions() );

        Qt::Alignment align = docPrivate->defaultTextOption.alignment();
        if (blockFormat.hasProperty(QTextFormat::BlockAlignment))
            align = blockFormat.alignment();

        Qt::Alignment alres = align;
        if (!(alres & Qt::AlignHorizontal_Mask))
            alres |= Qt::AlignLeft;
        if (!(alres & Qt::AlignAbsolute) && (alres & (Qt::AlignLeft | Qt::AlignRight))) {
            if (dir == Qt::RightToLeft)
                alres ^= (Qt::AlignLeft | Qt::AlignRight);
            alres |= Qt::AlignAbsolute;
        }

        option.setAlignment(alres); // for paragraph that are RTL, alignment is auto-reversed;

        if (blockFormat.nonBreakableLines() || document->pageSize().width() < 0) {
            option.setWrapMode(QTextOption::ManualWrap);
        }

        tl->setTextOption(option);

        const bool haveWordOrAnyWrapMode = (option.wrapMode() == QTextOption::WrapAtWordBoundaryOrAnywhere);

        const QFixed cy = layoutStruct->y;
        const QFixed l = layoutStruct->x_left  + totalLeftMargin;
        const QFixed r = layoutStruct->x_right - totalRightMargin;

        tl->beginLayout();
        bool firstLine = true;
        while (1) {
            QTextLine line = tl->createLine();
            if (!line.isValid())
                break;
            line.setLeadingIncluded(true);

            QFixed left, right;
            floatMargins(layoutStruct->y, layoutStruct, &left, &right);
            left = qMax(left, l);
            right = qMin(right, r);
            QFixed text_indent;
            if (firstLine) {
                text_indent = QFixed::fromReal(blockFormat.textIndent());
                if (dir == Qt::LeftToRight)
                    left += text_indent;
                else
                    right -= text_indent;
                firstLine = false;
            }

            if (fixedColumnWidth != -1)
                line.setNumColumns(fixedColumnWidth, (right - left).toReal());
            else
                line.setLineWidth((right - left).toReal());

            floatMargins(layoutStruct->y, layoutStruct, &left, &right);
            left = qMax(left, l);
            right = qMin(right, r);
            if (dir == Qt::LeftToRight)
                left += text_indent;
            else
                right -= text_indent;

            if (fixedColumnWidth == -1 && QFixed::fromReal(line.naturalTextWidth()) > right-left) {
                // float has been added in the meantime, redo
                layoutStruct->pendingFloats.clear();

                line.setLineWidth((right-left).toReal());
                if (QFixed::fromReal(line.naturalTextWidth()) > right-left) {
                    if (haveWordOrAnyWrapMode) {
                        option.setWrapMode(QTextOption::WrapAnywhere);
                        tl->setTextOption(option);
                    }

                    layoutStruct->pendingFloats.clear();
                    // lines min width more than what we have
                    layoutStruct->y = findY(layoutStruct->y, layoutStruct, QFixed::fromReal(line.naturalTextWidth()));
                    floatMargins(layoutStruct->y, layoutStruct, &left, &right);
                    left = qMax(left, l);
                    right = qMin(right, r);
                    if (dir == Qt::LeftToRight)
                        left += text_indent;
                    else
                        right -= text_indent;
                    line.setLineWidth(qMax<qreal>(line.naturalTextWidth(), (right-left).toReal()));

                    if (haveWordOrAnyWrapMode) {
                        option.setWrapMode(QTextOption::WordWrap);
                        tl->setTextOption(option);
                    }
                }

            }

            QFixed lineBreakHeight, lineHeight, lineAdjustment;
            qreal scaling = (q->paintDevice() && q->paintDevice()->logicalDpiY() != qt_defaultDpi()) ?
                            qreal(q->paintDevice()->logicalDpiY()) / qreal(qt_defaultDpi()) : 1;
            getLineHeightParams(blockFormat, line, scaling, &lineAdjustment, &lineBreakHeight, &lineHeight);

            if (layoutStruct->pageHeight > 0 && layoutStruct->absoluteY() + lineBreakHeight > layoutStruct->pageBottom) {
                layoutStruct->newPage();

                floatMargins(layoutStruct->y, layoutStruct, &left, &right);
                left = qMax(left, l);
                right = qMin(right, r);
                if (dir == Qt::LeftToRight)
                    left += text_indent;
                else
                    right -= text_indent;
            }

            line.setPosition(QPointF((left - layoutStruct->x_left).toReal(), (layoutStruct->y - cy - lineAdjustment).toReal()));
            layoutStruct->y += lineHeight;
            layoutStruct->contentsWidth
                = qMax<QFixed>(layoutStruct->contentsWidth, QFixed::fromReal(line.x() + line.naturalTextWidth()) + totalRightMargin);

            // position floats
            for (int i = 0; i < layoutStruct->pendingFloats.size(); ++i) {
                QTextFrame *f = layoutStruct->pendingFloats.at(i);
                positionFloat(f);
            }
            layoutStruct->pendingFloats.clear();
        }
        tl->endLayout();
    } else {
        const int cnt = tl->lineCount();
        for (int i = 0; i < cnt; ++i) {
            LDEBUG << "going to move text line" << i;
            QTextLine line = tl->lineAt(i);
            layoutStruct->contentsWidth
                = qMax(layoutStruct->contentsWidth, QFixed::fromReal(line.x() + tl->lineAt(i).naturalTextWidth()) + totalRightMargin);

            QFixed lineBreakHeight, lineHeight, lineAdjustment;
            qreal scaling = (q->paintDevice() && q->paintDevice()->logicalDpiY() != qt_defaultDpi()) ?
                            qreal(q->paintDevice()->logicalDpiY()) / qreal(qt_defaultDpi()) : 1;
            getLineHeightParams(blockFormat, line, scaling, &lineAdjustment, &lineBreakHeight, &lineHeight);

            if (layoutStruct->pageHeight != QFIXED_MAX) {
                if (layoutStruct->absoluteY() + lineBreakHeight > layoutStruct->pageBottom)
                    layoutStruct->newPage();
                line.setPosition(QPointF(line.position().x(), (layoutStruct->y - lineAdjustment).toReal() - tl->position().y()));
            }
            layoutStruct->y += lineHeight;
        }
        if (layoutStruct->updateRect.isValid()
            && blockLength > 1) {
            if (layoutFrom >= blockPosition + blockLength) {
                // if our height didn't change and the change in the document is
                // in one of the later paragraphs, then we don't need to repaint
                // this one
                layoutStruct->updateRect.setTop(qMax(layoutStruct->updateRect.top(), layoutStruct->y.toReal()));
            } else if (layoutTo < blockPosition) {
                if (oldPosition == tl->position())
                    // if the change in the document happened earlier in the document
                    // and our position did /not/ change because none of the earlier paragraphs
                    // or frames changed their height, then we don't need to repaint
                    // this one
                    layoutStruct->updateRect.setBottom(qMin(layoutStruct->updateRect.bottom(), tl->position().y()));
                else
                    layoutStruct->updateRect.setBottom(qreal(INT_MAX)); // reset
            }
        }
    }

    // ### doesn't take floats into account. would need to do it per line. but how to retrieve then? (Simon)
    const QFixed margins = totalLeftMargin + totalRightMargin;
    layoutStruct->minimumWidth = qMax(layoutStruct->minimumWidth, QFixed::fromReal(tl->minimumWidth()) + margins);

    const QFixed maxW = QFixed::fromReal(tl->maximumWidth()) + margins;

    if (maxW > 0) {
        if (layoutStruct->maximumWidth == QFIXED_MAX)
            layoutStruct->maximumWidth = maxW;
        else
            layoutStruct->maximumWidth = qMax(layoutStruct->maximumWidth, maxW);
    }
}

void TextDocumentLayoutPrivate::floatMargins(const QFixed &y, const TextLayoutStruct *layoutStruct,
                                              QFixed *left, QFixed *right) const
{
    *left = layoutStruct->x_left;
    *right = layoutStruct->x_right;
    TextFrameData *lfd = data(layoutStruct->frame);
    for (int i = 0; i < lfd->floats.size(); ++i) {
        TextFrameData *fd = data(lfd->floats.at(i));
        if (!fd->layoutDirty) {
            if (fd->position.y <= y && fd->position.y + fd->size.height > y) {
                if (lfd->floats.at(i)->frameFormat().position() == QTextFrameFormat::FloatLeft)
                    *left = qMax(*left, fd->position.x + fd->size.width);
                else
                    *right = qMin(*right, fd->position.x);
            }
        }
    }
}

QFixed TextDocumentLayoutPrivate::findY(QFixed yFrom, const TextLayoutStruct *layoutStruct, QFixed requiredWidth) const
{
    QFixed right, left;
    requiredWidth = qMin(requiredWidth, layoutStruct->x_right - layoutStruct->x_left);

    while (1) {
        floatMargins(yFrom, layoutStruct, &left, &right);
        if (right-left >= requiredWidth)
            break;

        // move float down until we find enough space
        QFixed newY = QFIXED_MAX;
        TextFrameData *lfd = data(layoutStruct->frame);
        for (int i = 0; i < lfd->floats.size(); ++i) {
            TextFrameData *fd = data(lfd->floats.at(i));
            if (!fd->layoutDirty) {
                if (fd->position.y <= yFrom && fd->position.y + fd->size.height > yFrom)
                    newY = qMin(newY, fd->position.y + fd->size.height);
            }
        }
        if (newY == QFIXED_MAX)
            break;
        yFrom = newY;
    }
    return yFrom;
}

TextDocumentLayout::TextDocumentLayout(QTextDocument *doc)
    : QAbstractTextDocumentLayout(*new TextDocumentLayoutPrivate, doc)
{
    Q_D(TextDocumentLayout);
    d->lineManager = new LineManager(this);
}


void TextDocumentLayout::draw(QPainter *painter, const PaintContext &context)
{
    Q_D(TextDocumentLayout);
    QTextFrame *frame = d->document->rootFrame();
    TextFrameData *fd = data(frame);

    if(fd->sizeDirty)
        return;

    if (context.clip.isValid()) {
        d->ensureLayouted(QFixed::fromReal(context.clip.bottom()));
    } else {
        d->ensureLayoutFinished();
    }

    QFixed width = fd->size.width;
    if (d->document->pageSize().width() == 0 && d->viewportRect.isValid()) {
        // we're in NoWrap mode, meaning the frame should expand to the viewport
        // so that backgrounds are drawn correctly
        fd->size.width = qMax(width, QFixed::fromReal(d->viewportRect.right()));
    }

    // Make sure we conform to the root frames bounds when drawing.
    d->clipRect = QRectF(fd->position.toPointF(), fd->size.toSizeF()).adjusted(fd->leftMargin.toReal(), 0, -fd->rightMargin.toReal(), 0);
    d->drawFrame(QPointF(), painter, context, frame);
    fd->size.width = width;
}

void TextDocumentLayout::setViewport(const QRectF &viewport)
{
    Q_D(TextDocumentLayout);
    d->viewportRect = viewport;
}

static void markFrames(QTextFrame *current, int from, int oldLength, int length)
{
    int end = qMax(oldLength, length) + from;

    if (current->firstPosition() >= end || current->lastPosition() < from)
        return;

    TextFrameData *fd = data(current);
    // float got removed in editing operation
    QTextFrame *null = nullptr; // work-around for (at least) MSVC 2012 emitting
                                // warning C4100 for its own header <algorithm>
                                // when passing nullptr directly to std::remove
    fd->floats.erase(std::remove(fd->floats.begin(), fd->floats.end(), null),
                     fd->floats.end());

    fd->layoutDirty = true;
    fd->sizeDirty = true;

    QList<QTextFrame *> children = current->childFrames();
    for (int i = 0; i < children.size(); ++i)
        markFrames(children.at(i), from, oldLength, length);
}

void TextDocumentLayout::documentChanged(int from, int oldLength, int length)
{
    Q_D(TextDocumentLayout);

    QTextBlock startIt = document()->findBlock(from);
    QTextBlock endIt = document()->findBlock(qMax(0, from + length - 1));
    if (endIt.isValid())
        endIt = endIt.next();
    for (QTextBlock blockIt = startIt; blockIt.isValid() && blockIt != endIt; blockIt = blockIt.next())
        blockIt.clearLayout();

    if (d->docPrivate->pageSize.isNull())
        return;

    QRectF updateRect;

    d->lazyLayoutStepSize = 1000;
    d->sizeChangedTimer.stop();
    d->insideDocumentChange = true;

    const int documentLength = d->docPrivate->length();
    const bool fullLayout = (oldLength == 0 && length == documentLength);
    const bool smallChange = documentLength > 0
                             && (qMax(length, oldLength) * 100 / documentLength) < 5;

    // don't show incremental layout progress (avoid scroll bar flicker)
    // if we see only a small change in the document and we're either starting
    // a layout run or we're already in progress for that and we haven't seen
    // any bigger change previously (showLayoutProgress already false)
    if (smallChange
        && (d->currentLazyLayoutPosition == -1 || d->showLayoutProgress == false))
        d->showLayoutProgress = false;
    else
        d->showLayoutProgress = true;

    if (fullLayout) {
        d->contentHasAlignment = false;
        d->currentLazyLayoutPosition = 0;
        d->checkPoints.clear();
        d->layoutStep();
    } else {
        d->ensureLayoutedByPosition(from);
        updateRect = doLayout(from, oldLength, length);
    }

    if (!d->layoutTimer.isActive() && d->currentLazyLayoutPosition != -1)
        d->layoutTimer.start(10, this);

    d->insideDocumentChange = false;

    for (QTextBlock blockIt = startIt; blockIt.isValid() && blockIt != endIt; blockIt = blockIt.next())
        emit updateBlock(blockIt);

    if (d->showLayoutProgress) {
        const QSizeF newSize = dynamicDocumentSize();
        if (newSize != d->lastReportedSize) {
            d->lastReportedSize = newSize;
            emit documentSizeChanged(newSize);
        }
    }

    if (!updateRect.isValid()) {
        // don't use the frame size, it might have shrunken
        updateRect = QRectF(QPointF(0, 0), QSizeF(qreal(INT_MAX), qreal(INT_MAX)));
    }

    emit update(updateRect);
}

QRectF TextDocumentLayout::doLayout(int from, int oldLength, int length)
{
    Q_D(TextDocumentLayout);


    // mark all frames between f_start and f_end as dirty
    markFrames(d->document->rootFrame(), from, oldLength, length);

    QRectF updateRect;

    QTextFrame *root = d->document->rootFrame();
    if(data(root)->sizeDirty)
        updateRect = d->layoutFrame(root, from, from + length);
    data(root)->layoutDirty = false;

    if (d->currentLazyLayoutPosition == -1)
        layoutFinished();
    else if (d->showLayoutProgress)
        d->sizeChangedTimer.start(0, this);

    return updateRect;
}

int TextDocumentLayout::hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const
{
    Q_D(const TextDocumentLayout);
    d->ensureLayouted(QFixed::fromReal(point.y()));
    QTextFrame *f = d->document->rootFrame();
    int position = 0;
    QTextLayout *l = 0;
    QFixedPoint pointf;
    pointf.x = QFixed::fromReal(point.x());
    pointf.y = QFixed::fromReal(point.y());
    TextDocumentLayoutPrivate::HitPoint p = d->hitTest(f, pointf, &position, &l, accuracy);
    if (accuracy == Qt::ExactHit && p < TextDocumentLayoutPrivate::PointExact)
        return -1;

    // ensure we stay within document bounds
    int lastPos = f->lastPosition();
    if (l && !l->preeditAreaText().isEmpty())
        lastPos += l->preeditAreaText().length();
    if (position > lastPos)
        position = lastPos;
    else if (position < 0)
        position = 0;

    return position;
}

void TextDocumentLayout::resizeInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format)
{
    Q_D(TextDocumentLayout);
    QTextCharFormat f = format.toCharFormat();
    Q_ASSERT(f.isValid());
    QTextObjectHandler handler = d->handlers.value(f.objectType());
    if (!handler.component)
        return;

    QSizeF intrinsic = handler.iface->intrinsicSize(d->document, posInDocument, format);

    QTextFrameFormat::Position pos = QTextFrameFormat::InFlow;
    QTextFrame *frame = qobject_cast<QTextFrame *>(d->document->objectForFormat(f));
    if (frame) {
        pos = frame->frameFormat().position();
        TextFrameData *fd = data(frame);
        fd->sizeDirty = false;
        fd->size = QFixedSize::fromSizeF(intrinsic);
        fd->minimumWidth = fd->maximumWidth = fd->size.width;
    }

    QSizeF inlineSize = (pos == QTextFrameFormat::InFlow ? intrinsic : QSizeF(0, 0));
    item.setWidth(inlineSize.width());

    QFontMetrics m(f.font());
    switch (f.verticalAlignment())
    {
    case QTextCharFormat::AlignMiddle:
        item.setDescent(inlineSize.height() / 2);
        item.setAscent(inlineSize.height() / 2);
        break;
    case QTextCharFormat::AlignBaseline:
        item.setDescent(m.descent());
        item.setAscent(inlineSize.height() - m.descent());
        break;
    default:
        item.setDescent(0);
        item.setAscent(inlineSize.height());
    }
}

void TextDocumentLayout::positionInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format)
{
    Q_D(TextDocumentLayout);
    Q_UNUSED(posInDocument);
    if (item.width() != 0)
        // inline
        return;

    QTextCharFormat f = format.toCharFormat();
    Q_ASSERT(f.isValid());
    QTextObjectHandler handler = d->handlers.value(f.objectType());
    if (!handler.component)
        return;

    QTextFrame *frame = qobject_cast<QTextFrame *>(d->document->objectForFormat(f));
    if (!frame)
        return;

    QTextBlock b = d->document->findBlock(frame->firstPosition());
    QTextLine line;
    if (b.position() <= frame->firstPosition() && b.position() + b.length() > frame->lastPosition())
        line = b.layout()->lineAt(b.layout()->lineCount()-1);

    d->positionFloat(frame, line.isValid() ? &line : 0);
}

void TextDocumentLayout::drawInlineObject(QPainter *p, const QRectF &rect, QTextInlineObject item,
                                           int posInDocument, const QTextFormat &format)
{
    Q_D(TextDocumentLayout);
    QTextCharFormat f = format.toCharFormat();
    Q_ASSERT(f.isValid());
    QTextFrame *frame = qobject_cast<QTextFrame *>(d->document->objectForFormat(f));
    if (frame && frame->frameFormat().position() != QTextFrameFormat::InFlow)
        return; // don't draw floating frames from inline objects here but in drawFlow instead

    QAbstractTextDocumentLayout::drawInlineObject(p, rect, item, posInDocument, format);
}

int TextDocumentLayout::dynamicPageCount() const
{
    Q_D(const TextDocumentLayout);
    const QSizeF pgSize = d->document->pageSize();
    if (pgSize.height() < 0)
        return 1;
    return qCeil(dynamicDocumentSize().height() / pgSize.height());
}

QSizeF TextDocumentLayout::dynamicDocumentSize() const
{
    Q_D(const TextDocumentLayout);
    return data(d->document->rootFrame())->size.toSizeF();
}

int TextDocumentLayout::pageCount() const
{
    Q_D(const TextDocumentLayout);
    d->ensureLayoutFinished();
    return dynamicPageCount();
}

QSizeF TextDocumentLayout::documentSize() const
{
    Q_D(const TextDocumentLayout);
    d->ensureLayoutFinished();
    return dynamicDocumentSize();
}

void TextDocumentLayoutPrivate::ensureLayouted(QFixed y) const
{
    Q_Q(const TextDocumentLayout);
    if (currentLazyLayoutPosition == -1)
        return;
    const QSizeF oldSize = q->dynamicDocumentSize();
    Q_UNUSED(oldSize);

    if (checkPoints.isEmpty())
        layoutStep();

    while (currentLazyLayoutPosition != -1
           && checkPoints.last().y < y)
        layoutStep();
}

void TextDocumentLayoutPrivate::ensureLayoutedByPosition(int position) const
{
    if (currentLazyLayoutPosition == -1)
        return;
    if (position < currentLazyLayoutPosition)
        return;
    while (currentLazyLayoutPosition != -1
           && currentLazyLayoutPosition < position) {
        const_cast<TextDocumentLayout *>(q_func())->doLayout(currentLazyLayoutPosition, 0, INT_MAX - currentLazyLayoutPosition);
    }
}

void TextDocumentLayoutPrivate::layoutStep() const
{
    ensureLayoutedByPosition(currentLazyLayoutPosition + lazyLayoutStepSize);
    lazyLayoutStepSize = qMin(200000, lazyLayoutStepSize * 2);
}

void TextDocumentLayout::setCursorWidth(int width)
{
    Q_D(TextDocumentLayout);
    d->cursorWidth = width;
}

int TextDocumentLayout::cursorWidth() const
{
    Q_D(const TextDocumentLayout);
    return d->cursorWidth;
}

void TextDocumentLayout::setFixedColumnWidth(int width)
{
    Q_D(TextDocumentLayout);
    d->fixedColumnWidth = width;
}

QRectF TextDocumentLayout::frameBoundingRect(QTextFrame *frame) const
{
    Q_D(const TextDocumentLayout);
    if (d->docPrivate->pageSize.isNull())
        return QRectF();
    d->ensureLayoutFinished();
    return d->frameBoundingRectInternal(frame);
}

QRectF TextDocumentLayoutPrivate::frameBoundingRectInternal(QTextFrame *frame) const
{
    QPointF pos;
    const int framePos = frame->firstPosition();
    QTextFrame *f = frame;
    while (f) {
        TextFrameData *fd = data(f);
        pos += fd->position.toPointF();

        f = f->parentFrame();
    }
    return QRectF(pos, data(frame)->size.toSizeF());
}

QRectF TextDocumentLayout::blockBoundingRect(const QTextBlock &block) const
{
    Q_D(const TextDocumentLayout);
    if (d->docPrivate->pageSize.isNull() || !block.isValid() || !block.isVisible())
        return QRectF();
    d->ensureLayoutedByPosition(block.position() + block.length());
    QTextFrame *frame = d->document->frameAt(block.position());
    QPointF offset;

    while (frame) {
        TextFrameData *fd = data(frame);
        offset += fd->position.toPointF();

        frame = frame->parentFrame();
    }

    const QTextLayout *layout = block.layout();
    QRectF rect = layout->boundingRect();
    rect.moveTopLeft(layout->position() + offset);
    return rect;
}

int TextDocumentLayout::layoutStatus() const
{
    Q_D(const TextDocumentLayout);
    int pos = d->currentLazyLayoutPosition;
    if (pos == -1)
        return 100;
    return pos * 100 / d->document->docHandle()->length();
}

void TextDocumentLayout::timerEvent(QTimerEvent *e)
{
    Q_D(TextDocumentLayout);
    if (e->timerId() == d->layoutTimer.timerId()) {
        if (d->currentLazyLayoutPosition != -1)
            d->layoutStep();
    } else if (e->timerId() == d->sizeChangedTimer.timerId()) {
        d->lastReportedSize = dynamicDocumentSize();
        emit documentSizeChanged(d->lastReportedSize);
        d->sizeChangedTimer.stop();

        if (d->currentLazyLayoutPosition == -1) {
            const int newCount = dynamicPageCount();
            if (newCount != d->lastPageCount) {
                d->lastPageCount = newCount;
                emit pageCountChanged(newCount);
            }
        }
    } else {
        QAbstractTextDocumentLayout::timerEvent(e);
    }
}

void TextDocumentLayout::layoutFinished()
{
    Q_D(TextDocumentLayout);
    d->layoutTimer.stop();
    if (!d->insideDocumentChange)
        d->sizeChangedTimer.start(0, this);
    // reset
    d->showLayoutProgress = true;
}

void TextDocumentLayout::ensureLayouted(qreal y)
{
    d_func()->ensureLayouted(QFixed::fromReal(y));
}

qreal TextDocumentLayout::idealWidth() const
{
    Q_D(const TextDocumentLayout);
    d->ensureLayoutFinished();
    return d->idealWidth;
}

bool TextDocumentLayout::contentHasAlignment() const
{
    Q_D(const TextDocumentLayout);
    return d->contentHasAlignment;
}

LineManager *TextDocumentLayout::getLineManager()
{
    Q_D(TextDocumentLayout);
    return d->lineManager;
}

void TextDocumentLayout::stateChangeUpdate(int pos)
{
    Q_D(TextDocumentLayout);
    d->lineManager->setDirtyPos(pos);
    d->lineManager->textDocumentFinishedUpdating(this->document()->blockCount());
}

QTextDocument *TextDocumentLayout::lineDocument()
{
    Q_D(TextDocumentLayout);
    return d->lineManager->m_lineDocument;
}

void TextDocumentLayout::collapseLines(int pos, int len)
{
    Q_D(TextDocumentLayout);
    d->lineManager->collapseLines(pos,len);
    emit linesCollapsed(pos, len);
}

void TextDocumentLayout::expandLines(int pos, int len)
{
    Q_D(TextDocumentLayout);
    d->lineManager->expandLines(pos, len);
    emit linesExpanded(pos, len);
}

std::pair<int,int> TextDocumentLayout::isFirstLineOfCollapsedSection(int lineNumber)
{
    Q_D(TextDocumentLayout);
    return d->lineManager->isFirstLineOfCollapsedSection(lineNumber);
}

std::pair<int,int> TextDocumentLayout::isLineAfterCollapsedSection(int lineNumber)
{
    Q_D(TextDocumentLayout);
    return d->lineManager->isLineAfterCollapsedSection(lineNumber);
}

QTextDocument *TextDocumentLayout::lineManagerParentDocument()
{
    Q_D(TextDocumentLayout);
    return d->lineManager->m_parentDocument;
}

void TextDocumentLayout::setLineManagerParentDocument(QTextDocument *doc)
{
    Q_D(TextDocumentLayout);
    d->lineManager->m_parentDocument = doc;
}

void TextDocumentLayout::setLineDocumentFont(const QFont& font)
{
    Q_D(TextDocumentLayout);
    d->lineManager->setLineDocumentFont(font);
}

void TextDocumentLayout::setDirtyPos(int pos)
{
    Q_D(TextDocumentLayout);
    d->lineManager->setDirtyPos(pos);
}

void TextDocumentLayout::textDocumentFinishedUpdating(int newLineNumber)
{
    Q_D(TextDocumentLayout);
    d->lineManager->textDocumentFinishedUpdating(newLineNumber);
}

void TextDocumentLayout::updateLineSurface(int oldLineNum, int newLineNum, int dirtyPos)
{
    emit updateLineSurfaceSignal(oldLineNum, newLineNum, dirtyPos);
}

qreal TextDocumentLayoutPrivate::scaleToDevice(qreal value) const
{
    if (!paintDevice)
        return value;
    return value * paintDevice->logicalDpiY() / qreal(qt_defaultDpi());
}

QFixed TextDocumentLayoutPrivate::scaleToDevice(QFixed value) const
{
    if (!paintDevice)
        return value;
    return value * QFixed(paintDevice->logicalDpiY()) / QFixed(qt_defaultDpi());
}

}

#include "moc_textdocumentlayout.cpp"
