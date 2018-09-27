#ifndef LINESURFACE_P_H
#define LINESURFACE_P_H

#include "linesurface.h"
#include "private/qquickimplicitsizeitem_p_p.h"
#include "private/qquickitem_p.h"
#include "documenthandler.h"
#include <QtQml/qqml.h>
#include <QtCore/qlist.h>
#include <climits>
#include "linemanager.h"

class QTextLayout;

namespace lv {

class TextControl;
class TextNode;
class TextNodeEngine;
class LineManager;

class LV_EDITOR_EXPORT LineSurfacePrivate : public QQuickItemPrivate
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

    LineSurfacePrivate()
        : color(QRgb(0xFF000000))
        , font(sourceFont), document(nullptr)
        , updateType(UpdatePaintNode)
        , textEdit(nullptr), prevLineNumber(0)
        , lineNumber(0), dirtyPos(0)
        , lineManager(new LineManager)
    {
    }

    ~LineSurfacePrivate()
    {
        qDeleteAll(textNodeMap);
    }


    static LineSurfacePrivate *get(LineSurface *item) {
        return static_cast<LineSurfacePrivate *>(QObjectPrivate::get(item)); }

    void init();

    QColor color;
    QSizeF contentSize;
    QFont sourceFont;
    QFont font;
    QTextDocument *document;
    QList<Node*> textNodeMap;


    enum UpdateType {
        UpdateNone,
        UpdateOnlyPreprocess,
        UpdatePaintNode
    };

    UpdateType updateType;
    TextEdit* textEdit;
    int prevLineNumber;
    int lineNumber;
    int dirtyPos;
    int deltaLineNumber;
    LineManager *lineManager;
};

}

#endif // LINESURFACE_P_H
