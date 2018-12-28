#ifndef LINESURFACE_H
#define LINESURFACE_H

#include <QtGui/qtextoption.h>
#include "live/lveditorglobal.h"
#include "textedit_p.h"
#include "qquickpainteditem.h"

class QTextBlock;

namespace lv {

class LineSurfacePrivate;
class LineManager;
class TextNode;


class LV_EDITOR_EXPORT LineSurface : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)

public:

    friend class TextUtil;
    friend class LineManager;

public:

    class Node
    {
    public:
        explicit Node(int startPos, TextNode* node)
            : m_startPos(startPos), m_node(node) { }
        TextNode* textNode() const { return m_node; }
        void moveStartPos(int delta) { Q_ASSERT(m_startPos + delta > 0); m_startPos += delta; }
        int startPos() const { return m_startPos; }

    private:
        int m_startPos;
        TextNode* m_node;
    };

    LineSurface(QQuickItem *parent=nullptr);
    ~LineSurface() Q_DECL_OVERRIDE;
    QFont font() const;
    void setFont(const QFont &font);
    QColor color() const;
    void setColor(const QColor &c);
    Q_INVOKABLE void setComponents(lv::TextEdit* te);
    void setDocument(QTextDocument* doc);
    void unsetTextDocument();

public Q_SLOTS:
    void setDirtyBlockPosition(int pos);
    void paletteSlot(int blockNum);
    void triggerUpdate(int prev, int curr, int dirty);

Q_SIGNALS:
    void colorChanged(const QColor &color);
    void fontChanged(const QFont &font);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void updateSize();
    void triggerPreprocess();

private:
    void showHideLines(bool show, int pos, int num);
    void writeOutBlockStates();
    void writeOutBlockVisibility();

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

    enum UpdateType {
        UpdateNone,
        UpdateOnlyPreprocess,
        UpdatePaintNode
    };

    QColor m_color;
    QFont m_font;
    QTextDocument *m_document;
    QList<Node*> m_textNodeMap;
    UpdateType m_updateType;
    TextEdit* m_textEdit;
    int m_previousLineNumber;
    int m_lineNumber;
    int m_dirtyPos;
    int m_deltaLineNumber;
    bool m_updatePending;
private:
    Q_DISABLE_COPY(LineSurface)
};

}

QML_DECLARE_TYPE(lv::LineSurface)


#endif // LINESURFACE_H
