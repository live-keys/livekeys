#ifndef LINENUMBERSURFACE_H
#define LINENUMBERSURFACE_H

#include "qquickitem.h"
#include "qsgnode.h"
#include "textedit_p.h"
#include "linemanager.h"

class TextNode;

namespace lv {

class LineNumberSurfacePrivate;

class LV_EDITOR_EXPORT LineRootNode : public QSGTransformNode
{
public:
    LineRootNode();
    void resetFrameDecorations(TextNode* newNode);
    TextNode* frameDecorationsNode;
};
// done
class LV_EDITOR_EXPORT LineNumberSurface : public QQuickItem {

    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    LineNumberSurface(QQuickItem *parent = nullptr);
    ~LineNumberSurface() override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) Q_DECL_OVERRIDE;

    Q_INVOKABLE void setComponents(TextEdit *te); //done

    void updateSize(); //done
    void linesAdded(); //done
    void linesRemoved(); //done

    void collapseLines(int pos, int num); //done
    void expandLines(int pos, int num); //done

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE; //done
    // void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    QColor color() const{ return m_color; } //done

public Q_SLOTS:
    void setDirtyBlockPosition(int pos); //done
    void textDocumentFinished(); //done
    void setColor(QColor color); //done?

signals:
    void colorChanged(QColor color); //done

private:
    Q_DISABLE_COPY(LineNumberSurface) //done
    // Q_DECLARE_PRIVATE(LineNumberSurface)

    void init(); //done-ish

    TextEdit* textEdit; //done

    // Document containing line numbers. Allows us to use pre-existing text rendering structure beneath.
    QTextDocument* lineDocument; //done
    int prevLineNumber; //done
    int lineNumber;//done
    QFont font;//done

    //  Actual nodes to be drawn. One block - one node correspondence
    QList<TextNode*> lineNodes; //done

    // Index of the block that was changed?
    int dirtyPos; //done

    // number of lines added/removed
    int deltaLineNumber; //done

    LineManager *lineManager; //done

    static inline int numberOfDigits(int i) {
        int res = 0;
        if (i < 10) return 2;
        while (i > 0)
        {
            res++;
            i/=10;
        }
        return res;
    } // done

    void showHideLines(bool show, int pos, int num); // done
    void changeLastCharInBlock(int blockNumber, char c); //done
    void replaceTextInBlock(int blockNumber, std::string s); //done
    void expandCollapseSkeleton(int pos, int num, QString &replacement, bool show); //done
    void updateLineDocument(); //done

    QColor m_color; //done
};

inline void LineNumberSurface::setColor(QColor color){
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
} //done


}
#endif // LINENUMBERSURFACE_H

