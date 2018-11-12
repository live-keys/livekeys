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

#include "textedit_p.h"
#include "textedit_p_p.h"
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

#include <QtGlobal>

#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
#include <QSGRectangleNode> // change for 5.11
#endif

// <REMOVED>
// This might be copied
#include "private/qquicktextnode_p.h"
// <TOREMOVE>
// This might deal with the whole event system behind QQuickTextEdit, or
// it may not :)
#include "private/qquickevents_p_p.h"

//#include "private/qqmlglobal_p.h"

// <TOREMOVE>
// TODO: Document where it's used
#include "private/qqmlproperty_p.h"

// <TOREMOVE> : just copy
#include "private/qtextengine_p.h"
#include "private/qsgadaptationlayer_p.h"

#include <algorithm>

namespace lv {

/*!
    \qmltype TextEdit
    \instantiates QQuickTextEdit
    \inqmlmodule QtQuick
    \ingroup qtquick-visual
    \ingroup qtquick-input
    \inherits Item
    \brief Displays multiple lines of editable formatted text

    The TextEdit item displays a block of editable, formatted text.

    It can display both plain and rich text. For example:

    \qml
TextEdit {
    width: 240
    text: "<b>Hello</b> <i>World!</i>"
    font.family: "Helvetica"
    font.pointSize: 20
    color: "blue"
    focus: true
}
    \endqml

    \image declarative-textedit.gif

    Setting \l {Item::focus}{focus} to \c true enables the TextEdit item to receive keyboard focus.

    Note that the TextEdit does not implement scrolling, following the cursor, or other behaviors specific
    to a look-and-feel. For example, to add flickable scrolling that follows the cursor:

    \snippet qml/texteditor.qml 0

    A particular look-and-feel might use smooth scrolling (eg. using SmoothedAnimation), might have a visible
    scrollbar, or a scrollbar that fades in to show location, etc.

    Clipboard support is provided by the cut(), copy(), and paste() functions, and the selection can
    be handled in a traditional "mouse" mechanism by setting selectByMouse, or handled completely
    from QML by manipulating selectionStart and selectionEnd, or using selectAll() or selectWord().

    You can translate between cursor positions (characters from the start of the document) and pixel
    points using positionAt() and positionToRectangle().

    \sa Text, TextInput
*/

/*!
    \qmlsignal QtQuick::TextEdit::linkActivated(string link)

    This signal is emitted when the user clicks on a link embedded in the text.
    The link must be in rich text or HTML format and the
    \a link string provides access to the particular link.

    The corresponding handler is \c onLinkActivated.
*/

// This is a pretty arbitrary figure. The idea is that we don't want to break down the document
// into text nodes corresponding to a text block each so that the glyph node grouping doesn't become pointless.
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

TextEdit::TextEdit(QQuickImplicitSizeItem *parent)
: QQuickImplicitSizeItem(*(new TextEditPrivate), parent)
{
    Q_D(TextEdit);
    d->init();
}

TextEdit::TextEdit(TextEditPrivate &dd, QQuickImplicitSizeItem *parent)
: QQuickImplicitSizeItem(dd, parent)
{
    Q_D(TextEdit);
    d->init();
}

QString TextEdit::text() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return QString();
    if (!d->textCached && isComponentComplete()) {
        TextEditPrivate *d = const_cast<TextEditPrivate *>(d_func());
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

/*!
    \qmlproperty string QtQuick::TextEdit::font.family

    Sets the family name of the font.

    The family name is case insensitive and may optionally include a foundry name, e.g. "Helvetica [Cronyx]".
    If the family is available from more than one foundry and the foundry isn't specified, an arbitrary foundry is chosen.
    If the family isn't available a family will be set using the font matching algorithm.
*/

/*!
    \qmlproperty string QtQuick::TextEdit::font.styleName
    \since 5.6

    Sets the style name of the font.

    The style name is case insensitive. If set, the font will be matched against style name instead
    of the font properties \l font.weight, \l font.bold and \l font.italic.
*/


/*!
    \qmlproperty bool QtQuick::TextEdit::font.bold

    Sets whether the font weight is bold.
*/

/*!
    \qmlproperty enumeration QtQuick::TextEdit::font.weight

    Sets the font's weight.

    The weight can be one of:
    \list
    \li Font.Thin
    \li Font.Light
    \li Font.ExtraLight
    \li Font.Normal - the default
    \li Font.Medium
    \li Font.DemiBold
    \li Font.Bold
    \li Font.ExtraBold
    \li Font.Black
    \endlist

    \qml
    TextEdit { text: "Hello"; font.weight: Font.DemiBold }
    \endqml
*/

/*!
    \qmlproperty bool QtQuick::TextEdit::font.italic

    Sets whether the font has an italic style.
*/

/*!
    \qmlproperty bool QtQuick::TextEdit::font.underline

    Sets whether the text is underlined.
*/

/*!
    \qmlproperty bool QtQuick::TextEdit::font.strikeout

    Sets whether the font has a strikeout style.
*/

/*!
    \qmlproperty real QtQuick::TextEdit::font.pointSize

    Sets the font size in points. The point size must be greater than zero.
*/

/*!
    \qmlproperty int QtQuick::TextEdit::font.pixelSize

    Sets the font size in pixels.

    Using this function makes the font device dependent.  Use
    \l{TextEdit::font.pointSize} to set the size of the font in a
    device independent manner.
*/

/*!
    \qmlproperty real QtQuick::TextEdit::font.letterSpacing

    Sets the letter spacing for the font.

    Letter spacing changes the default spacing between individual letters in the font.
    A positive value increases the letter spacing by the corresponding pixels; a negative value decreases the spacing.
*/

/*!
    \qmlproperty real QtQuick::TextEdit::font.wordSpacing

    Sets the word spacing for the font.

    Word spacing changes the default spacing between individual words.
    A positive value increases the word spacing by a corresponding amount of pixels,
    while a negative value decreases the inter-word spacing accordingly.
*/

/*!
    \qmlproperty enumeration QtQuick::TextEdit::font.capitalization

    Sets the capitalization for the text.

    \list
    \li Font.MixedCase - This is the normal text rendering option where no capitalization change is applied.
    \li Font.AllUppercase - This alters the text to be rendered in all uppercase type.
    \li Font.AllLowercase - This alters the text to be rendered in all lowercase type.
    \li Font.SmallCaps - This alters the text to be rendered in small-caps type.
    \li Font.Capitalize - This alters the text to be rendered with the first character of each word as an uppercase character.
    \endlist

    \qml
    TextEdit { text: "Hello"; font.capitalization: Font.AllLowercase }
    \endqml
*/

/*!
    \qmlproperty string QtQuick::TextEdit::text

    The text to display.  If the text format is AutoText the text edit will
    automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().

    The text-property is mostly suitable for setting the initial content and
    handling modifications to relatively small text content. The append(),
    insert() and remove() methods provide more fine-grained control and
    remarkably better performance for modifying especially large rich text
    content.

    \sa clear()
*/
void TextEdit::setText(const QString &text)
{

    Q_D(TextEdit);
    if (TextEdit::text() == text)
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

/*!
    \qmlproperty string QtQuick::TextEdit::preeditText
    \readonly
    \since 5.7

    This property contains partial text input from an input method.
*/
QString TextEdit::preeditText() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return QString();
    return d->control->preeditText();
}

/*!
    \qmlproperty enumeration QtQuick::TextEdit::textFormat

    The way the text property should be displayed.

    \list
    \li TextEdit.AutoText
    \li TextEdit.PlainText
    \li TextEdit.RichText
    \endlist

    The default is TextEdit.PlainText.  If the text format is TextEdit.AutoText the text edit
    will automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().

    \table
    \row
    \li
    \qml
Column {
    TextEdit {
        font.pointSize: 24
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.pointSize: 24
        textFormat: TextEdit.RichText
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.pointSize: 24
        textFormat: TextEdit.PlainText
        text: "<b>Hello</b> <i>World!</i>"
    }
}
    \endqml
    \li \image declarative-textformat.png
    \endtable
*/
TextEdit::TextFormat TextEdit::textFormat() const
{
    Q_D(const TextEdit);
    return d->format;
}

void TextEdit::setTextFormat(TextFormat format)
{
    Q_D(TextEdit);

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

/*!
    \qmlproperty enumeration QtQuick::TextEdit::renderType

    Override the default rendering type for this component.

    Supported render types are:
    \list
    \li Text.QtRendering - the default
    \li Text.NativeRendering
    \endlist

    Select Text.NativeRendering if you prefer text to look native on the target platform and do
    not require advanced features such as transformation of the text. Using such features in
    combination with the NativeRendering render type will lend poor and sometimes pixelated
    results.
*/
TextEdit::RenderType TextEdit::renderType() const
{
    Q_D(const TextEdit);
    return d->renderType;
}

void TextEdit::setRenderType(TextEdit::RenderType renderType)
{
    Q_D(TextEdit);
    if (d->renderType == renderType)
        return;

    d->renderType = renderType;
    emit renderTypeChanged();
    d->updateDefaultTextOption();

    if (isComponentComplete())
        updateSize();
}

QFont TextEdit::font() const
{
    Q_D(const TextEdit);
    return d->sourceFont;
}

void TextEdit::setFont(const QFont &font)
{
    Q_D(TextEdit);
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

/*!
    \qmlproperty color QtQuick::TextEdit::color

    The text color.

    \qml
    // green text using hexadecimal notation
    TextEdit { color: "#00FF00" }
    \endqml

    \qml
    // steelblue text using SVG color name
    TextEdit { color: "steelblue" }
    \endqml
*/
QColor TextEdit::color() const
{
    Q_D(const TextEdit);
    return d->color;
}

void TextEdit::setColor(const QColor &color)
{
    Q_D(TextEdit);
    if (d->color == color)
        return;

    d->color = color;

    updateWholeDocument();
    emit colorChanged(d->color);
}

/*!
    \qmlproperty color QtQuick::TextEdit::selectionColor

    The text highlight color, used behind selections.
*/
QColor TextEdit::selectionColor() const
{
    Q_D(const TextEdit);
    return d->selectionColor;
}

void TextEdit::setSelectionColor(const QColor &color)
{
    Q_D(TextEdit);
    if (d->selectionColor == color)
        return;

    d->selectionColor = color;
    updateWholeDocument();
    emit selectionColorChanged(d->selectionColor);
}

/*!
    \qmlproperty color QtQuick::TextEdit::selectedTextColor

    The selected text color, used in selections.
*/
QColor TextEdit::selectedTextColor() const
{
    Q_D(const TextEdit);
    return d->selectedTextColor;
}

void TextEdit::setSelectedTextColor(const QColor &color)
{
    Q_D(TextEdit);
    if (d->selectedTextColor == color)
        return;

    d->selectedTextColor = color;
    updateWholeDocument();
    emit selectedTextColorChanged(d->selectedTextColor);
}

int TextEdit::fragmentStart() const {
    Q_D(const TextEdit);
    return d->fragmentStart;
}

int TextEdit::fragmentEnd() const {
    Q_D(const TextEdit);
    return d->fragmentEnd;
}

void TextEdit::setFragmentStart(int frStart) {
    Q_D(TextEdit);
    d->fragmentStart = frStart;
}

void TextEdit::setFragmentEnd(int frEnd) {
    Q_D(TextEdit);
    d->fragmentEnd = frEnd;
}

void TextEdit::resetFragmentStart() {
    setFragmentStart(-1);
}

void TextEdit::resetFragmentEnd() {
    setFragmentEnd(-1);
}

/*!
    \qmlproperty enumeration QtQuick::TextEdit::horizontalAlignment
    \qmlproperty enumeration QtQuick::TextEdit::verticalAlignment
    \qmlproperty enumeration QtQuick::TextEdit::effectiveHorizontalAlignment

    Sets the horizontal and vertical alignment of the text within the TextEdit item's
    width and height. By default, the text alignment follows the natural alignment
    of the text, for example text that is read from left to right will be aligned to
    the left.

    Valid values for \c horizontalAlignment are:
    \list
    \li TextEdit.AlignLeft (default)
    \li TextEdit.AlignRight
    \li TextEdit.AlignHCenter
    \li TextEdit.AlignJustify
    \endlist

    Valid values for \c verticalAlignment are:
    \list
    \li TextEdit.AlignTop (default)
    \li TextEdit.AlignBottom
    \li TextEdit.AlignVCenter
    \endlist

    When using the attached property LayoutMirroring::enabled to mirror application
    layouts, the horizontal alignment of text will also be mirrored. However, the property
    \c horizontalAlignment will remain unchanged. To query the effective horizontal alignment
    of TextEdit, use the read-only property \c effectiveHorizontalAlignment.
*/
TextEdit::HAlignment TextEdit::hAlign() const
{
    Q_D(const TextEdit);
    return d->hAlign;
}

void TextEdit::setHAlign(HAlignment align)
{
    Q_D(TextEdit);
    bool forceAlign = d->hAlignImplicit && d->effectiveLayoutMirror;
    d->hAlignImplicit = false;
    if (d->setHAlign(align, forceAlign) && isComponentComplete()) {
        d->updateDefaultTextOption();
        updateSize();
    }
}

void TextEdit::resetHAlign()
{
    Q_D(TextEdit);
    d->hAlignImplicit = true;
    if (d->determineHorizontalAlignment() && isComponentComplete()) {
        d->updateDefaultTextOption();
        updateSize();
    }
}

TextEdit::HAlignment TextEdit::effectiveHAlign() const
{
    Q_D(const TextEdit);
    TextEdit::HAlignment effectiveAlignment = d->hAlign;
    if (!d->hAlignImplicit && d->effectiveLayoutMirror) {
        switch (d->hAlign) {
        case TextEdit::AlignLeft:
            effectiveAlignment = TextEdit::AlignRight;
            break;
        case TextEdit::AlignRight:
            effectiveAlignment = TextEdit::AlignLeft;
            break;
        default:
            break;
        }
    }
    return effectiveAlignment;
}

bool TextEditPrivate::setHAlign(TextEdit::HAlignment alignment, bool forceAlign)
{
    Q_Q(TextEdit);
    if (hAlign != alignment || forceAlign) {
        TextEdit::HAlignment oldEffectiveHAlign = q->effectiveHAlign();
        hAlign = alignment;
        emit q->horizontalAlignmentChanged(alignment);
        if (oldEffectiveHAlign != q->effectiveHAlign())
            emit q->effectiveHorizontalAlignmentChanged();
        return true;
    }
    return false;
}


Qt::LayoutDirection TextEditPrivate::textDirection(const QString &text) const
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

bool TextEditPrivate::determineHorizontalAlignment()
{
    Q_Q(TextEdit);
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

        return setHAlign(direction == Qt::RightToLeft ? TextEdit::AlignRight : TextEdit::AlignLeft);
    }
    return false;
}

void TextEditPrivate::mirrorChange()
{
    Q_Q(TextEdit);
    if (q->isComponentComplete()) {
        if (!hAlignImplicit && (hAlign == TextEdit::AlignRight || hAlign == TextEdit::AlignLeft)) {
            updateDefaultTextOption();
            q->updateSize();
            emit q->effectiveHorizontalAlignmentChanged();
        }
    }
}

#ifndef QT_NO_IM
Qt::InputMethodHints TextEditPrivate::effectiveInputMethodHints() const
{
    return inputMethodHints | Qt::ImhMultiLine;
}
#endif

void TextEditPrivate::setTopPadding(qreal value, bool reset)
{
    Q_Q(TextEdit);
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

void TextEditPrivate::setLeftPadding(qreal value, bool reset)
{
    Q_Q(TextEdit);
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

void TextEditPrivate::setRightPadding(qreal value, bool reset)
{
    Q_Q(TextEdit);
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

void TextEditPrivate::setBottomPadding(qreal value, bool reset)
{
    Q_Q(TextEdit);
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

bool TextEditPrivate::isImplicitResizeEnabled() const
{
    return !extra.isAllocated() || extra->implicitResize;
}

void TextEditPrivate::setImplicitResizeEnabled(bool enabled)
{
    if (!enabled)
        extra.value().implicitResize = false;
    else if (extra.isAllocated())
        extra->implicitResize = true;
}

TextEdit::VAlignment TextEdit::vAlign() const
{
    Q_D(const TextEdit);
    return d->vAlign;
}

void TextEdit::setVAlign(TextEdit::VAlignment alignment)
{
    Q_D(TextEdit);
    if (alignment == d->vAlign)
        return;
    d->vAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
    moveCursorDelegate();
    emit verticalAlignmentChanged(d->vAlign);
}
/*!
    \qmlproperty enumeration QtQuick::TextEdit::wrapMode

    Set this property to wrap the text to the TextEdit item's width.
    The text will only wrap if an explicit width has been set.

    \list
    \li TextEdit.NoWrap - no wrapping will be performed. If the text contains insufficient newlines, then implicitWidth will exceed a set width.
    \li TextEdit.WordWrap - wrapping is done on word boundaries only. If a word is too long, implicitWidth will exceed a set width.
    \li TextEdit.WrapAnywhere - wrapping is done at any point on a line, even if it occurs in the middle of a word.
    \li TextEdit.Wrap - if possible, wrapping occurs at a word boundary; otherwise it will occur at the appropriate point on the line, even in the middle of a word.
    \endlist

    The default is TextEdit.NoWrap. If you set a width, consider using TextEdit.Wrap.
*/
TextEdit::WrapMode TextEdit::wrapMode() const
{
    Q_D(const TextEdit);
    return d->wrapMode;
}

void TextEdit::setWrapMode(WrapMode mode)
{
    Q_D(TextEdit);
    if (mode == d->wrapMode)
        return;
    d->wrapMode = mode;
    d->updateDefaultTextOption();
    updateSize();
    emit wrapModeChanged();
}

/*!
    \qmlproperty int QtQuick::TextEdit::lineCount

    Returns the total number of lines in the textEdit item.
*/
int TextEdit::lineCount() const
{
    Q_D(const TextEdit);
    return d->lineCount;
}

/*!
    \qmlproperty int QtQuick::TextEdit::length

    Returns the total number of plain text characters in the TextEdit item.

    As this number doesn't include any formatting markup it may not be the same as the
    length of the string returned by the \l text property.

    This property can be faster than querying the length the \l text property as it doesn't
    require any copying or conversion of the TextEdit's internal string data.
*/

int TextEdit::length() const
{
    Q_D(const TextEdit);
    if (!d->document) return 0;
    // QTextDocument::characterCount() includes the terminating null character.
    return qMax(0, d->document->characterCount() - 1);
}

/*!
    \qmlproperty real QtQuick::TextEdit::contentWidth

    Returns the width of the text, including the width past the width
    which is covered due to insufficient wrapping if \l wrapMode is set.
*/
qreal TextEdit::contentWidth() const
{
    Q_D(const TextEdit);
    return d->contentSize.width();
}

/*!
    \qmlproperty real QtQuick::TextEdit::contentHeight

    Returns the height of the text, including the height past the height
    that is covered if the text does not fit within the set height.
*/
qreal TextEdit::contentHeight() const
{
    Q_D(const TextEdit);
    return d->contentSize.height();
}

/*!
    \qmlproperty url QtQuick::TextEdit::baseUrl

    This property specifies a base URL which is used to resolve relative URLs
    within the text.

    The default value is the url of the QML file instantiating the TextEdit item.
*/

QUrl TextEdit::baseUrl() const
{
    Q_D(const TextEdit);
    if (d->baseUrl.isEmpty()) {
        if (QQmlContext *context = qmlContext(this))
            const_cast<TextEditPrivate *>(d)->baseUrl = context->baseUrl();
    }
    return d->baseUrl;
}

void TextEdit::setBaseUrl(const QUrl &url)
{
    Q_D(TextEdit);
    if (baseUrl() != url) {
        d->baseUrl = url;

        if (d->document) d->document->setBaseUrl(url);
        emit baseUrlChanged();
    }
}

void TextEdit::resetBaseUrl()
{
    if (QQmlContext *context = qmlContext(this))
        setBaseUrl(context->baseUrl());
    else
        setBaseUrl(QUrl());
}

/*!
    \qmlmethod rectangle QtQuick::TextEdit::positionToRectangle(position)

    Returns the rectangle at the given \a position in the text. The x, y,
    and height properties correspond to the cursor that would describe
    that position.
*/
QRectF TextEdit::positionToRectangle(int pos) const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return QRectF();
    QTextCursor c(d->document);
    c.setPosition(pos);
    return d->control->cursorRect(c).translated(d->xoff, d->yoff);

}

/*!
    \qmlmethod int QtQuick::TextEdit::positionAt(int x, int y)

    Returns the text position closest to pixel position (\a x, \a y).

    Position 0 is before the first character, position 1 is after the first character
    but before the second, and so on until position \l {text}.length, which is after all characters.
*/
int TextEdit::positionAt(qreal x, qreal y) const
{
    Q_D(const TextEdit);
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

/*!
    \qmlmethod QtQuick::TextEdit::moveCursorSelection(int position, SelectionMode mode = TextEdit.SelectCharacters)

    Moves the cursor to \a position and updates the selection according to the optional \a mode
    parameter. (To only move the cursor, set the \l cursorPosition property.)

    When this method is called it additionally sets either the
    selectionStart or the selectionEnd (whichever was at the previous cursor position)
    to the specified position. This allows you to easily extend and contract the selected
    text range.

    The selection mode specifies whether the selection is updated on a per character or a per word
    basis.  If not specified the selection mode will default to TextEdit.SelectCharacters.

    \list
    \li TextEdit.SelectCharacters - Sets either the selectionStart or selectionEnd (whichever was at
    the previous cursor position) to the specified position.
    \li TextEdit.SelectWords - Sets the selectionStart and selectionEnd to include all
    words between the specified position and the previous cursor position.  Words partially in the
    range are included.
    \endlist

    For example, take this sequence of calls:

    \code
        cursorPosition = 5
        moveCursorSelection(9, TextEdit.SelectCharacters)
        moveCursorSelection(7, TextEdit.SelectCharacters)
    \endcode

    This moves the cursor to position 5, extend the selection end from 5 to 9
    and then retract the selection end from 9 to 7, leaving the text from position 5 to 7
    selected (the 6th and 7th characters).

    The same sequence with TextEdit.SelectWords will extend the selection start to a word boundary
    before or on position 5 and extend the selection end to a word boundary on or past position 9.
*/
void TextEdit::moveCursorSelection(int pos)
{
    //Note that this is the same as setCursorPosition but with the KeepAnchor flag set
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    d->control->setTextCursor(cursor);
}

void TextEdit::moveCursorSelection(int pos, SelectionMode mode)
{
    Q_D(TextEdit);
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

/*!
    \qmlproperty bool QtQuick::TextEdit::cursorVisible
    If true the text edit shows a cursor.

    This property is set and unset when the text edit gets active focus, but it can also
    be set directly (useful, for example, if a KeyProxy might forward keys to it).
*/
bool TextEdit::isCursorVisible() const
{
    Q_D(const TextEdit);
    return d->cursorVisible;
}

void TextEdit::setCursorVisible(bool on)
{
    Q_D(TextEdit);
    d->cursorVisible = on;
    if (on && isComponentComplete())
        TextUtil::createCursor(d);
    if (!on && !d->persistentSelection)
        if (d->control) d->control->setCursorIsFocusIndicator(true);
    if (d->control) d->control->setCursorVisible(on);
    emit cursorVisibleChanged(d->cursorVisible);
}

/*!
    \qmlproperty int QtQuick::TextEdit::cursorPosition
    The position of the cursor in the TextEdit.
*/
int TextEdit::cursorPosition() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return -1;

    return d->control->textCursor().position();
}

void TextEdit::setCursorPosition(int pos)
{
    Q_D(TextEdit);
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

/*!
    \qmlproperty Component QtQuick::TextEdit::cursorDelegate
    The delegate for the cursor in the TextEdit.

    If you set a cursorDelegate for a TextEdit, this delegate will be used for
    drawing the cursor instead of the standard cursor. An instance of the
    delegate will be created and managed by the text edit when a cursor is
    needed, and the x and y properties of delegate instance will be set so as
    to be one pixel before the top left of the current character.

    Note that the root item of the delegate component must be a QQuickItem or
    QQuickItem derived item.
*/
QQmlComponent* TextEdit::cursorDelegate() const
{
    Q_D(const TextEdit);
    return d->cursorComponent;
}

void TextEdit::setCursorDelegate(QQmlComponent* c)
{
    Q_D(TextEdit);
    TextUtil::setCursorDelegate(d, c);
}

void TextEdit::createCursor()
{
    Q_D(TextEdit);
    d->cursorPending = true;
    TextUtil::createCursor(d);
}

/*!
    \qmlproperty int QtQuick::TextEdit::selectionStart

    The cursor position before the first character in the current selection.

    This property is read-only. To change the selection, use select(start,end),
    selectAll(), or selectWord().

    \sa selectionEnd, cursorPosition, selectedText
*/
int TextEdit::selectionStart() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return -1;

    return d->control->textCursor().selectionStart();
}

/*!
    \qmlproperty int QtQuick::TextEdit::selectionEnd

    The cursor position after the last character in the current selection.

    This property is read-only. To change the selection, use select(start,end),
    selectAll(), or selectWord().

    \sa selectionStart, cursorPosition, selectedText
*/
int TextEdit::selectionEnd() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return -1;

    return d->control->textCursor().selectionEnd();
}

/*!
    \qmlproperty string QtQuick::TextEdit::selectedText

    This read-only property provides the text currently selected in the
    text edit.

    It is equivalent to the following snippet, but is faster and easier
    to use.
    \code
    //myTextEdit is the id of the TextEdit
    myTextEdit.text.toString().substring(myTextEdit.selectionStart,
            myTextEdit.selectionEnd);
    \endcode
*/
QString TextEdit::selectedText() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return QString();

#ifndef QT_NO_TEXTHTMLPARSER
    return d->richText
            ? d->control->textCursor().selectedText()
            : d->control->textCursor().selection().toPlainText();
#else
    return d->control->textCursor().selection().toPlainText();
#endif
}

/*!
    \qmlproperty bool QtQuick::TextEdit::activeFocusOnPress

    Whether the TextEdit should gain active focus on a mouse press. By default this is
    set to true.
*/
bool TextEdit::focusOnPress() const
{
    Q_D(const TextEdit);
    return d->focusOnPress;
}

void TextEdit::setFocusOnPress(bool on)
{
    Q_D(TextEdit);
    if (d->focusOnPress == on)
        return;
    d->focusOnPress = on;
    emit activeFocusOnPressChanged(d->focusOnPress);
}

/*!
    \qmlproperty bool QtQuick::TextEdit::persistentSelection

    Whether the TextEdit should keep the selection visible when it loses active focus to another
    item in the scene. By default this is set to false.
*/
bool TextEdit::persistentSelection() const
{
    Q_D(const TextEdit);
    return d->persistentSelection;
}

void TextEdit::setPersistentSelection(bool on)
{
    Q_D(TextEdit);
    if (d->persistentSelection == on)
        return;
    d->persistentSelection = on;
    emit persistentSelectionChanged(d->persistentSelection);
}

/*!
   \qmlproperty real QtQuick::TextEdit::textMargin

   The margin, in pixels, around the text in the TextEdit.
*/
qreal TextEdit::textMargin() const
{
    Q_D(const TextEdit);
    return d->textMargin;
}

void TextEdit::setTextMargin(qreal margin)
{
    Q_D(TextEdit);

    if (abs(d->textMargin - margin) < LV_ACCURACY)
        return;
    d->textMargin = margin;
    if (!d->document) d->document->setDocumentMargin(d->textMargin);
    emit textMarginChanged(d->textMargin);
}

/*!
    \qmlproperty enumeration QtQuick::TextEdit::inputMethodHints

    Provides hints to the input method about the expected content of the text edit and how it
    should operate.

    The value is a bit-wise combination of flags or Qt.ImhNone if no hints are set.

    Flags that alter behaviour are:

    \list
    \li Qt.ImhHiddenText - Characters should be hidden, as is typically used when entering passwords.
    \li Qt.ImhSensitiveData - Typed text should not be stored by the active input method
            in any persistent storage like predictive user dictionary.
    \li Qt.ImhNoAutoUppercase - The input method should not try to automatically switch to upper case
            when a sentence ends.
    \li Qt.ImhPreferNumbers - Numbers are preferred (but not required).
    \li Qt.ImhPreferUppercase - Upper case letters are preferred (but not required).
    \li Qt.ImhPreferLowercase - Lower case letters are preferred (but not required).
    \li Qt.ImhNoPredictiveText - Do not use predictive text (i.e. dictionary lookup) while typing.

    \li Qt.ImhDate - The text editor functions as a date field.
    \li Qt.ImhTime - The text editor functions as a time field.
    \endlist

    Flags that restrict input (exclusive flags) are:

    \list
    \li Qt.ImhDigitsOnly - Only digits are allowed.
    \li Qt.ImhFormattedNumbersOnly - Only number input is allowed. This includes decimal point and minus sign.
    \li Qt.ImhUppercaseOnly - Only upper case letter input is allowed.
    \li Qt.ImhLowercaseOnly - Only lower case letter input is allowed.
    \li Qt.ImhDialableCharactersOnly - Only characters suitable for phone dialing are allowed.
    \li Qt.ImhEmailCharactersOnly - Only characters suitable for email addresses are allowed.
    \li Qt.ImhUrlCharactersOnly - Only characters suitable for URLs are allowed.
    \endlist

    Masks:

    \list
    \li Qt.ImhExclusiveInputMask - This mask yields nonzero if any of the exclusive flags are used.
    \endlist
*/

Qt::InputMethodHints TextEdit::inputMethodHints() const
{
#ifdef QT_NO_IM
    return Qt::ImhNone;
#else
    Q_D(const TextEdit);
    return d->inputMethodHints;
#endif // QT_NO_IM
}

void TextEdit::setInputMethodHints(Qt::InputMethodHints hints)
{
#ifdef QT_NO_IM
    Q_UNUSED(hints);
#else
    Q_D(TextEdit);

    if (hints == d->inputMethodHints)
        return;

    d->inputMethodHints = hints;
    updateInputMethod(Qt::ImHints);
    emit inputMethodHintsChanged();
#endif // QT_NO_IM
}

void TextEdit::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    Q_D(TextEdit);
    if (!d->inLayout && ((abs(newGeometry.width() - oldGeometry.width()) > LV_ACCURACY && widthValid())
        || (abs(newGeometry.height() - oldGeometry.height()) > LV_ACCURACY && heightValid()))) {
        updateSize();
        updateWholeDocument();
        moveCursorDelegate();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

}

/*!
    Ensures any delayed caching or data loading the class
    needs to performed is complete.
*/
void TextEdit::componentComplete()
{
    Q_D(TextEdit);

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

/*!
    \qmlproperty bool QtQuick::TextEdit::selectByKeyboard
    \since 5.1

    Defaults to true when the editor is editable, and false
    when read-only.

    If true, the user can use the keyboard to select text
    even if the editor is read-only. If false, the user
    cannot use the keyboard to select text even if the
    editor is editable.

    \sa readOnly
*/
bool TextEdit::selectByKeyboard() const
{
    Q_D(const TextEdit);
    if (d->selectByKeyboardSet)
        return d->selectByKeyboard;
    return !isReadOnly();
}

void TextEdit::setSelectByKeyboard(bool on)
{
    Q_D(TextEdit);
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

/*!
    \qmlproperty bool QtQuick::TextEdit::selectByMouse

    Defaults to false.

    If true, the user can use the mouse to select text in some
    platform-specific way. Note that for some platforms this may
    not be an appropriate interaction; it may conflict with how
    the text needs to behave inside a Flickable, for example.
*/
bool TextEdit::selectByMouse() const
{
    Q_D(const TextEdit);
    return d->selectByMouse;
}

void TextEdit::setSelectByMouse(bool on)
{
    Q_D(TextEdit);

    d->selectByMouse = on;
    setKeepMouseGrab(on);
    if (on) {
        if (d->control) d->control->setTextInteractionFlags(d->control->textInteractionFlags() | Qt::TextSelectableByMouse);
    }
    else if (d->control)
        d->control->setTextInteractionFlags(d->control->textInteractionFlags() & ~Qt::TextSelectableByMouse);
    emit selectByMouseChanged(on);

}

/*!
    \qmlproperty enumeration QtQuick::TextEdit::mouseSelectionMode

    Specifies how text should be selected using a mouse.

    \list
    \li TextEdit.SelectCharacters - The selection is updated with individual characters. (Default)
    \li TextEdit.SelectWords - The selection is updated with whole words.
    \endlist

    This property only applies when \l selectByMouse is true.
*/
TextEdit::SelectionMode TextEdit::mouseSelectionMode() const
{
    Q_D(const TextEdit);
    return d->mouseSelectionMode;
}

void TextEdit::setMouseSelectionMode(SelectionMode mode)
{
    Q_D(TextEdit);

    d->mouseSelectionMode = mode;
    if (d->control) d->control->setWordSelectionEnabled(mode == SelectWords);
    emit mouseSelectionModeChanged(mode);
}

/*!
    \qmlproperty bool QtQuick::TextEdit::readOnly

    Whether the user can interact with the TextEdit item. If this
    property is set to true the text cannot be edited by user interaction.

    By default this property is false.
*/
void TextEdit::setReadOnly(bool r)
{
    Q_D(TextEdit);

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

bool TextEdit::isReadOnly() const
{
    Q_D(const TextEdit);
    return d->readOnly;
}

/*!
    \qmlproperty rectangle QtQuick::TextEdit::cursorRectangle

    The rectangle where the standard text cursor is rendered
    within the text edit. Read-only.

    The position and height of a custom cursorDelegate are updated to follow the cursorRectangle
    automatically when it changes.  The width of the delegate is unaffected by changes in the
    cursor rectangle.
*/
QRectF TextEdit::cursorRectangle() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return QRectF();

    return d->control->cursorRect().translated(d->xoff, d->yoff);
}

bool TextEdit::event(QEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return QQuickItem::event(event);

    if (event->type() == QEvent::ShortcutOverride) {
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
        return event->isAccepted();
    }
    return QQuickItem::event(event);
}

/*!
\overload
Handles the given key \a event.
*/
void TextEdit::keyPressEvent(QKeyEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control)
    {
        QQuickItem::keyPressEvent(event);
        return;
    }

    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (!event->isAccepted())
        QQuickItem::keyPressEvent(event);
}

/*!
\overload
Handles the given key \a event.
*/
void TextEdit::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control)
    {
        QQuickItem::keyReleaseEvent(event);
        return;
    }
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (!event->isAccepted())
        QQuickItem::keyReleaseEvent(event);
}

/*!
    \qmlmethod QtQuick::TextEdit::deselect()

    Removes active text selection.
*/
void TextEdit::deselect()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    QTextCursor c = d->control->textCursor();
    c.clearSelection();
    d->control->setTextCursor(c);
}

/*!
    \qmlmethod QtQuick::TextEdit::selectAll()

    Causes all text to be selected.
*/
void TextEdit::selectAll()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    d->control->selectAll();
}

/*!
    \qmlmethod QtQuick::TextEdit::selectWord()

    Causes the word closest to the current cursor position to be selected.
*/
void TextEdit::selectWord()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    QTextCursor c = d->control->textCursor();
    c.select(QTextCursor::WordUnderCursor);
    d->control->setTextCursor(c);
}

/*!
    \qmlmethod QtQuick::TextEdit::select(int start, int end)

    Causes the text from \a start to \a end to be selected.

    If either start or end is out of range, the selection is not changed.

    After calling this, selectionStart will become the lesser
    and selectionEnd will become the greater (regardless of the order passed
    to this method).

    \sa selectionStart, selectionEnd
*/
void TextEdit::select(int start, int end)
{
    Q_D(TextEdit);
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

/*!
    \qmlmethod QtQuick::TextEdit::isRightToLeft(int start, int end)

    Returns true if the natural reading direction of the editor text
    found between positions \a start and \a end is right to left.
*/
bool TextEdit::isRightToLeft(int start, int end)
{
    if (start > end) {
        qmlInfo(this) << "isRightToLeft(start, end) called with the end property being smaller than the start.";
        return false;
    } else {
        return getText(start, end).isRightToLeft();
    }
}

#ifndef QT_NO_CLIPBOARD
/*!
    \qmlmethod QtQuick::TextEdit::cut()

    Moves the currently selected text to the system clipboard.
*/
void TextEdit::cut()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    d->control->cut();
}

/*!
    \qmlmethod QtQuick::TextEdit::copy()

    Copies the currently selected text to the system clipboard.
*/
void TextEdit::copy()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    d->control->copy();
}

/*!
    \qmlmethod QtQuick::TextEdit::paste()

    Replaces the currently selected text by the contents of the system clipboard.
*/
void TextEdit::paste()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    d->control->paste();
}
#endif // QT_NO_CLIPBOARD


/*!
    \qmlmethod QtQuick::TextEdit::undo()

    Undoes the last operation if undo is \l {canUndo}{available}. Deselects any
    current selection, and updates the selection start to the current cursor
    position.
*/

void TextEdit::undo()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    d->control->undo();
}

/*!
    \qmlmethod QtQuick::TextEdit::redo()

    Redoes the last operation if redo is \l {canRedo}{available}.
*/

void TextEdit::redo()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    d->control->redo();
}

/*!
\overload
Handles the given mouse \a event.
*/
void TextEdit::mousePressEvent(QMouseEvent *event)
{
    Q_D(TextEdit);
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

/*!
\overload
Handles the given mouse \a event.
*/
void TextEdit::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control)
    {
        QQuickItem::mouseReleaseEvent(event);
        return;
    }
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));

    if (!event->isAccepted())
        QQuickItem::mouseReleaseEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void TextEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control)
    {
        QQuickItem::mouseDoubleClickEvent(event);
        return;
    }
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    if (!event->isAccepted())
        QQuickItem::mouseDoubleClickEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void TextEdit::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(TextEdit);
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
/*!
\overload
Handles the given input method \a event.
*/
void TextEdit::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;
    const bool wasComposing = isInputMethodComposing();
    d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
    setCursorVisible(d->control->cursorVisible());
    if (wasComposing != isInputMethodComposing())
        emit inputMethodComposingChanged();
}

/*!
\overload
Returns the value of the given \a property and \a argument.
*/
QVariant TextEdit::inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const
{
    Q_D(const TextEdit);

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

/*!
\overload
Returns the value of the given \a property.
*/
QVariant TextEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
    return inputMethodQuery(property, QVariant());
}
#endif // QT_NO_IM

void TextEdit::triggerPreprocess()
{
    Q_D(TextEdit);
    if (d->updateType == TextEditPrivate::UpdateNone)
        d->updateType = TextEditPrivate::UpdateOnlyPreprocess;
    polish();
    update();
}

typedef QList<TextEditPrivate::Node*>::iterator TextNodeIterator;


static bool comesBefore(TextEditPrivate::Node* n1, TextEditPrivate::Node* n2)
{
    return n1->startPos() < n2->startPos();
}

static inline void updateNodeTransform(TextNode* node, const QPointF &topLeft)
{
    QMatrix4x4 transformMatrix;
    transformMatrix.translate(static_cast<float>(topLeft.x()), static_cast<float>(topLeft.y()));
    node->setMatrix(transformMatrix);
}

/*!
 * \internal
 *
 * Invalidates font caches owned by the text objects owned by the element
 * to work around the fact that text objects cannot be used from multiple threads.
 */
void TextEdit::invalidateFontCaches()
{
    Q_D(TextEdit);
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

QSGNode *TextEdit::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData);
    Q_D(TextEdit);

    if (!d->document || !d->control) {
        if (oldNode) delete oldNode;
        return nullptr;
    }

    if (d->updateType != TextEditPrivate::UpdatePaintNode && oldNode != nullptr) {
        // Update done in preprocess() in the nodes
        d->updateType = TextEditPrivate::UpdateNone;
        return oldNode;
    }

    d->updateType = TextEditPrivate::UpdateNone;

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
        TextEditPrivate::Node *firstCleanNode = (nodeIterator != d->textNodeMap.end()) ? *nodeIterator : nullptr;

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

                            if (block.blockNumber() > 5)
                            {
                                nodeOffset.setY(nodeOffset.y()-75);
                            }
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
#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
        if (!isReadOnly() && d->cursorVisible && d->control->cursorOn()){ // change for 5.11
            cursor = d->sceneGraphContext()->createRectangleNode();
            cursor->setRect(d->control->cursorRect());
            cursor->setColor(d->color);
        }
#else
        if (!isReadOnly() && d->cursorVisible && d->control->cursorOn())
            cursor = d->sceneGraphContext()->createRectangleNode(d->control->cursorRect(), d->color);
#endif
        rootNode->resetCursorNode(cursor);
    }

    invalidateFontCaches();

    return rootNode;
}

void TextEdit::updatePolish()
{
    invalidateFontCaches();
}

/*!
    \qmlproperty bool QtQuick::TextEdit::canPaste

    Returns true if the TextEdit is writable and the content of the clipboard is
    suitable for pasting into the TextEdit.
*/
bool TextEdit::canPaste() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return false;

    if (!d->canPasteValid) {
        const_cast<TextEditPrivate *>(d)->canPaste = d->control->canPaste();
        const_cast<TextEditPrivate *>(d)->canPasteValid = true;
    }
    return d->canPaste;
}

/*!
    \qmlproperty bool QtQuick::TextEdit::canUndo

    Returns true if the TextEdit is writable and there are previous operations
    that can be undone.
*/

bool TextEdit::canUndo() const
{
    Q_D(const TextEdit);
    if (!d->document) return false;

    return d->document->isUndoAvailable();
}

/*!
    \qmlproperty bool QtQuick::TextEdit::canRedo

    Returns true if the TextEdit is writable and there are \l {undo}{undone}
    operations that can be redone.
*/

bool TextEdit::canRedo() const
{
    Q_D(const TextEdit);
    if (!d->document) return false;

    return d->document->isRedoAvailable();
}

/*!
    \qmlproperty bool QtQuick::TextEdit::inputMethodComposing


    This property holds whether the TextEdit has partial text input from an
    input method.

    While it is composing an input method may rely on mouse or key events from
    the TextEdit to edit or commit the partial text.  This property can be used
    to determine when to disable events handlers that may interfere with the
    correct operation of an input method.
*/
bool TextEdit::isInputMethodComposing() const
{
#ifdef QT_NO_IM
    return false;
#else
    Q_D(const TextEdit);
    if (!d->document || !d->control) return false;
    return d->control->hasImState();
#endif // QT_NO_IM
}

TextEditPrivate::ExtraData::ExtraData()
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

void TextEditPrivate::setTextDocument(QTextDocument *d)
{
    Q_Q(TextEdit);
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
    lv_qmlobject_connect(control, TextControl, SIGNAL(updateCursorRequest()), q, TextEdit, SLOT(updateCursor()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(selectionChanged()), q, TextEdit, SIGNAL(selectedTextChanged()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(selectionChanged()), q, TextEdit, SLOT(updateSelection()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(cursorPositionChanged()), q, TextEdit, SLOT(updateSelection()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(cursorPositionChanged()), q, TextEdit, SIGNAL(cursorPositionChanged()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(cursorRectangleChanged()), q, TextEdit, SLOT(moveCursorDelegate()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(linkActivated(QString)), q, TextEdit, SIGNAL(linkActivated(QString)));
    lv_qmlobject_connect(control, TextControl, SIGNAL(linkHovered(QString)), q, TextEdit, SIGNAL(linkHovered(QString)));
    lv_qmlobject_connect(control, TextControl, SIGNAL(textChanged()), q, TextEdit, SLOT(q_textChanged()));
    lv_qmlobject_connect(control, TextControl, SIGNAL(preeditTextChanged()), q, TextEdit, SIGNAL(preeditTextChanged()));
    /*#if QT_CONFIG(clipboard)
        qmlobject_connect(QGuiApplication::clipboard(), QClipboard, SIGNAL(dataChanged()), q, QQuickTextEdit, SLOT(q_canPasteChanged()));
    #endif*/
    lv_qmlobject_connect(document, QTextDocument, SIGNAL(undoAvailable(bool)), q, TextEdit, SIGNAL(canUndoChanged()));
    lv_qmlobject_connect(document, QTextDocument, SIGNAL(redoAvailable(bool)), q, TextEdit, SIGNAL(canRedoChanged()));



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

    QObject::connect(document, &QTextDocument::contentsChange, q, &TextEdit::q_contentsChange);
    QObject::connect(document->documentLayout(), &QAbstractTextDocumentLayout::updateBlock, q, &TextEdit::invalidateBlock);
    QObject::connect(document->documentLayout(), &QAbstractTextDocumentLayout::update, q, &TextEdit::highlightingDone);
}

void TextEditPrivate::unsetTextDocument()
{
    Q_Q(TextEdit);
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

void TextEditPrivate::init()
{
    Q_Q(TextEdit);

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
    lv_qmlobject_connect(QGuiApplication::clipboard(), QClipboard, SIGNAL(dataChanged()), q, TextEdit, SLOT(q_canPasteChanged()));
#endif

}

void TextEditPrivate::resetInputMethod()
{
    Q_Q(TextEdit);
    if (!q->isReadOnly() && q->hasActiveFocus() && qGuiApp)
        QGuiApplication::inputMethod()->reset();
}

void TextEdit::q_textChanged()
{
    Q_D(TextEdit);
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

void TextEdit::markDirtyNodesForRange(int start, int end, int charDelta)
{
    Q_D(TextEdit);
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

    if (start == end) nodesUpdate = false;

    TextEditPrivate::Node dummyNode(start, nullptr);
    TextNodeIterator it = std::lower_bound(d->textNodeMap.begin(), d->textNodeMap.end(), &dummyNode, &comesBefore);
    if (nodesUpdate || charDelta)
    {
        // qLowerBound gives us the first node past the start of the affected portion, rewind to the first node
        // that starts at the last position before the edit position. (there might be several because of images)
        if (it != d->textNodeMap.begin()) {
            --it;
            TextEditPrivate::Node otherDummy((*it)->startPos(), nullptr);
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

void TextEdit::q_contentsChange(int pos, int charsRemoved, int charsAdded)
{
    Q_D(TextEdit);

    const int editRange = pos + qMax(charsAdded, charsRemoved);
    const int delta = charsAdded - charsRemoved;

    emit dirtyBlockPosition(d->document->findBlock(pos).blockNumber());

    markDirtyNodesForRange(pos, editRange, delta);

    d->highlightingInProgress = true;

    polish();
    if (isComponentComplete()) {
        d->updateType = TextEditPrivate::UpdatePaintNode;
        update();
    }
}

void TextEdit::moveCursorDelegate()
{
    Q_D(TextEdit);
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

void TextEdit::updateSelection()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    // No need for node updates when we go from an empty selection to another empty selection
    if (d->control->textCursor().hasSelection() || d->hadSelection) {
        markDirtyNodesForRange(qMin(d->lastSelectionStart, d->control->textCursor().selectionStart()), qMax(d->control->textCursor().selectionEnd(), d->lastSelectionEnd), 0);
        polish();
        if (isComponentComplete()) {
            d->updateType = TextEditPrivate::UpdatePaintNode;
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

QRectF TextEdit::boundingRect() const
{
    Q_D(const TextEdit);
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

QRectF TextEdit::clipRect() const
{
    Q_D(const TextEdit);
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

qreal TextEditPrivate::getImplicitWidth() const
{
    Q_Q(const TextEdit);
    if (!requireImplicitWidth) {
        // We don't calculate implicitWidth unless it is required.
        // We need to force a size update now to ensure implicitWidth is calculated
        const_cast<TextEditPrivate*>(this)->requireImplicitWidth = true;
        const_cast<TextEdit*>(q)->updateSize();
    }
    return implicitWidth;
}

//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void TextEdit::updateSize()
{
    Q_D(TextEdit);

    if (!isComponentComplete()) {
        d->dirty = true;
        return;
    }

    if (!d->document || !d->control) return;

    if (d->document->textWidth() != -1 )
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

void TextEdit::updateWholeDocument()
{
    Q_D(TextEdit);
    if (!d->textNodeMap.isEmpty()) {
        Q_FOREACH (TextEditPrivate::Node* node, d->textNodeMap)
            node->setDirty();
    }

    polish();
    if (isComponentComplete()) {
        d->updateType = TextEditPrivate::UpdatePaintNode;
        update();
    }
}

void TextEdit::highlightingDone(const QRectF &)
{
    Q_D(TextEdit);

    if (d->highlightingInProgress)
    {
        d->highlightingInProgress = false;
        emit textDocumentFinishedUpdating();
    }
}

void TextEdit::invalidateBlock(const QTextBlock &block)
{
    Q_D(TextEdit);

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
        d->updateType = TextEditPrivate::UpdatePaintNode;
        update();
    }
}

void TextEdit::updateCursor()
{
    Q_D(TextEdit);
    polish();
    if (isComponentComplete()) {
        d->updateType = TextEditPrivate::UpdatePaintNode;
        update();
    }
}

void TextEdit::q_updateAlignment()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    if (d->determineHorizontalAlignment()) {
        d->updateDefaultTextOption();
        d->xoff = qMax(qreal(0), TextUtil::alignedX(d->document->size().width(), width(), effectiveHAlign()));
        moveCursorDelegate();
    }
}

void TextEdit::updateTotalLines()
{
    Q_D(TextEdit);

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

void TextEditPrivate::updateDefaultTextOption()
{
    Q_Q(TextEdit);
    if (!document || !control) return;

    QTextOption opt = document->defaultTextOption();
    int oldAlignment = opt.alignment();
    Qt::LayoutDirection oldTextDirection = opt.textDirection();

    TextEdit::HAlignment horizontalAlignment = q->effectiveHAlign();
    if (contentDirection == Qt::RightToLeft) {
        if (horizontalAlignment == TextEdit::AlignLeft)
            horizontalAlignment = TextEdit::AlignRight;
        else if (horizontalAlignment == TextEdit::AlignRight)
            horizontalAlignment = TextEdit::AlignLeft;
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
    opt.setUseDesignMetrics(renderType != TextEdit::NativeRendering);

    if (oldWrapMode != opt.wrapMode() || oldAlignment != opt.alignment()
        || oldTextDirection != opt.textDirection()
        || oldUseDesignMetrics != opt.useDesignMetrics()) {
        document->setDefaultTextOption(opt);
    }
}

void TextEdit::focusInEvent(QFocusEvent *event)
{
    Q_D(TextEdit);
    d->handleFocusEvent(event);
    QQuickItem::focusInEvent(event);
}

void TextEdit::focusOutEvent(QFocusEvent *event)
{
    Q_D(TextEdit);
    d->handleFocusEvent(event);
    QQuickItem::focusOutEvent(event);
}

void TextEditPrivate::handleFocusEvent(QFocusEvent *event)
{
    Q_Q(TextEdit);
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

void TextEditPrivate::addCurrentTextNodeToRoot(TextNodeEngine *engine, QSGTransformNode *root, TextNode *node, TextNodeIterator &it, int startPos)
{
    engine->addToSceneGraph(node, QQuickText::Normal, QColor());
    it = textNodeMap.insert(it, new TextEditPrivate::Node(startPos, node));
    ++it;
    root->appendChildNode(node);
}

TextNode *TextEditPrivate::createTextNode()
{
    Q_Q(TextEdit);
    TextNode* node = new TextNode(q);
    node->setUseNativeRenderer(renderType == TextEdit::NativeRendering);
    return node;
}

void TextEdit::q_canPasteChanged()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;
    bool old = d->canPaste;
    d->canPaste = d->control->canPaste();
    bool changed = old!=d->canPaste || !d->canPasteValid;
    d->canPasteValid = true;
    if (changed)
        emit canPasteChanged();
}

/*!
    \qmlmethod string QtQuick::TextEdit::getText(int start, int end)

    Returns the section of text that is between the \a start and \a end positions.

    The returned text does not include any rich text formatting.
*/

QString TextEdit::getText(int start, int end) const
{
    Q_D(const TextEdit);
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

/*!
    \qmlmethod string QtQuick::TextEdit::getFormattedText(int start, int end)

    Returns the section of text that is between the \a start and \a end positions.

    The returned text will be formatted according the \l textFormat property.
*/

QString TextEdit::getFormattedText(int start, int end) const
{
    Q_D(const TextEdit);
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

/*!
    \qmlmethod QtQuick::TextEdit::insert(int position, string text)

    Inserts \a text into the TextEdit at position.
*/
void TextEdit::insert(int position, const QString &text)
{
    Q_D(TextEdit);
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

/*!
    \qmlmethod string QtQuick::TextEdit::remove(int start, int end)

    Removes the section of text that is between the \a start and \a end positions from the TextEdit.
*/

void TextEdit::remove(int start, int end)
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    start = qBound(0, start, d->document->characterCount() - 1);
    end = qBound(0, end, d->document->characterCount() - 1);
    QTextCursor cursor(d->document);
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    d->control->updateCursorRectangle(false);
}

bool TextEditPrivate::isLinkHoveredConnected()
{
    Q_Q(TextEdit);
    LV_IS_SIGNAL_CONNECTED(q, TextEdit, linkHovered, (const QString &));
}

/*!
    \qmlsignal QtQuick::TextEdit::linkHovered(string link)
    \since 5.2

    This signal is emitted when the user hovers a link embedded in the text.
    The link must be in rich text or HTML format and the
    \a link string provides access to the particular link.

    The corresponding handler is \c onLinkHovered.

    \sa hoveredLink, linkAt()
*/

/*!
    \qmlsignal QtQuick::TextEdit::editingFinished()
    \since 5.6

    This signal is emitted when the text edit loses focus.

    The corresponding handler is \c onEditingFinished.
*/

/*!
    \qmlproperty string QtQuick::TextEdit::hoveredLink
    \since 5.2

    This property contains the link string when the user hovers a link
    embedded in the text. The link must be in rich text or HTML format
    and the link string provides access to the particular link.

    \sa linkHovered, linkAt()
*/

QString TextEdit::hoveredLink() const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return QString();

    if (const_cast<TextEditPrivate *>(d)->isLinkHoveredConnected()) {
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

void TextEdit::hoverEnterEvent(QHoverEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    if (d->isLinkHoveredConnected())
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
}

void TextEdit::hoverMoveEvent(QHoverEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    if (d->isLinkHoveredConnected())
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
}

void TextEdit::hoverLeaveEvent(QHoverEvent *event)
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    if (d->isLinkHoveredConnected())
        d->control->processEvent(event, QPointF(-d->xoff, -d->yoff));
}

/*!
    \qmlmethod void QtQuick::TextEdit::append(string text)
    \since 5.2

    Appends a new paragraph with \a text to the end of the TextEdit.

    In order to append without inserting a new paragraph,
    call \c myTextEdit.insert(myTextEdit.length, text) instead.
*/
void TextEdit::append(const QString &text)
{
    Q_D(TextEdit);
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

/*!
    \qmlmethod QtQuick::TextEdit::linkAt(real x, real y)
    \since 5.3

    Returns the link string at point \a x, \a y in content coordinates,
    or an empty string if no link exists at that point.

    \sa hoveredLink
*/
QString TextEdit::linkAt(qreal x, qreal y) const
{
    Q_D(const TextEdit);
    if (!d->document || !d->control) return QString();

    return d->control->anchorAt(QPointF(x + topPadding(), y + leftPadding()));
}

/*!
    \since 5.6
    \qmlproperty real QtQuick::TextEdit::padding
    \qmlproperty real QtQuick::TextEdit::topPadding
    \qmlproperty real QtQuick::TextEdit::leftPadding
    \qmlproperty real QtQuick::TextEdit::bottomPadding
    \qmlproperty real QtQuick::TextEdit::rightPadding

    These properties hold the padding around the content. This space is reserved
    in addition to the contentWidth and contentHeight.
*/
qreal TextEdit::padding() const
{
    Q_D(const TextEdit);
    return d->padding();
}

void TextEdit::setPadding(qreal padding)
{
    Q_D(TextEdit);
    if (qFuzzyCompare(d->padding(), padding))
        return;

    d->extra.value().padding = padding;
    updateSize();
    if (isComponentComplete()) {
        d->updateType = TextEditPrivate::UpdatePaintNode;
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

void TextEdit::resetPadding()
{
    setPadding(0);
}

qreal TextEdit::topPadding() const
{
    Q_D(const TextEdit);
    if (d->extra.isAllocated() && d->extra->explicitTopPadding)
        return d->extra->topPadding;
    return d->padding();
}

void TextEdit::setTopPadding(qreal padding)
{
    Q_D(TextEdit);
    d->setTopPadding(padding);
}

void TextEdit::resetTopPadding()
{
    Q_D(TextEdit);
    d->setTopPadding(0, true);
}

qreal TextEdit::leftPadding() const
{
    Q_D(const TextEdit);
    if (d->extra.isAllocated() && d->extra->explicitLeftPadding)
        return d->extra->leftPadding;
    return d->padding();
}

void TextEdit::setLeftPadding(qreal padding)
{
    Q_D(TextEdit);
    d->setLeftPadding(padding);
}

void TextEdit::resetLeftPadding()
{
    Q_D(TextEdit);
    d->setLeftPadding(0, true);
}

qreal TextEdit::rightPadding() const
{
    Q_D(const TextEdit);
    if (d->extra.isAllocated() && d->extra->explicitRightPadding)
        return d->extra->rightPadding;
    return d->padding();
}

void TextEdit::setRightPadding(qreal padding)
{
    Q_D(TextEdit);
    d->setRightPadding(padding);
}

void TextEdit::resetRightPadding()
{
    Q_D(TextEdit);
    d->setRightPadding(0, true);
}

qreal TextEdit::bottomPadding() const
{
    Q_D(const TextEdit);
    if (d->extra.isAllocated() && d->extra->explicitBottomPadding)
        return d->extra->bottomPadding;
    return d->padding();
}

void TextEdit::setBottomPadding(qreal padding)
{
    Q_D(TextEdit);
    d->setBottomPadding(padding);
}

void TextEdit::resetBottomPadding()
{
    Q_D(TextEdit);
    d->setBottomPadding(0, true);
}

/*!
    \qmlmethod QtQuick::TextEdit::clear()
    \since 5.7

    Clears the contents of the text edit
    and resets partial text input from an input method.

    Use this method instead of setting the \l text property to an empty string.

    \sa QInputMethod::reset()
*/
void TextEdit::clear()
{
    Q_D(TextEdit);
    if (!d->document || !d->control) return;

    d->resetInputMethod();
    d->control->clear();
}


void TextEditPrivate::implicitWidthChanged()
{
    Q_Q(TextEdit);
    QQuickImplicitSizeItemPrivate::implicitWidthChanged();
#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
    emit q->implicitWidthChanged(); // change for 5.11
#else
    emit q->implicitWidthChanged2();
#endif
}

void TextEditPrivate::implicitHeightChanged()
{
    Q_Q(TextEdit);
    QQuickImplicitSizeItemPrivate::implicitHeightChanged();
#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
    emit q->implicitHeightChanged(); // change for 5.11
#else
    emit q->implicitHeightChanged2();
#endif
}

DocumentHandler* TextEdit::documentHandler()
{
    Q_D(TextEdit);
    return d->documentHandler;
}

void TextEdit::setDocumentHandler(DocumentHandler *dh)
{
    Q_D(TextEdit);
    d->documentHandler = dh;
    dh->setTextEdit(this);
}

void TextEdit::linePaletteAdded(int lineStart, int lineEnd, int height, QObject *palette)
{
    Q_D(TextEdit);
    int lineHeight = static_cast<int>(implicitHeight() / lineCount());

    qDebug() << lineHeight;
    if (lineStart < lineEnd && lineEnd <= lineCount())
    {
        QTextCursor c(d->document);
        c.beginEditBlock();
        c.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, lineStart-1);
        c.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, lineEnd-1-lineStart);
        c.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        qDebug() << c.selectedText();
        c.removeSelectedText();

        c.endEditBlock();

        c.beginEditBlock();
        int num = qCeil(height*1.0/lineHeight);
        qDebug() << "num: " << num;
        for (int i = 0; i<num-1;i++){
            c.insertBlock();
        }
        c.endEditBlock();
    }
}

void TextEdit::clearSelectionOnFocus(bool value){
    Q_D(TextEdit);
    d->control->clearSelectionOnFocus(value);
}

void TextEdit::setTextDocument(QTextDocument *td)
{
    Q_D(TextEdit);
    if (td)
    {
        d->setTextDocument(td);
    }
    else d->unsetTextDocument();
}

void TextEdit::collapseLines(int pos, int num, QString &replacement)
{
    Q_UNUSED(replacement)
    showHideLines(false, pos, num);
}

void TextEdit::expandLines(int pos, int num, QString &replacement)
{
    Q_UNUSED(replacement)
    showHideLines(true, pos, num);
}

void TextEdit::showHideLines(bool show, int pos, int num)
{
    Q_D(TextEdit);
    auto it = d->document->rootFrame()->begin();
    Q_ASSERT(d->document->blockCount() > pos);
    Q_ASSERT(d->document->blockCount() >= pos + num);
    for (int i = 0; i < pos+1; i++, ++it);
    auto itCopy = it;
    int start = it.currentBlock().position();

    int length = 0;
    for (int i = 0; i < num; i++)
    {
        it.currentBlock().setVisible(show);
        length += it.currentBlock().length();
        ++it;
    }

    d->document->markContentsDirty(start, length);

#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
    for (int i=0; i<num; i++)
    {
        invalidateBlock(itCopy.currentBlock());
        ++itCopy;
    }
#endif
}

void TextEdit::updateFragmentVisibility()
{
    Q_D(TextEdit);
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

void TextEdit::replaceTextInBlock(int blockNumber, std::string s)
{
    Q_D(TextEdit);
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
