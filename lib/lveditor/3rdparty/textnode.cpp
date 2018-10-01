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

#include "textnode_p.h"

#include "textnodeengine_p.h"

#include <private/qsgadaptationlayer_p.h>
#include <private/qsgdistancefieldglyphnode_p.h>
#include <private/qquickclipnode_p.h>
#include <private/qquickitem_p.h>
#include <QtQuick/private/qsgcontext_p.h>

#include <QtCore/qpoint.h>
#include <qtextdocument.h>
#include <qtextlayout.h>
#include <qabstracttextdocumentlayout.h>
#include <qxmlstream.h>
#include <private/qquickstyledtext_p.h>
#include <private/qfont_p.h>
#include <private/qfontengine_p.h>

#include <private/qtextdocumentlayout_p.h>
#include <qhash.h>

#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
#include <QSGRectangleNode>
#include <QSGImageNode>
#endif

namespace lv {

namespace {

    class ProtectedLayoutAccessor: public QAbstractTextDocumentLayout
    {
    public:
        inline QTextCharFormat formatAccessor(int pos)
        {
            return format(pos);
        }
    };

}

/*!
  Creates an empty QQuickTextNode
*/
TextNode::TextNode(QQuickItem *ownerElement)
    : m_cursorNode(0), m_ownerElement(ownerElement), m_useNativeRenderer(false)
{
#ifdef QSG_RUNTIME_DESCRIPTION
    qsgnode_set_description(this, QLatin1String("text"));
#endif
}

TextNode::~TextNode()
{
    qDeleteAll(m_textures);
}

QSGGlyphNode *TextNode::addGlyphs(const QPointF &position, const QGlyphRun &glyphs, const QColor &color,
                                     QQuickText::TextStyle style, const QColor &styleColor,
                                     QSGNode *parentNode)
{
    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
    QRawFont font = glyphs.rawFont();
    bool preferNativeGlyphNode = m_useNativeRenderer;
    if (!preferNativeGlyphNode) {
        QRawFontPrivate *fontPriv = QRawFontPrivate::get(font);
        if (fontPriv->fontEngine->hasUnreliableGlyphOutline())
            preferNativeGlyphNode = true;
        else
            preferNativeGlyphNode = !QFontDatabase().isSmoothlyScalable(font.familyName(), font.styleName());
    }

    QSGGlyphNode *node = sg->sceneGraphContext()->createGlyphNode(sg, preferNativeGlyphNode);

    node->setOwnerElement(m_ownerElement);
    node->setGlyphs(position + QPointF(0, glyphs.rawFont().ascent()), glyphs);
    node->setStyle(style);
    node->setStyleColor(styleColor);
    node->setColor(color);
    node->update();

    /* We flag the geometry as static, but we never call markVertexDataDirty
       or markIndexDataDirty on them. This is because all text nodes are
       discarded when a change occurs. If we start appending/removing from
       existing geometry, then we also need to start marking the geometry as
       dirty.
     */
    node->geometry()->setIndexDataPattern(QSGGeometry::StaticPattern);
    node->geometry()->setVertexDataPattern(QSGGeometry::StaticPattern);

    if (parentNode == 0)
        parentNode = this;
    parentNode->appendChildNode(node);

    return node;
}

void TextNode::setCursor(const QRectF &rect, const QColor &color)
{
    if (m_cursorNode != 0)
        delete m_cursorNode;

    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
    m_cursorNode =  sg->sceneGraphContext()->createRectangleNode(); // change for 5.11
    m_cursorNode->setRect(rect);
    m_cursorNode->setColor(color);
#else
    m_cursorNode =  sg->sceneGraphContext()->createRectangleNode(rect, color);
#endif
    appendChildNode(m_cursorNode);
}

void TextNode::clearCursor()
{
    if (m_cursorNode)
        removeChildNode(m_cursorNode);
    delete m_cursorNode;
    m_cursorNode = 0;
}

void TextNode::addRectangleNode(const QRectF &rect, const QColor &color)
{
    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
    auto node =  sg->sceneGraphContext()->createRectangleNode(); // change for 5.11
    node->setRect(rect);
    node->setColor(color);
#else
    auto node = sg->sceneGraphContext()->createRectangleNode(rect, color);
#endif
    appendChildNode(node);
}


void TextNode::addImage(const QRectF &rect, const QImage &image)
{
    QSGRenderContext *sg = QQuickItemPrivate::get(m_ownerElement)->sceneGraphRenderContext();
    QSGImageNode *node = sg->sceneGraphContext()->createImageNode();
    QSGTexture *texture = sg->createTexture(image);
    if (m_ownerElement->smooth()) {
        texture->setFiltering(QSGTexture::Linear);
        texture->setMipmapFiltering(QSGTexture::Linear);
    }
    m_textures.append(texture);

#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
    node->setSourceRect(rect); // change for 5.11
    node->setRect(rect);
#else
    node->setTargetRect(rect);
    node->setInnerTargetRect(rect);
#endif

    node->setTexture(texture);
    if (m_ownerElement->smooth()) {
        node->setFiltering(QSGTexture::Linear);
        node->setMipmapFiltering(QSGTexture::Linear);
    }
    appendChildNode(node);
#if (QT_VERSION <= QT_VERSION_CHECK(5,7,1))
    node->update();
#endif
}

void TextNode::addTextDocument(const QPointF &position, QTextDocument *textDocument,
                                  const QColor &textColor,
                                  QQuickText::TextStyle style, const QColor &styleColor,
                                  const QColor &anchorColor,
                                  const QColor &selectionColor, const QColor &selectedTextColor,
                                  int selectionStart, int selectionEnd)
{
    TextNodeEngine engine;
    engine.setTextColor(textColor);
    engine.setSelectedTextColor(selectedTextColor);
    engine.setSelectionColor(selectionColor);
    engine.setAnchorColor(anchorColor);
    engine.setPosition(position);

    QList<QTextFrame *> frames;
    frames.append(textDocument->rootFrame());
    while (!frames.isEmpty()) {
        QTextFrame *textFrame = frames.takeFirst();
        frames.append(textFrame->childFrames());

        engine.addFrameDecorations(textDocument, textFrame);

        if (textFrame->firstPosition() > textFrame->lastPosition()
         && textFrame->frameFormat().position() != QTextFrameFormat::InFlow) {
            const int pos = textFrame->firstPosition() - 1;
            ProtectedLayoutAccessor *a = static_cast<ProtectedLayoutAccessor *>(textDocument->documentLayout());
            QTextCharFormat format = a->formatAccessor(pos);
            QRectF rect = a->frameBoundingRect(textFrame);

            QTextBlock block = textFrame->firstCursorPosition().block();
            engine.setCurrentLine(block.layout()->lineForTextPosition(pos - block.position()));
            engine.addTextObject(rect.topLeft(), format, TextNodeEngine::Unselected, textDocument,
                                 pos, textFrame->frameFormat().position());
        } else {
            QTextFrame::iterator it = textFrame->begin();

            while (!it.atEnd()) {
                Q_ASSERT(!engine.currentLine().isValid());

                QTextBlock block = it.currentBlock();
                engine.addTextBlock(textDocument, block, position, textColor, anchorColor, selectionStart, selectionEnd);
                ++it;
            }
        }
    }

    engine.addToSceneGraph(this, style, styleColor);
}

void TextNode::addTextLayout(const QPointF &position, QTextLayout *textLayout, const QColor &color,
                                QQuickText::TextStyle style, const QColor &styleColor,
                                const QColor &anchorColor,
                                const QColor &selectionColor, const QColor &selectedTextColor,
                                int selectionStart, int selectionEnd,
                                int lineStart, int lineCount)
{
    TextNodeEngine engine;
    engine.setTextColor(color);
    engine.setSelectedTextColor(selectedTextColor);
    engine.setSelectionColor(selectionColor);
    engine.setAnchorColor(anchorColor);
    engine.setPosition(position);

#ifndef QT_NO_IM
    int preeditLength = textLayout->preeditAreaText().length();
    int preeditPosition = textLayout->preeditAreaPosition();
#endif

    QVarLengthArray<QTextLayout::FormatRange> colorChanges;
    engine.mergeFormats(textLayout, &colorChanges);

    lineCount = lineCount >= 0
            ? qMin(lineStart + lineCount, textLayout->lineCount())
            : textLayout->lineCount();

    for (int i=lineStart; i<lineCount; ++i) {
        QTextLine line = textLayout->lineAt(i);

        int start = line.textStart();
        int length = line.textLength();
        int end = start + length;

#ifndef QT_NO_IM
        if (preeditPosition >= 0
         && preeditPosition >= start
         && preeditPosition < end) {
            end += preeditLength;
        }
#endif

        engine.setCurrentLine(line);
        engine.addGlyphsForRanges(colorChanges, start, end, selectionStart, selectionEnd);
    }

    engine.addToSceneGraph(this, style, styleColor);
}

void TextNode::deleteContent()
{
    while (firstChild() != 0)
        delete firstChild();
    m_cursorNode = 0;
    qDeleteAll(m_textures);
    m_textures.clear();
}

}
