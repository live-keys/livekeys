#include "linesurface.h"
#include "linesurface_p.h"
#include "textcontrol_p.h"
#include "qquickwindow.h"
#include "textnodeengine_p.h"
#include "textutil_p.h"

#include <QtCore/qmath.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextobject.h>
#include <QtGui/qtexttable.h>
#include <QtQml/qqmlinfo.h>
#include <QtQuick/qsgsimplerectnode.h>
#include <QTimer>

#include "private/qquicktextnode_p.h"
#include "private/qquickevents_p_p.h"

#include "private/qqmlproperty_p.h"

#include "private/qtextengine_p.h"
#include "private/qsgadaptationlayer_p.h"

#include <algorithm>


namespace lv {

static const int nodeBreakingSize = 300;

namespace {
    class ProtectedLayoutAccessor: public QAbstractTextDocumentLayout
    {
    public:
        inline QTextCharFormat formatAccessor(int pos)
        {
            return format(pos);
        }
    };

    class RootNode : public QSGTransformNode
    {
    public:
        RootNode() : cursorNode(nullptr), frameDecorationsNode(nullptr)
        { }

        void resetFrameDecorations(TextNode* newNode)
        {
            if (frameDecorationsNode) {
                removeChildNode(frameDecorationsNode);
                delete frameDecorationsNode;
            }
            frameDecorationsNode = newNode;
            newNode->setFlag(QSGNode::OwnedByParent);
        }

        void resetCursorNode(QSGRectangleNode* newNode)
        {
            if (cursorNode)
                removeChildNode(cursorNode);
            delete cursorNode;
            cursorNode = newNode;
            if (cursorNode) {
                appendChildNode(cursorNode);
                cursorNode->setFlag(QSGNode::OwnedByParent);
            }
        }

        QSGRectangleNode *cursorNode;
        TextNode* frameDecorationsNode;

    };
}

LineSurface::LineSurface(QQuickImplicitSizeItem *parent)
: QQuickImplicitSizeItem(*(new LineSurfacePrivate), parent)
{
    /*Q_D(LineSurface);
    d->init();*/
}

LineSurface::LineSurface(LineSurfacePrivate &dd, QQuickImplicitSizeItem *parent)
: QQuickImplicitSizeItem(dd, parent)
{
    /*Q_D(LineSurface);
    d->init();*/
}

QString LineSurface::text() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QString();
    if (!d->textCached && isComponentComplete()) {
        LineSurfacePrivate *d = const_cast<LineSurfacePrivate *>(d_func());
#ifndef QT_NO_TEXTHTMLPARSER
        if (d->richText)
            d->text = d->control->toHtml();
        else
#endif
            d->text = d->control->toPlainText();
        d->textCached = true;
    }
    return d->text;
}

void LineSurface::setText(const QString &text)
{

    Q_D(LineSurface);
    if (LineSurface::text() == text)
        return;

    d->richText = d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(text));
    if (!isComponentComplete()) {
        d->text = text;
    } else if (d->control) {
        if (d->richText) {
        #ifndef QT_NO_TEXTHTMLPARSER
                d->control->setHtml(text);
        #else
                d->control->setPlainText(text);
        #endif
        } else {
            d->control->setPlainText(text);
        }
    }
}

QString LineSurface::preeditText() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QString();
    return d->control->preeditText();
}

LineSurface::TextFormat LineSurface::textFormat() const
{
    Q_D(const LineSurface);
    return d->format;
}

void LineSurface::setTextFormat(TextFormat format)
{
    Q_D(LineSurface);

    if (format == d->format)
        return;

    bool wasRich = d->richText;
    d->richText = format == RichText || (format == AutoText && (wasRich || Qt::mightBeRichText(text())));

#ifndef QT_NO_TEXTHTMLPARSER
    if (isComponentComplete()) {
        if (wasRich && !d->richText) {
            if (d->control) d->control->setPlainText(!d->textCached ? d->control->toHtml() : d->text);
            updateSize();
        } else if (!wasRich && d->richText) {
            if (d->control) d->control->setHtml(!d->textCached ? d->control->toPlainText() : d->text);
            updateSize();
        }
    }
#endif

    d->format = format;
    d->control->setAcceptRichText(d->format != PlainText);
    emit textFormatChanged(d->format);
}

LineSurface::RenderType LineSurface::renderType() const
{
    Q_D(const LineSurface);
    return d->renderType;
}

void LineSurface::setRenderType(LineSurface::RenderType renderType)
{
    Q_D(LineSurface);
    if (d->renderType == renderType)
        return;

    d->renderType = renderType;
    emit renderTypeChanged();
    d->updateDefaultTextOption();

    if (isComponentComplete())
        updateSize();
}

QFont LineSurface::font() const
{
    Q_D(const LineSurface);
    return d->sourceFont;
}

void LineSurface::setFont(const QFont &font)
{
    Q_D(LineSurface);
    if (d->sourceFont == font)
        return;

    d->sourceFont = font;
    QFont oldFont = d->font;
    d->font = font;
    if (static_cast<int>(d->font.pointSizeF()) != -1) {
        // 0.5pt resolution
        qreal size = qRound(d->font.pointSizeF()*2.0);
        d->font.setPointSizeF(size/2.0);
    }

    if (oldFont != d->font) {
        if (d->document) d->document->setDefaultFont(d->font);
        if (d->cursorItem) {
            d->cursorItem->setHeight(QFontMetrics(d->font).height());
            moveCursorDelegate();
        }
        updateSize();
        updateWholeDocument();
#ifndef QT_NO_IM
        updateInputMethod(Qt::ImCursorRectangle | Qt::ImAnchorRectangle | Qt::ImFont);
#endif
    }
    emit fontChanged(d->sourceFont);
}

QColor LineSurface::color() const
{
    Q_D(const LineSurface);
    return d->color;
}

void LineSurface::setColor(const QColor &color)
{
    Q_D(LineSurface);
    if (d->color == color)
        return;

    d->color = color;

    updateWholeDocument();
    emit colorChanged(d->color);
}

QColor LineSurface::selectionColor() const
{
    Q_D(const LineSurface);
    return d->selectionColor;
}

void LineSurface::setSelectionColor(const QColor &color)
{
    Q_D(LineSurface);
    if (d->selectionColor == color)
        return;

    d->selectionColor = color;
    updateWholeDocument();
    emit selectionColorChanged(d->selectionColor);
}

QColor LineSurface::selectedTextColor() const
{
    Q_D(const LineSurface);
    return d->selectedTextColor;
}

void LineSurface::setSelectedTextColor(const QColor &color)
{
    Q_D(LineSurface);
    if (d->selectedTextColor == color)
        return;

    d->selectedTextColor = color;
    updateWholeDocument();
    emit selectedTextColorChanged(d->selectedTextColor);
}

int LineSurface::fragmentStart() const {
    Q_D(const LineSurface);
    return d->fragmentStart;
}

int LineSurface::fragmentEnd() const {
    Q_D(const LineSurface);
    return d->fragmentEnd;
}

void LineSurface::setFragmentStart(int frStart) {
    Q_D(LineSurface);
    d->fragmentStart = frStart;
}

void LineSurface::setFragmentEnd(int frEnd) {
    Q_D(LineSurface);
    d->fragmentEnd = frEnd;
}

void LineSurface::resetFragmentStart() {
    setFragmentStart(-1);
}

void LineSurface::resetFragmentEnd() {
    setFragmentEnd(-1);
}


LineSurface::HAlignment LineSurface::hAlign() const
{
    Q_D(const LineSurface);
    return d->hAlign;
}

void LineSurface::setHAlign(HAlignment align)
{
    Q_D(LineSurface);
    bool forceAlign = d->hAlignImplicit && d->effectiveLayoutMirror;
    d->hAlignImplicit = false;
    if (d->setHAlign(align, forceAlign) && isComponentComplete()) {
        d->updateDefaultTextOption();
        updateSize();
    }
}

void LineSurface::resetHAlign()
{
    Q_D(LineSurface);
    d->hAlignImplicit = true;
    if (d->determineHorizontalAlignment() && isComponentComplete()) {
        d->updateDefaultTextOption();
        updateSize();
    }
}

LineSurface::HAlignment LineSurface::effectiveHAlign() const
{
    Q_D(const LineSurface);
    LineSurface::HAlignment effectiveAlignment = d->hAlign;
    if (!d->hAlignImplicit && d->effectiveLayoutMirror) {
        switch (d->hAlign) {
        case LineSurface::AlignLeft:
            effectiveAlignment = LineSurface::AlignRight;
            break;
        case LineSurface::AlignRight:
            effectiveAlignment = LineSurface::AlignLeft;
            break;
        default:
            break;
        }
    }
    return effectiveAlignment;
}

bool LineSurfacePrivate::setHAlign(LineSurface::HAlignment alignment, bool forceAlign)
{
    Q_Q(LineSurface);
    if (hAlign != alignment || forceAlign) {
        LineSurface::HAlignment oldEffectiveHAlign = q->effectiveHAlign();
        hAlign = alignment;
        emit q->horizontalAlignmentChanged(alignment);
        if (oldEffectiveHAlign != q->effectiveHAlign())
            emit q->effectiveHorizontalAlignmentChanged();
        return true;
    }
    return false;
}


Qt::LayoutDirection LineSurfacePrivate::textDirection(const QString &text) const
{
    const QChar *character = text.constData();
    while (!character->isNull()) {
        switch (character->direction()) {
        case QChar::DirL:
            return Qt::LeftToRight;
        case QChar::DirR:
        case QChar::DirAL:
        case QChar::DirAN:
            return Qt::RightToLeft;
        default:
            break;
        }
        character++;
    }
    return Qt::LayoutDirectionAuto;
}

bool LineSurfacePrivate::determineHorizontalAlignment()
{
    Q_Q(LineSurface);
    if (!document || !control) return false;

    if (hAlignImplicit && q->isComponentComplete()) {
        Qt::LayoutDirection direction = contentDirection;
#ifndef QT_NO_IM
        if (direction == Qt::LayoutDirectionAuto) {
            const QString preeditText = control->textCursor().block().layout()->preeditAreaText();
            direction = textDirection(preeditText);
        }
        if (direction == Qt::LayoutDirectionAuto)
            direction = qGuiApp->inputMethod()->inputDirection();
#endif

        return setHAlign(direction == Qt::RightToLeft ? LineSurface::AlignRight : LineSurface::AlignLeft);
    }
    return false;
}

void LineSurfacePrivate::mirrorChange()
{
    Q_Q(LineSurface);
    if (q->isComponentComplete()) {
        if (!hAlignImplicit && (hAlign == LineSurface::AlignRight || hAlign == LineSurface::AlignLeft)) {
            updateDefaultTextOption();
            q->updateSize();
            emit q->effectiveHorizontalAlignmentChanged();
        }
    }
}

#ifndef QT_NO_IM
Qt::InputMethodHints LineSurfacePrivate::effectiveInputMethodHints() const
{
    return inputMethodHints | Qt::ImhMultiLine;
}
#endif

void LineSurfacePrivate::setTopPadding(qreal value, bool reset)
{
    Q_Q(LineSurface);
    qreal oldPadding = q->topPadding();
    if (!reset || extra.isAllocated()) {
        extra.value().topPadding = value;
        extra.value().explicitTopPadding = !reset;
    }
    if ((!reset && !qFuzzyCompare(oldPadding, value)) || (reset && !qFuzzyCompare(oldPadding, padding()))) {
        q->updateSize();
        emit q->topPaddingChanged();
    }
}

void LineSurface::setComponents(TextEdit* te)
{
    Q_D(LineSurface);
    d->textEdit = te;

    d->document = new QTextDocument(this);
}

void LineSurfacePrivate::setLeftPadding(qreal value, bool reset)
{
    Q_Q(LineSurface);
    qreal oldPadding = q->leftPadding();
    if (!reset || extra.isAllocated()) {
        extra.value().leftPadding = value;
        extra.value().explicitLeftPadding = !reset;
    }
    if ((!reset && !qFuzzyCompare(oldPadding, value)) || (reset && !qFuzzyCompare(oldPadding, padding()))) {
        q->updateSize();
        emit q->leftPaddingChanged();
    }
}

void LineSurfacePrivate::setRightPadding(qreal value, bool reset)
{
    Q_Q(LineSurface);
    qreal oldPadding = q->rightPadding();
    if (!reset || extra.isAllocated()) {
        extra.value().rightPadding = value;
        extra.value().explicitRightPadding = !reset;
    }
    if ((!reset && !qFuzzyCompare(oldPadding, value)) || (reset && !qFuzzyCompare(oldPadding, padding()))) {
        q->updateSize();
        emit q->rightPaddingChanged();
    }
}

void LineSurfacePrivate::setBottomPadding(qreal value, bool reset)
{
    Q_Q(LineSurface);
    qreal oldPadding = q->bottomPadding();
    if (!reset || extra.isAllocated()) {
        extra.value().bottomPadding = value;
        extra.value().explicitBottomPadding = !reset;
    }
    if ((!reset && !qFuzzyCompare(oldPadding, value)) || (reset && !qFuzzyCompare(oldPadding, padding()))) {
        q->updateSize();
        emit q->bottomPaddingChanged();
    }
}

bool LineSurfacePrivate::isImplicitResizeEnabled() const
{
    return !extra.isAllocated() || extra->implicitResize;
}

void LineSurfacePrivate::setImplicitResizeEnabled(bool enabled)
{
    if (!enabled)
        extra.value().implicitResize = false;
    else if (extra.isAllocated())
        extra->implicitResize = true;
}

LineSurface::VAlignment LineSurface::vAlign() const
{
    Q_D(const LineSurface);
    return d->vAlign;
}

void LineSurface::setVAlign(LineSurface::VAlignment alignment)
{
    Q_D(LineSurface);
    if (alignment == d->vAlign)
        return;
    d->vAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
    moveCursorDelegate();
    emit verticalAlignmentChanged(d->vAlign);
}

LineSurface::WrapMode LineSurface::wrapMode() const
{
    Q_D(const LineSurface);
    return d->wrapMode;
}

void LineSurface::setWrapMode(WrapMode mode)
{
    Q_D(LineSurface);
    if (mode == d->wrapMode)
        return;
    d->wrapMode = mode;
    d->updateDefaultTextOption();
    updateSize();
    emit wrapModeChanged();
}

int LineSurface::lineCount() const
{
    Q_D(const LineSurface);
    return d->lineCount;
}

int LineSurface::length() const
{
    Q_D(const LineSurface);
    if (!d->document) return 0;
    // QTextDocument::characterCount() includes the terminating null character.
    return qMax(0, d->document->characterCount() - 1);
}


qreal LineSurface::contentWidth() const
{
    Q_D(const LineSurface);
    return d->contentSize.width();
}


qreal LineSurface::contentHeight() const
{
    Q_D(const LineSurface);
    return d->contentSize.height();
}

QUrl LineSurface::baseUrl() const
{
    Q_D(const LineSurface);
    if (d->baseUrl.isEmpty()) {
        if (QQmlContext *context = qmlContext(this))
            const_cast<LineSurfacePrivate *>(d)->baseUrl = context->baseUrl();
    }
    return d->baseUrl;
}

void LineSurface::setBaseUrl(const QUrl &url)
{
    Q_D(LineSurface);
    if (baseUrl() != url) {
        d->baseUrl = url;

        if (d->document) d->document->setBaseUrl(url);
        emit baseUrlChanged();
    }
}

void LineSurface::resetBaseUrl()
{
    if (QQmlContext *context = qmlContext(this))
        setBaseUrl(context->baseUrl());
    else
        setBaseUrl(QUrl());
}

QRectF LineSurface::positionToRectangle(int pos) const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QRectF();
    QTextCursor c(d->document);
    c.setPosition(pos);
    return d->control->cursorRect(c).translated(d->xoff, d->yoff);

}

int LineSurface::positionAt(qreal x, qreal y) const
{
    Q_D(const LineSurface);
    if (!d->document) return -1;
    x -= d->xoff;
    y -= d->yoff;

    int r = d->document->documentLayout()->hitTest(QPointF(x, y), Qt::FuzzyHit);
#ifndef QT_NO_IM
    QTextCursor cursor = d->control->textCursor();
    if (r > cursor.position()) {
        // The cursor position includes positions within the preedit text, but only positions in the
        // same text block are offset so it is possible to get a position that is either part of the
        // preedit or the next text block.
        QTextLayout *layout = cursor.block().layout();
        const int preeditLength = layout
                ? layout->preeditAreaText().length()
                : 0;
        if (preeditLength > 0
                && d->document->documentLayout()->blockBoundingRect(cursor.block()).contains(x, y)) {
            r = r > cursor.position() + preeditLength
                    ? r - preeditLength
                    : cursor.position();
        }
    }
#endif
    return r;
}

void LineSurface::moveCursorSelection(int pos)
{
    //Note that this is the same as setCursorPosition but with the KeepAnchor flag set
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    d->control->setTextCursor(cursor);
}

void LineSurface::moveCursorSelection(int pos, SelectionMode mode)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    if (mode == SelectCharacters) {
        cursor.setPosition(pos, QTextCursor::KeepAnchor);
    } else if (cursor.anchor() < pos || (cursor.anchor() == pos && cursor.position() < pos)) {
        if (cursor.anchor() > cursor.position()) {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
            if (cursor.position() == cursor.anchor())
                cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);
            else
                cursor.setPosition(cursor.position(), QTextCursor::MoveAnchor);
        } else {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
        }

        cursor.setPosition(pos, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
        if (cursor.position() != pos)
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    } else if (cursor.anchor() > pos || (cursor.anchor() == pos && cursor.position() > pos)) {
        if (cursor.anchor() < cursor.position()) {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
        } else {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            if (cursor.position() != cursor.anchor()) {
                cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
            }
        }

        cursor.setPosition(pos, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        if (cursor.position() != pos) {
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
        }
    }
    d->control->setTextCursor(cursor);
}

bool LineSurface::isCursorVisible() const
{
    Q_D(const LineSurface);
    return d->cursorVisible;
}

void LineSurface::setCursorVisible(bool on)
{
    Q_D(LineSurface);
    d->cursorVisible = on;
    if (on && isComponentComplete())
        TextUtil::createCursor(d);
    if (!on && !d->persistentSelection)
        if (d->control) d->control->setCursorIsFocusIndicator(true);
    if (d->control) d->control->setCursorVisible(on);
    emit cursorVisibleChanged(d->cursorVisible);
}

int LineSurface::cursorPosition() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return -1;

    return d->control->textCursor().position();
}

void LineSurface::setCursorPosition(int pos)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;
    if (pos < 0 || pos >= d->document->characterCount()) // characterCount includes the terminating null.
        return;
    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos && cursor.anchor() == pos)
        return;
    cursor.setPosition(pos);
    d->control->setTextCursor(cursor);
    d->control->updateCursorRectangle(true);
}


QQmlComponent* LineSurface::cursorDelegate() const
{
    Q_D(const LineSurface);
    return d->cursorComponent;
}

void LineSurface::setCursorDelegate(QQmlComponent* c)
{
    Q_D(LineSurface);
    TextUtil::setCursorDelegate(d, c);
}

void LineSurface::createCursor()
{
    Q_D(LineSurface);
    d->cursorPending = true;
    TextUtil::createCursor(d);
}

int LineSurface::selectionStart() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return -1;

    return d->control->textCursor().selectionStart();
}

int LineSurface::selectionEnd() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return -1;

    return d->control->textCursor().selectionEnd();
}

QString LineSurface::selectedText() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QString();

#ifndef QT_NO_TEXTHTMLPARSER
    return d->richText
            ? d->control->textCursor().selectedText()
            : d->control->textCursor().selection().toPlainText();
#else
    return d->control->textCursor().selection().toPlainText();
#endif
}

bool LineSurface::focusOnPress() const
{
    Q_D(const LineSurface);
    return d->focusOnPress;
}

void LineSurface::setFocusOnPress(bool on)
{
    Q_D(LineSurface);
    if (d->focusOnPress == on)
        return;
    d->focusOnPress = on;
    emit activeFocusOnPressChanged(d->focusOnPress);
}

bool LineSurface::persistentSelection() const
{
    Q_D(const LineSurface);
    return d->persistentSelection;
}

void LineSurface::setPersistentSelection(bool on)
{
    Q_D(LineSurface);
    if (d->persistentSelection == on)
        return;
    d->persistentSelection = on;
    emit persistentSelectionChanged(d->persistentSelection);
}

qreal LineSurface::textMargin() const
{
    Q_D(const LineSurface);
    return d->textMargin;
}

void LineSurface::setTextMargin(qreal margin)
{
    Q_D(LineSurface);

    if (abs(d->textMargin - margin) < LV_ACCURACY)
        return;
    d->textMargin = margin;
    if (!d->document) d->document->setDocumentMargin(d->textMargin);
    emit textMarginChanged(d->textMargin);
}


Qt::InputMethodHints LineSurface::inputMethodHints() const
{
#ifdef QT_NO_IM
    return Qt::ImhNone;
#else
    Q_D(const LineSurface);
    return d->inputMethodHints;
#endif // QT_NO_IM
}

void LineSurface::setInputMethodHints(Qt::InputMethodHints hints)
{
#ifdef QT_NO_IM
    Q_UNUSED(hints);
#else
    Q_D(LineSurface);

    if (hints == d->inputMethodHints)
        return;

    d->inputMethodHints = hints;
    updateInputMethod(Qt::ImHints);
    emit inputMethodHintsChanged();
#endif // QT_NO_IM
}

void LineSurface::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    Q_D(LineSurface);
    if (!d->inLayout && ((abs(newGeometry.width() - oldGeometry.width()) > LV_ACCURACY && widthValid())
        || (abs(newGeometry.height() - oldGeometry.height()) > LV_ACCURACY && heightValid()))) {
        updateSize();
        updateWholeDocument();
        moveCursorDelegate();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

}

void LineSurface::componentComplete()
{
    Q_D(LineSurface);

    QQuickItem::componentComplete();
    if (!d->document || !d->control) return;

    d->document->setBaseUrl(baseUrl());
#ifndef QT_NO_TEXTHTML_PARSER
    if (d->richText)
        d->control->setHtml(d->text);
    else
#endif
    if (!d->text.isEmpty())
        d->control->setPlainText(d->text);

    if (d->dirty) {
        d->determineHorizontalAlignment();
        d->updateDefaultTextOption();
        updateSize();
        d->dirty = false;
    }
    if (d->cursorComponent && isCursorVisible())
        TextUtil::createCursor(d);
}

bool LineSurface::selectByKeyboard() const
{
    Q_D(const LineSurface);
    if (d->selectByKeyboardSet)
        return d->selectByKeyboard;
    return !isReadOnly();
}

void LineSurface::setSelectByKeyboard(bool on)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    bool was = selectByKeyboard();
    if (!d->selectByKeyboardSet || was != on) {
        d->selectByKeyboardSet = true;
        d->selectByKeyboard = on;
        if (on)
            d->control->setTextInteractionFlags(d->control->textInteractionFlags() | Qt::TextSelectableByKeyboard);
        else
            d->control->setTextInteractionFlags(d->control->textInteractionFlags() & ~Qt::TextSelectableByKeyboard);
        emit selectByKeyboardChanged(on);
    }
}

bool LineSurface::selectByMouse() const
{
    Q_D(const LineSurface);
    return d->selectByMouse;
}

void LineSurface::setSelectByMouse(bool on)
{
    Q_D(LineSurface);

    d->selectByMouse = on;
    setKeepMouseGrab(on);
    if (on) {
        if (d->control) d->control->setTextInteractionFlags(d->control->textInteractionFlags() | Qt::TextSelectableByMouse);
    }
    else if (d->control)
        d->control->setTextInteractionFlags(d->control->textInteractionFlags() & ~Qt::TextSelectableByMouse);
    emit selectByMouseChanged(on);

}

LineSurface::SelectionMode LineSurface::mouseSelectionMode() const
{
    Q_D(const LineSurface);
    return d->mouseSelectionMode;
}

void LineSurface::setMouseSelectionMode(SelectionMode mode)
{
    Q_D(LineSurface);

    d->mouseSelectionMode = mode;
    if (d->control) d->control->setWordSelectionEnabled(mode == SelectWords);
    emit mouseSelectionModeChanged(mode);
}

void LineSurface::setReadOnly(bool r)
{
    Q_D(LineSurface);

    d->readOnly = r;
#ifndef QT_NO_IM
    setFlag(QQuickItem::ItemAcceptsInputMethod, !d->readOnly);
#endif
    Qt::TextInteractionFlags flags = Qt::LinksAccessibleByMouse;
    if (d->selectByMouse)
        flags = flags | Qt::TextSelectableByMouse;
    if (d->selectByKeyboardSet && d->selectByKeyboard)
        flags = flags | Qt::TextSelectableByKeyboard;
    else if (!d->selectByKeyboardSet && !d->readOnly)
        flags = flags | Qt::TextSelectableByKeyboard;
    if (!d->readOnly)
        flags = flags | Qt::TextEditable;
    if (d->control)
    {
        d->control->setTextInteractionFlags(flags);
        d->control->moveCursor(QTextCursor::End);
    }

#ifndef QT_NO_IM
    updateInputMethod(Qt::ImEnabled);
#endif
    q_canPasteChanged();
    emit readOnlyChanged(d->readOnly);
    if (!d->selectByKeyboardSet)
        emit selectByKeyboardChanged(!d->readOnly);
    if (d->readOnly) {
        setCursorVisible(false);
    } else if (hasActiveFocus()) {
        setCursorVisible(true);
    }
}

bool LineSurface::isReadOnly() const
{
    Q_D(const LineSurface);
    return d->readOnly;
}

QRectF LineSurface::cursorRectangle() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QRectF();

    return d->control->cursorRect().translated(d->xoff, d->yoff);
}

bool LineSurface::event(QEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return QQuickItem::event(event);

    if (event->type() == QEvent::ShortcutOverride) {
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
        return event->isAccepted();
    }
    return QQuickItem::event(event);
}

void LineSurface::keyPressEvent(QKeyEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control)
    {
        QQuickItem::keyPressEvent(event);
        return;
    }

    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (!event->isAccepted())
        QQuickItem::keyPressEvent(event);
}

void LineSurface::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control)
    {
        QQuickItem::keyReleaseEvent(event);
        return;
    }
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (!event->isAccepted())
        QQuickItem::keyReleaseEvent(event);
}

void LineSurface::deselect()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    QTextCursor c = d->control->textCursor();
    c.clearSelection();
    d->control->setTextCursor(c);
}

void LineSurface::selectAll()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    d->control->selectAll();
}

void LineSurface::selectWord()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    QTextCursor c = d->control->textCursor();
    c.select(QTextCursor::WordUnderCursor);
    d->control->setTextCursor(c);
}
void LineSurface::select(int start, int end)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    if (start < 0 || end < 0 || start >= d->document->characterCount() || end >= d->document->characterCount())
        return;
    QTextCursor cursor = d->control->textCursor();
    cursor.beginEditBlock();
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.endEditBlock();
    d->control->setTextCursor(cursor);

    // QTBUG-11100
    updateSelection();
    updateInputMethod();
}

bool LineSurface::isRightToLeft(int start, int end)
{
    if (start > end) {
        qmlInfo(this) << "isRightToLeft(start, end) called with the end property being smaller than the start.";
        return false;
    } else {
        return getText(start, end).isRightToLeft();
    }
}

#ifndef QT_NO_CLIPBOARD
void LineSurface::cut()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    d->control->cut();
}

void LineSurface::copy()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    d->control->copy();
}

void LineSurface::paste()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    d->control->paste();
}
#endif // QT_NO_CLIPBOARD



void LineSurface::undo()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    d->control->undo();
}
void LineSurface::redo()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    d->control->redo();
}

void LineSurface::mousePressEvent(QMouseEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control)
    {
        QQuickItem::mousePressEvent(event);
        return;
    }

    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (d->focusOnPress){
        bool hadActiveFocus = hasActiveFocus();
        forceActiveFocus(Qt::MouseFocusReason);
        // re-open input panel on press if already focused
#ifndef QT_NO_IM
        if (hasActiveFocus() && hadActiveFocus && !isReadOnly())
            qGuiApp->inputMethod()->show();
#else
        Q_UNUSED(hadActiveFocus);
#endif
    }
    if (!event->isAccepted())
        QQuickItem::mousePressEvent(event);
}

void LineSurface::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control)
    {
        QQuickItem::mouseReleaseEvent(event);
        return;
    }
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));

    if (!event->isAccepted())
        QQuickItem::mouseReleaseEvent(event);
}

void LineSurface::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control)
    {
        QQuickItem::mouseDoubleClickEvent(event);
        return;
    }
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (!event->isAccepted())
        QQuickItem::mouseDoubleClickEvent(event);
}

void LineSurface::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control)
    {
        QQuickItem::mouseMoveEvent(event);
        return;
    }
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (!event->isAccepted())
        QQuickItem::mouseMoveEvent(event);
}

#ifndef QT_NO_IM
void LineSurface::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;
    const bool wasComposing = isInputMethodComposing();
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    setCursorVisible(d->control->cursorVisible());
    if (wasComposing != isInputMethodComposing())
        emit inputMethodComposingChanged();
}

QVariant LineSurface::inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const
{
    Q_D(const LineSurface);

    QVariant v;
    switch (property) {
    case Qt::ImEnabled:
        v = static_cast<bool>(flags() & ItemAcceptsInputMethod);
        break;
    case Qt::ImHints:
        v = static_cast<int>(d->effectiveInputMethodHints());
        break;
    case Qt::ImInputItemClipRectangle:
        v = QQuickItem::inputMethodQuery(property);
        break;
    default:
        if (property == Qt::ImCursorPosition && !argument.isNull())
            argument = QVariant(argument.toPointF() - QPointF(d->xoff, d->yoff));
        if (!d->document || !d->control) break;
        v = d->control->inputMethodQuery(property, argument);
        if (property == Qt::ImCursorRectangle || property == Qt::ImAnchorRectangle)
            v = QVariant(v.toRectF().translated(d->xoff, d->yoff));
        break;
    }
    return v;
}

QVariant LineSurface::inputMethodQuery(Qt::InputMethodQuery property) const
{
    return inputMethodQuery(property, QVariant());
}
#endif // QT_NO_IM

void LineSurface::triggerPreprocess()
{
    Q_D(LineSurface);
    if (d->updateType == LineSurfacePrivate::UpdateNone)
        d->updateType = LineSurfacePrivate::UpdateOnlyPreprocess;
    polish();
    update();
}

typedef QList<LineSurfacePrivate::Node*>::iterator TextNodeIterator;


static bool comesBefore(LineSurfacePrivate::Node* n1, LineSurfacePrivate::Node* n2)
{
    return n1->startPos() < n2->startPos();
}

static inline void updateNodeTransform(TextNode* node, const QPointF &topLeft)
{
    QMatrix4x4 transformMatrix;
    transformMatrix.translate(static_cast<float>(topLeft.x()), static_cast<float>(topLeft.y()));
    node->setMatrix(transformMatrix);
}

void LineSurface::invalidateFontCaches()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;


    QTextBlock block;
    for (block = d->document->firstBlock(); block.isValid(); block = block.next()) {
        if (block.layout() != nullptr && block.layout()->engine() != nullptr)
            block.layout()->engine()->resetFontEngineCache();
    }
}

inline void resetEngine(TextNodeEngine *engine, const QColor& textColor, const QColor& selectedTextColor, const QColor& selectionColor)
{
    *engine = TextNodeEngine();
    engine->setTextColor(textColor);
    engine->setSelectedTextColor(selectedTextColor);
    engine->setSelectionColor(selectionColor);
}

QSGNode *LineSurface::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData);
    Q_D(LineSurface);

    if (!d->document || !d->control) {
        if (oldNode) delete oldNode;
        return nullptr;
    }

    if (d->updateType != LineSurfacePrivate::UpdatePaintNode && oldNode != nullptr) {
        // Update done in preprocess() in the nodes
        d->updateType = LineSurfacePrivate::UpdateNone;
        return oldNode;
    }

    d->updateType = LineSurfacePrivate::UpdateNone;

    if (!oldNode) {
        // If we had any QQuickTextNode node references, they were deleted along with the root node
        // But here we must delete the Node structures in textNodeMap
        qDeleteAll(d->textNodeMap);
        d->textNodeMap.clear();
    }

    RootNode *rootNode = static_cast<RootNode *>(oldNode);
    TextNodeIterator nodeIterator = d->textNodeMap.begin();

    while (nodeIterator != d->textNodeMap.end() && !(*nodeIterator)->dirty())
        ++nodeIterator;

    TextNodeEngine engine;
    TextNodeEngine frameDecorationsEngine;

    d->lastHighlightChangeStart = INT_MAX;
    d->lastHighlightChangeEnd   = 0;

    if (!oldNode || nodeIterator < d->textNodeMap.end()) {

        if (!oldNode)
            rootNode = new RootNode;

        int firstDirtyPos = 0;
        if (nodeIterator != d->textNodeMap.end()) {
            firstDirtyPos = (*nodeIterator)->startPos();
            do {
                rootNode->removeChildNode((*nodeIterator)->textNode());
                delete (*nodeIterator)->textNode();
                delete *nodeIterator;
                nodeIterator = d->textNodeMap.erase(nodeIterator);
            } while (nodeIterator != d->textNodeMap.end() && (*nodeIterator)->dirty());
        }

        // FIXME: the text decorations could probably be handled separately (only updated for affected textFrames)
        rootNode->resetFrameDecorations(d->createTextNode());
        resetEngine(&frameDecorationsEngine, d->color, d->selectedTextColor, d->selectionColor);

        TextNode *node = nullptr;

        int currentNodeSize = 0;
        int nodeStart = firstDirtyPos;
        QPointF basePosition(d->xoff, d->yoff);
        QMatrix4x4 basePositionMatrix;
        basePositionMatrix.translate(static_cast<float>(basePosition.x()), static_cast<float>(basePosition.y()));
        rootNode->setMatrix(basePositionMatrix);

        QPointF nodeOffset;
        LineSurfacePrivate::Node *firstCleanNode = (nodeIterator != d->textNodeMap.end()) ? *nodeIterator : nullptr;

        if (d->document && d->control) {
            QList<QTextFrame *> frames;
            frames.append(d->document->rootFrame());

            while (!frames.isEmpty()) { //INFO: Root frame
                QTextFrame *textFrame = frames.takeFirst();
                frames.append(textFrame->childFrames());
                frameDecorationsEngine.addFrameDecorations(d->document, textFrame);

                if (textFrame->lastPosition() < firstDirtyPos || (firstCleanNode && textFrame->firstPosition() >= firstCleanNode->startPos()))
                    continue;

                //INFO: creating the text node
                node = d->createTextNode();
                resetEngine(&engine, d->color, d->selectedTextColor, d->selectionColor);

                if (textFrame->firstPosition() > textFrame->lastPosition()
                        && textFrame->frameFormat().position() != QTextFrameFormat::InFlow) {

                    updateNodeTransform(node, d->document->documentLayout()->frameBoundingRect(textFrame).topLeft());
                    const int pos = textFrame->firstPosition() - 1;
                    ProtectedLayoutAccessor *a = static_cast<ProtectedLayoutAccessor *>(d->document->documentLayout());
                    QTextCharFormat format = a->formatAccessor(pos);
                    QTextBlock block = textFrame->firstCursorPosition().block();
                    engine.setCurrentLine(block.layout()->lineForTextPosition(pos - block.position()));
                    engine.addTextObject(QPointF(0, 0), format, TextNodeEngine::Unselected, d->document,
                                                  pos, textFrame->frameFormat().position());
                    nodeStart = pos;
                } else {
                    // Having nodes spanning across frame boundaries will break the current bookkeeping mechanism. We need to prevent that.
                    QList<int> frameBoundaries;
                    frameBoundaries.reserve(frames.size());
                    Q_FOREACH (QTextFrame *frame, frames)
                        frameBoundaries.append(frame->firstPosition());
                    std::sort(frameBoundaries.begin(), frameBoundaries.end());

                    QTextFrame::iterator it = textFrame->begin();

                    while (!it.atEnd()) {

                        QTextBlock block = it.currentBlock();
                        ++it;

                        if (block.position() < firstDirtyPos)
                            continue;

                        if (!block.isVisible()) continue;

                        if (!engine.hasContents()) {
                            nodeOffset = d->document->documentLayout()->blockBoundingRect(block).topLeft();
                            updateNodeTransform(node, nodeOffset);
                            nodeStart = block.position();
                        }

                        engine.addTextBlock(d->document, block, -nodeOffset, d->color, QColor(), selectionStart(), selectionEnd() - 1);
                        currentNodeSize += block.length();

                        if ((it.atEnd()) || (firstCleanNode && block.next().position() >= firstCleanNode->startPos())) // last node that needed replacing or last block of the frame
                            break;

                        QList<int>::const_iterator lowerBound = std::lower_bound(frameBoundaries.constBegin(), frameBoundaries.constEnd(), block.next().position());
                        if (currentNodeSize > nodeBreakingSize || lowerBound == frameBoundaries.constEnd() || *lowerBound > nodeStart) {
                            currentNodeSize = 0;
                            d->addCurrentTextNodeToRoot(&engine, rootNode, node, nodeIterator, nodeStart);
                            node = d->createTextNode();
                            resetEngine(&engine, d->color, d->selectedTextColor, d->selectionColor);
                            nodeStart = block.next().position();
                        }
                    }
                }
                d->addCurrentTextNodeToRoot(&engine, rootNode, node, nodeIterator, nodeStart);
            }
        }


        frameDecorationsEngine.addToSceneGraph(rootNode->frameDecorationsNode, QQuickText::Normal, QColor());
        // Now prepend the frame decorations since we want them rendered first, with the text nodes and cursor in front.
        rootNode->prependChildNode(rootNode->frameDecorationsNode);

        Q_ASSERT(nodeIterator == d->textNodeMap.end() || (*nodeIterator) == firstCleanNode);
        // Update the position of the subsequent text blocks.
        if (d->document && firstCleanNode) {
            QPointF oldOffset = firstCleanNode->textNode()->matrix().map(QPointF(0,0));
            QPointF currentOffset = d->document->documentLayout()->blockBoundingRect(d->document->findBlock(firstCleanNode->startPos())).topLeft();
            QPointF delta = currentOffset - oldOffset;
            while (nodeIterator != d->textNodeMap.end()) {
                QMatrix4x4 transformMatrix = (*nodeIterator)->textNode()->matrix();
                transformMatrix.translate(static_cast<float>(delta.x()), static_cast<float>(delta.y()));
                (*nodeIterator)->textNode()->setMatrix(transformMatrix);
                ++nodeIterator;
            }

        }

        // Since we iterate over blocks from different text frames that are potentially not sorted
        // we need to ensure that our list of nodes is sorted again:
        std::sort(d->textNodeMap.begin(), d->textNodeMap.end(), &comesBefore);
    }

    if (d->document && d->control && d->cursorComponent == nullptr) {
        QSGRectangleNode* cursor = nullptr;
        if (!isReadOnly() && d->cursorVisible && d->control->cursorOn())
            cursor = d->sceneGraphContext()->createRectangleNode(d->control->cursorRect(), d->color);
        rootNode->resetCursorNode(cursor);
    }

    invalidateFontCaches();

    return rootNode;
}

void LineSurface::updatePolish()
{
    invalidateFontCaches();
}

bool LineSurface::canPaste() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return false;

    if (!d->canPasteValid) {
        const_cast<LineSurfacePrivate *>(d)->canPaste = d->control->canPaste();
        const_cast<LineSurfacePrivate *>(d)->canPasteValid = true;
    }
    return d->canPaste;
}


bool LineSurface::canUndo() const
{
    Q_D(const LineSurface);
    if (!d->document) return false;

    return d->document->isUndoAvailable();
}

bool LineSurface::canRedo() const
{
    Q_D(const LineSurface);
    if (!d->document) return false;

    return d->document->isRedoAvailable();
}

bool LineSurface::isInputMethodComposing() const
{
#ifdef QT_NO_IM
    return false;
#else
    Q_D(const LineSurface);
    if (!d->document || !d->control) return false;
    return d->control->hasImState();
#endif // QT_NO_IM
}

LineSurfacePrivate::ExtraData::ExtraData()
    : padding(0)
    , topPadding(0)
    , leftPadding(0)
    , rightPadding(0)
    , bottomPadding(0)
    , explicitTopPadding(false)
    , explicitLeftPadding(false)
    , explicitRightPadding(false)
    , explicitBottomPadding(false)
    , implicitResize(true)
{
}

void LineSurface::singleShotUpdate()
{
    Q_D(LineSurface);
    d->highlightingInProgress = true;
    emit dirtyBlockPosition(-1);
    d->document->markContentsDirty(0, d->document->characterCount());
}

void LineSurfacePrivate::setTextDocument(QTextDocument *d)
{
    Q_Q(LineSurface);
    document = d;

    control = new TextControl(document, q);
    control->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByKeyboard | Qt::TextEditable);
    control->setAcceptRichText(false);
    control->setCursorIsFocusIndicator(true);
/*
    fragmentStart=2;
    fragmentEnd = 10;
    q->updateFragmentVisibility();
*/
    lv_qmlobject_connect(control, TextControl, SIGNAL(updateCursorRequest()), q, LineSurface, SLOT(updateCursor()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(selectionChanged()), q, LineSurface, SIGNAL(selectedTextChanged()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(selectionChanged()), q, LineSurface, SLOT(updateSelection()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(cursorPositionChanged()), q, LineSurface, SLOT(updateSelection()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(cursorPositionChanged()), q, LineSurface, SIGNAL(cursorPositionChanged()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(cursorRectangleChanged()), q, LineSurface, SLOT(moveCursorDelegate()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(linkActivated(QString)), q, LineSurface, SIGNAL(linkActivated(QString)));
    lv_qmlobject_connect(control, TextControl, SIGNAL(linkHovered(QString)), q, LineSurface, SIGNAL(linkHovered(QString)));
    lv_qmlobject_connect(control, TextControl, SIGNAL(textChanged()), q, LineSurface, SLOT(q_textChanged()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(preeditTextChanged()), q, LineSurface, SIGNAL(preeditTextChanged()));
    /*#if QT_CONFIG(clipboard)
        qmlobject_connect(QGuiApplication::clipboard(), QClipboard, SIGNAL(dataChanged()), q, QQuickLineSurface, SLOT(q_canPasteChanged()));
    #endif*/
    lv_qmlobject_connect(document, QTextDocument, SIGNAL(undoAvailable(bool)), q, LineSurface, SIGNAL(canUndoChanged()));
    lv_qmlobject_connect(document, QTextDocument, SIGNAL(redoAvailable(bool)), q, LineSurface, SIGNAL(canRedoChanged()));



    document->setDefaultFont(font);
    document->setDocumentMargin(textMargin);
    document->setUndoRedoEnabled(false); // flush undo buffer.
    document->setUndoRedoEnabled(true);

    q->setCursorVisible(cursorVisible);
    q->setSelectByMouse(selectByMouse);
    q->setMouseSelectionMode(mouseSelectionMode);
    q->setReadOnly(readOnly);
    updateDefaultTextOption();
    q->updateSize();

    QObject::connect(document, &QTextDocument::contentsChange, q, &LineSurface::q_contentsChange);
    QObject::connect(document->documentLayout(), &QAbstractTextDocumentLayout::updateBlock, q, &LineSurface::invalidateBlock);
    QObject::connect(document->documentLayout(), &QAbstractTextDocumentLayout::update, q, &LineSurface::highlightingDone);
}

void LineSurfacePrivate::unsetTextDocument()
{
    Q_Q(LineSurface);
    if ( document )
    q->markDirtyNodesForRange(0, document->characterCount(), 0);

    if (control)
    {
        control->disconnect();
        delete control;
        control = nullptr;
    }

    document = nullptr;


}

void LineSurfacePrivate::init()
{
    Q_Q(LineSurface);

#ifndef QT_NO_CLIPBOARD
    if (QGuiApplication::clipboard()->supportsSelection())
        q->setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton);
    else
#endif
        q->setAcceptedMouseButtons(Qt::LeftButton);

#ifndef QT_NO_IM
    q->setFlag(QQuickItem::ItemAcceptsInputMethod);
#endif
    q->setFlag(QQuickItem::ItemHasContents);

    q->setAcceptHoverEvents(true);


#ifndef QT_NO_CLIPBOARD
    lv_qmlobject_connect(QGuiApplication::clipboard(), QClipboard, SIGNAL(dataChanged()), q, LineSurface, SLOT(q_canPasteChanged()));
#endif

}

void LineSurfacePrivate::resetInputMethod()
{
    Q_Q(LineSurface);
    if (!q->isReadOnly() && q->hasActiveFocus() && qGuiApp)
        QGuiApplication::inputMethod()->reset();
}

void LineSurface::q_textChanged()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;
    d->textCached = false;
    for (QTextBlock it = d->document->begin(); it != d->document->end(); it = it.next()) {
        d->contentDirection = d->textDirection(it.text());
        if (d->contentDirection != Qt::LayoutDirectionAuto)
            break;
    }
    d->determineHorizontalAlignment();
    d->updateDefaultTextOption();
    updateSize();
    emit textChanged();
}

void LineSurface::markDirtyNodesForRange(int start, int end, int charDelta)
{
    Q_D(LineSurface);
    if (start == end)
        return;

    int oldEnd = end;
    bool nodesUpdate = true;

    if ( start < d->lastHighlightChangeStart && end > d->lastHighlightChangeEnd ){ // stretch both ways
        d->lastHighlightChangeStart = start;
        d->lastHighlightChangeEnd   = end;
    } else if ( start < d->lastHighlightChangeStart ){ // stretch left
        end = d->lastHighlightChangeStart;
        d->lastHighlightChangeStart = start;
    } else if ( end > d->lastHighlightChangeEnd ){ // stretch right
        start = d->lastHighlightChangeEnd;
        d->lastHighlightChangeEnd = end;
    } else {
        nodesUpdate = false; // this is inside the interval we're updating
    }

    if(start == end) nodesUpdate = false;

    LineSurfacePrivate::Node dummyNode(start, nullptr);
    TextNodeIterator it = std::lower_bound(d->textNodeMap.begin(), d->textNodeMap.end(), &dummyNode, &comesBefore);
    if (nodesUpdate && charDelta)
    {
        // qLowerBound gives us the first node past the start of the affected portion, rewind to the first node
        // that starts at the last position before the edit position. (there might be several because of images)
        if (it != d->textNodeMap.begin()) {
            --it;
            LineSurfacePrivate::Node otherDummy((*it)->startPos(), nullptr);
            it = std::lower_bound(d->textNodeMap.begin(), d->textNodeMap.end(), &otherDummy, &comesBefore);
        }
    }

    TextNodeIterator otherIt = it;

    if (nodesUpdate)
    {
        while (it != d->textNodeMap.end() && (*it)->startPos() <= end)
        {
            (*it)->setDirty(); ++it;
        }
    }

    if (charDelta)
    {
        while (otherIt != d->textNodeMap.end() && (*otherIt)->startPos() <= oldEnd)
        {
            ++otherIt;
        }

        while (otherIt != d->textNodeMap.end())
        {
            (*otherIt)->moveStartPos(charDelta);
            ++otherIt;
        }
    }
}

void LineSurface::q_contentsChange(int pos, int charsRemoved, int charsAdded)
{
    Q_D(LineSurface);

    const int editRange = pos + qMax(charsAdded, charsRemoved);
    const int delta = charsAdded - charsRemoved;

    emit dirtyBlockPosition(d->document->findBlock(pos).blockNumber());

    markDirtyNodesForRange(pos, editRange, delta);

    d->highlightingInProgress = true;

    polish();
    if (isComponentComplete()) {
        d->updateType = LineSurfacePrivate::UpdatePaintNode;
        update();
    }
}

void LineSurface::moveCursorDelegate()
{
    Q_D(LineSurface);
#ifndef QT_NO_IM
    updateInputMethod();
#endif
    emit cursorRectangleChanged();
    if (!d->cursorItem)
        return;
    QRectF cursorRect = cursorRectangle();
    d->cursorItem->setX(cursorRect.x());
    d->cursorItem->setY(cursorRect.y());
}

void LineSurface::updateSelection()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    // No need for node updates when we go from an empty selection to another empty selection
    if (d->control->textCursor().hasSelection() || d->hadSelection) {
        markDirtyNodesForRange(qMin(d->lastSelectionStart, d->control->textCursor().selectionStart()), qMax(d->control->textCursor().selectionEnd(), d->lastSelectionEnd), 0);
        polish();
        if (isComponentComplete()) {
            d->updateType = LineSurfacePrivate::UpdatePaintNode;
            update();
        }
    }

    d->hadSelection = d->control->textCursor().hasSelection();

    if (d->lastSelectionStart != d->control->textCursor().selectionStart()) {
        d->lastSelectionStart = d->control->textCursor().selectionStart();
        emit selectionStartChanged();
    }
    if (d->lastSelectionEnd != d->control->textCursor().selectionEnd()) {
        d->lastSelectionEnd = d->control->textCursor().selectionEnd();
        emit selectionEndChanged();
    }
}

QRectF LineSurface::boundingRect() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QRectF();

    QRectF r(
            TextUtil::alignedX(d->contentSize.width(), width(), effectiveHAlign()),
            d->yoff,
            d->contentSize.width(),
            d->contentSize.height());

    int cursorWidth = 1;
    if (d->cursorItem)
        cursorWidth = 0;
    else if (!d->document->isEmpty())
        cursorWidth += 3;// ### Need a better way of accounting for space between char and cursor

    // Could include font max left/right bearings to either side of rectangle.
    r.setRight(r.right() + cursorWidth);

    return r;
}

QRectF LineSurface::clipRect() const
{
    Q_D(const LineSurface);
    QRectF r = QQuickItem::clipRect();
    if (!d->document || !d->control) return r;

    int cursorWidth = 1;
    if (d->cursorItem)
        cursorWidth = static_cast<int>(d->cursorItem->width());
    if (!d->document->isEmpty())
        cursorWidth += 3;// ### Need a better way of accounting for space between char and cursor

    // Could include font max left/right bearings to either side of rectangle.

    r.setRight(r.right() + cursorWidth);
    return r;
}

qreal LineSurfacePrivate::getImplicitWidth() const
{
    Q_Q(const LineSurface);
    if (!requireImplicitWidth) {
        // We don't calculate implicitWidth unless it is required.
        // We need to force a size update now to ensure implicitWidth is calculated
        const_cast<LineSurfacePrivate*>(this)->requireImplicitWidth = true;
        const_cast<LineSurface*>(q)->updateSize();
    }
    return implicitWidth;
}

//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void LineSurface::updateSize()
{
    Q_D(LineSurface);

    if (!isComponentComplete()) {
        d->dirty = true;
        return;
    }

    if (!d->document || !d->control) return;

    if (abs(d->document->textWidth() +1) > LV_ACCURACY)
        d->document->setTextWidth(-1);

    qreal naturalWidth = d->implicitWidth - leftPadding() - rightPadding();


    qreal newWidth = d->document->idealWidth();
    // ### assumes that if the width is set, the text will fill to edges
    // ### (unless wrap is false, then clipping will occur)
    if (widthValid()) {
        if (!d->requireImplicitWidth) {
            emit implicitWidthChanged();
            // if the implicitWidth is used, then updateSize() has already been called (recursively)
            if (d->requireImplicitWidth)
                return;
        }
        if (d->requireImplicitWidth) {
            d->document->setTextWidth(-1);
            naturalWidth = d->document->idealWidth();

            const bool wasInLayout = d->inLayout;
            d->inLayout = true;
            if (d->isImplicitResizeEnabled())
                setImplicitWidth(naturalWidth + leftPadding() + rightPadding());
            d->inLayout = wasInLayout;
            if (d->inLayout)    // probably the result of a binding loop, but by letting it
                return;         // get this far we'll get a warning to that effect.
        }
//        d->document->setTextWidth(-1);
//        if (abs(d->document->textWidth() - width()) > LV_ACCURACY) {
//            d->document->setTextWidth(width() - leftPadding() - rightPadding());
//            newWidth = d->document->idealWidth();
//        }
        //### need to confirm cost of always setting these
    } else if (d->wrapMode == NoWrap && abs(d->document->textWidth() - newWidth) > LV_ACCURACY) {
//        d->document->setTextWidth(newWidth); // ### Text does not align if width is not set or the idealWidth exceeds the textWidth (QTextDoc bug)
    } else {
//        d->document->setTextWidth(-1);
    }

    QFontMetricsF fm(d->font);
    qreal newHeight = d->document->isEmpty() ? qCeil(fm.height()) : d->document->size().height();

    if (d->isImplicitResizeEnabled()) {
        // ### Setting the implicitWidth triggers another updateSize(), and unless there are bindings nothing has changed.
        if (!widthValid() && !d->requireImplicitWidth)
            setImplicitSize(newWidth + leftPadding() + rightPadding(), newHeight + topPadding() + bottomPadding());
        else
            setImplicitHeight(newHeight + topPadding() + bottomPadding());
    }

    d->xoff = leftPadding() + qMax(qreal(0), TextUtil::alignedX(d->document->size().width(), width() - leftPadding() - rightPadding(), effectiveHAlign()));
    d->yoff = topPadding() + TextUtil::alignedY(d->document->size().height(), height() - topPadding() - bottomPadding(), d->vAlign);
    setBaselineOffset(fm.ascent() + d->yoff + d->textMargin);

    QSizeF size(newWidth, newHeight);
    if (d->contentSize != size) {
        d->contentSize = size;
        emit contentSizeChanged();
        updateTotalLines();
    }
}

void LineSurface::updateWholeDocument()
{
    Q_D(LineSurface);
    if (!d->textNodeMap.isEmpty()) {
        Q_FOREACH (LineSurfacePrivate::Node* node, d->textNodeMap)
            node->setDirty();
    }

    polish();
    if (isComponentComplete()) {
        d->updateType = LineSurfacePrivate::UpdatePaintNode;
        update();
    }
}

void LineSurface::highlightingDone(const QRectF &)
{
    Q_D(LineSurface);

    if (d->highlightingInProgress)
    {
        d->highlightingInProgress = false;
        emit textDocumentFinishedUpdating();
    }
}

void LineSurface::invalidateBlock(const QTextBlock &block)
{
    Q_D(LineSurface);

    markDirtyNodesForRange(block.position(), block.position() + block.length(), 0);

    ProjectDocumentBlockData* userData = static_cast<ProjectDocumentBlockData*>(block.userData());

    if (userData && userData->stateChangeFlag())
    {
        emit dirtyBlockPosition(block.blockNumber());
        emit textDocumentFinishedUpdating();
    }

    polish();
    if (isComponentComplete()) {
        updateSize();
        d->updateType = LineSurfacePrivate::UpdatePaintNode;
        update();
    }
}

void LineSurface::updateCursor()
{
    Q_D(LineSurface);
    polish();
    if (isComponentComplete()) {
        d->updateType = LineSurfacePrivate::UpdatePaintNode;
        update();
    }
}

void LineSurface::q_updateAlignment()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    if (d->determineHorizontalAlignment()) {
        d->updateDefaultTextOption();
        d->xoff = qMax(qreal(0), TextUtil::alignedX(d->document->size().width(), width(), effectiveHAlign()));
        moveCursorDelegate();
    }
}

void LineSurface::updateTotalLines()
{
    Q_D(LineSurface);

    int subLines = 0;
    if (!d->document || !d->control) return;

    for (QTextBlock it = d->document->begin(); it != d->document->end(); it = it.next()) {
        QTextLayout *layout = it.layout();
        if (!layout)
            continue;
        subLines += layout->lineCount()-1;
    }

    int newTotalLines = d->document->lineCount() + subLines;
    if (d->lineCount != newTotalLines) {
        d->lineCount = newTotalLines;
        emit lineCountChanged();
        updateFragmentVisibility();
    }
}

void LineSurfacePrivate::updateDefaultTextOption()
{
    Q_Q(LineSurface);
    if (!document || !control) return;

    QTextOption opt = document->defaultTextOption();
    int oldAlignment = opt.alignment();
    Qt::LayoutDirection oldTextDirection = opt.textDirection();

    LineSurface::HAlignment horizontalAlignment = q->effectiveHAlign();
    if (contentDirection == Qt::RightToLeft) {
        if (horizontalAlignment == LineSurface::AlignLeft)
            horizontalAlignment = LineSurface::AlignRight;
        else if (horizontalAlignment == LineSurface::AlignRight)
            horizontalAlignment = LineSurface::AlignLeft;
    }
    if (!hAlignImplicit)
        opt.setAlignment(static_cast<Qt::Alignment>(static_cast<int>(horizontalAlignment | vAlign)));
    else
        opt.setAlignment(Qt::Alignment(vAlign));

#ifndef QT_NO_IM
    if (contentDirection == Qt::LayoutDirectionAuto) {
        opt.setTextDirection(qGuiApp->inputMethod()->inputDirection());
    } else
#endif
    {
        opt.setTextDirection(contentDirection);
    }

    QTextOption::WrapMode oldWrapMode = opt.wrapMode();
    opt.setWrapMode(QTextOption::WrapMode(wrapMode));

    bool oldUseDesignMetrics = opt.useDesignMetrics();
    opt.setUseDesignMetrics(renderType != LineSurface::NativeRendering);

    if (oldWrapMode != opt.wrapMode() || oldAlignment != opt.alignment()
        || oldTextDirection != opt.textDirection()
        || oldUseDesignMetrics != opt.useDesignMetrics()) {
        document->setDefaultTextOption(opt);
    }
}

void LineSurface::focusInEvent(QFocusEvent *event)
{
    Q_D(LineSurface);
    d->handleFocusEvent(event);
    QQuickItem::focusInEvent(event);
}

void LineSurface::focusOutEvent(QFocusEvent *event)
{
    Q_D(LineSurface);
    d->handleFocusEvent(event);
    QQuickItem::focusOutEvent(event);
}

void LineSurfacePrivate::handleFocusEvent(QFocusEvent *event)
{
    Q_Q(LineSurface);
    if (!document || !control) return;
    bool focus = event->type() == QEvent::FocusIn;
    if (!q->isReadOnly())
        q->setCursorVisible(focus);
    control->processEvent(event, QPointF(-xoff, -yoff));
    if (focus) {
        q->q_updateAlignment();
#ifndef QT_NO_IM
        if (focusOnPress && !q->isReadOnly())
            qGuiApp->inputMethod()->show();
        q->connect(QGuiApplication::inputMethod(), SIGNAL(inputDirectionChanged(Qt::LayoutDirection)),
                q, SLOT(q_updateAlignment()));
#endif
    } else {
#ifndef QT_NO_IM
        q->disconnect(QGuiApplication::inputMethod(), SIGNAL(inputDirectionChanged(Qt::LayoutDirection)),
                   q, SLOT(q_updateAlignment()));
#endif
        emit q->editingFinished();
    }
}

void LineSurfacePrivate::addCurrentTextNodeToRoot(TextNodeEngine *engine, QSGTransformNode *root, TextNode *node, TextNodeIterator &it, int startPos)
{
    engine->addToSceneGraph(node, QQuickText::Normal, QColor());
    it = textNodeMap.insert(it, new LineSurfacePrivate::Node(startPos, node));
    ++it;
    root->appendChildNode(node);
}

TextNode *LineSurfacePrivate::createTextNode()
{
    Q_Q(LineSurface);
    TextNode* node = new TextNode(q);
    node->setUseNativeRenderer(renderType == LineSurface::NativeRendering);
    return node;
}

void LineSurface::q_canPasteChanged()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;
    bool old = d->canPaste;
    d->canPaste = d->control->canPaste();
    bool changed = old!=d->canPaste || !d->canPasteValid;
    d->canPasteValid = true;
    if (changed)
        emit canPasteChanged();
}

QString LineSurface::getText(int start, int end) const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QString();

    start = qBound(0, start, d->document->characterCount() - 1);
    end = qBound(0, end, d->document->characterCount() - 1);
    QTextCursor cursor(d->document);
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
#ifndef QT_NO_TEXTHTMLPARSER
    return d->richText
            ? cursor.selectedText()
            : cursor.selection().toPlainText();
#else
    return cursor.selection().toPlainText();
#endif
}

QString LineSurface::getFormattedText(int start, int end) const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QString();


    start = qBound(0, start, d->document->characterCount() - 1);
    end = qBound(0, end, d->document->characterCount() - 1);

    QTextCursor cursor(d->document);
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);

    if (d->richText) {
#ifndef QT_NO_TEXTHTMLPARSER
        return cursor.selection().toHtml();
#else
        return cursor.selection().toPlainText();
#endif
    } else {
        return cursor.selection().toPlainText();
    }
}

void LineSurface::insert(int position, const QString &text)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    if (position < 0 || position >= d->document->characterCount())
        return;
    QTextCursor cursor(d->document);
    cursor.setPosition(position);
    d->richText = d->richText || (d->format == AutoText && Qt::mightBeRichText(text));
    if (d->richText) {
#ifndef QT_NO_TEXTHTMLPARSER
        cursor.insertHtml(text);
#else
        cursor.insertText(text);
#endif
    } else {
        cursor.insertText(text);
    }
    d->control->updateCursorRectangle(false);
}


void LineSurface::remove(int start, int end)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    start = qBound(0, start, d->document->characterCount() - 1);
    end = qBound(0, end, d->document->characterCount() - 1);
    QTextCursor cursor(d->document);
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    d->control->updateCursorRectangle(false);
}

bool LineSurfacePrivate::isLinkHoveredConnected()
{
    Q_Q(LineSurface);
    LV_IS_SIGNAL_CONNECTED(q, LineSurface, linkHovered, (const QString &));
}

QString LineSurface::hoveredLink() const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QString();

    if (const_cast<LineSurfacePrivate *>(d)->isLinkHoveredConnected()) {
        return d->control->hoveredLink();
    } else {
#ifndef QT_NO_CURSOR
        if (QQuickWindow *wnd = window()) {
            QPointF pos = QCursor::pos(wnd->screen()) - wnd->position() - mapToScene(QPointF(0, 0));
            return d->control->anchorAt(pos);
        }
#endif // QT_NO_CURSOR
    }
    return QString();
}

void LineSurface::hoverEnterEvent(QHoverEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    if (d->isLinkHoveredConnected())
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
}

void LineSurface::hoverMoveEvent(QHoverEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    if (d->isLinkHoveredConnected())
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
}

void LineSurface::hoverLeaveEvent(QHoverEvent *event)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    if (d->isLinkHoveredConnected())
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
}

void LineSurface::append(const QString &text)
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;
    QTextCursor cursor(d->document);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::End);

    if (!d->document->isEmpty())
        cursor.insertBlock();

#ifndef QT_NO_TEXTHTMLPARSER
    if (d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(text))) {
        cursor.insertHtml(text);
    } else {
        cursor.insertText(text);
    }
#else
    cursor.insertText(text);
#endif // QT_NO_TEXTHTMLPARSER

    cursor.endEditBlock();
    d->control->updateCursorRectangle(false);
}

QString LineSurface::linkAt(qreal x, qreal y) const
{
    Q_D(const LineSurface);
    if (!d->document || !d->control) return QString();

    return d->control->anchorAt(QPointF(x + topPadding(), y + leftPadding()));
}

qreal LineSurface::padding() const
{
    Q_D(const LineSurface);
    return d->padding();
}

void LineSurface::setPadding(qreal padding)
{
    Q_D(LineSurface);
    if (qFuzzyCompare(d->padding(), padding))
        return;

    d->extra.value().padding = padding;
    updateSize();
    if (isComponentComplete()) {
        d->updateType = LineSurfacePrivate::UpdatePaintNode;
        update();
    }
    emit paddingChanged();
    if (!d->extra.isAllocated() || !d->extra->explicitTopPadding)
        emit topPaddingChanged();
    if (!d->extra.isAllocated() || !d->extra->explicitLeftPadding)
        emit leftPaddingChanged();
    if (!d->extra.isAllocated() || !d->extra->explicitRightPadding)
        emit rightPaddingChanged();
    if (!d->extra.isAllocated() || !d->extra->explicitBottomPadding)
        emit bottomPaddingChanged();
}

void LineSurface::resetPadding()
{
    setPadding(0);
}

qreal LineSurface::topPadding() const
{
    Q_D(const LineSurface);
    if (d->extra.isAllocated() && d->extra->explicitTopPadding)
        return d->extra->topPadding;
    return d->padding();
}

void LineSurface::setTopPadding(qreal padding)
{
    Q_D(LineSurface);
    d->setTopPadding(padding);
}

void LineSurface::resetTopPadding()
{
    Q_D(LineSurface);
    d->setTopPadding(0, true);
}

qreal LineSurface::leftPadding() const
{
    Q_D(const LineSurface);
    if (d->extra.isAllocated() && d->extra->explicitLeftPadding)
        return d->extra->leftPadding;
    return d->padding();
}

void LineSurface::setLeftPadding(qreal padding)
{
    Q_D(LineSurface);
    d->setLeftPadding(padding);
}

void LineSurface::resetLeftPadding()
{
    Q_D(LineSurface);
    d->setLeftPadding(0, true);
}

qreal LineSurface::rightPadding() const
{
    Q_D(const LineSurface);
    if (d->extra.isAllocated() && d->extra->explicitRightPadding)
        return d->extra->rightPadding;
    return d->padding();
}

void LineSurface::setRightPadding(qreal padding)
{
    Q_D(LineSurface);
    d->setRightPadding(padding);
}

void LineSurface::resetRightPadding()
{
    Q_D(LineSurface);
    d->setRightPadding(0, true);
}

qreal LineSurface::bottomPadding() const
{
    Q_D(const LineSurface);
    if (d->extra.isAllocated() && d->extra->explicitBottomPadding)
        return d->extra->bottomPadding;
    return d->padding();
}

void LineSurface::setBottomPadding(qreal padding)
{
    Q_D(LineSurface);
    d->setBottomPadding(padding);
}

void LineSurface::resetBottomPadding()
{
    Q_D(LineSurface);
    d->setBottomPadding(0, true);
}

void LineSurface::clear()
{
    Q_D(LineSurface);
    if (!d->document || !d->control) return;

    d->resetInputMethod();
    d->control->clear();
}


void LineSurfacePrivate::implicitWidthChanged()
{
    Q_Q(LineSurface);
    QQuickImplicitSizeItemPrivate::implicitWidthChanged();
    emit q->implicitWidthChanged2();
}

void LineSurfacePrivate::implicitHeightChanged()
{
    Q_Q(LineSurface);
    QQuickImplicitSizeItemPrivate::implicitHeightChanged();
    emit q->implicitHeightChanged2();
}

DocumentHandler* LineSurface::documentHandler()
{
    Q_D(LineSurface);
    return d->documentHandler;
}

void LineSurface::setDocumentHandler(DocumentHandler *dh)
{
    Q_D(LineSurface);
    d->documentHandler = dh;
    // dh->setTextEdit(this);
}

void LineSurface::clearSelectionOnFocus(bool value){
    Q_D(LineSurface);
    d->control->clearSelectionOnFocus(value);
}

void LineSurface::setTextDocument(QTextDocument *td)
{
    Q_D(LineSurface);
    if (td)
    {
        d->setTextDocument(td);
        // QTimer::singleShot(100, this, SLOT(singleShotUpdate()));
    } else d->unsetTextDocument();
}

void LineSurface::collapseLines(int pos, int num, QString &replacement)
{
    Q_UNUSED(replacement)
    showHideLines(false, pos, num);
}

void LineSurface::expandLines(int pos, int num, QString &replacement)
{
    Q_UNUSED(replacement)
    showHideLines(true, pos, num);
}

void LineSurface::showHideLines(bool show, int pos, int num)
{
    Q_D(LineSurface);
    auto it = d->document->rootFrame()->begin();
    Q_ASSERT(d->document->blockCount() > pos);
    Q_ASSERT(d->document->blockCount() >= pos + num);
    for (int i = 0; i < pos+1; i++, ++it);
    int start = it.currentBlock().position();

    int length = 0;
    for (int i = 0; i < num; i++)
    {
        it.currentBlock().setVisible(show);
        length += it.currentBlock().length();
        ++it;
    }

    d->document->markContentsDirty(start, length);
}

void LineSurface::updateFragmentVisibility()
{
    Q_D(LineSurface);
    if (!d->document || fragmentStart() == -1 || fragmentEnd() == -1) return;

    auto it = d->document->rootFrame()->begin(); int cnt = 0;
    auto endIt = d->document->rootFrame()->end();

    qDebug() << fragmentStart() << fragmentEnd();

    while (it != endIt)
    {
        if (cnt < fragmentStart()) it.currentBlock().setVisible(false);
        else if (cnt < fragmentEnd()) it.currentBlock().setVisible(true);
        else it.currentBlock().setVisible(false);

        qDebug() << it.currentBlock().text() << it.currentBlock().isVisible();

        ++cnt; ++it;
    }

    emit dirtyBlockPosition(0);
    emit textDocumentFinishedUpdating();

}

void LineSurface::replaceTextInBlock(int blockNumber, std::string s)
{
    Q_D(LineSurface);
    QTextBlock b = d->document->findBlockByNumber(blockNumber);
    QTextCursor cursor(b);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(QString(s.c_str()));
    cursor.endEditBlock();
}

}
