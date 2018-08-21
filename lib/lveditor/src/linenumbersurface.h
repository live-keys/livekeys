#ifndef LINENUMBERSURFACE_H
#define LINENUMBERSURFACE_H

#include "qquickitem.h"
#include "qsgnode.h"
#include "textedit_p.h"
#include "linemanager.h"

class TextNode;

// #define COLLAPSE_DEBUG

namespace lv {

class LineNumberSurfacePrivate;

class LV_EDITOR_EXPORT LineRootNode : public QSGTransformNode
{
public:
    LineRootNode();
    void resetFrameDecorations(TextNode* newNode);
    TextNode* frameDecorationsNode;
};

class LV_EDITOR_EXPORT LineNumberSurface : public QQuickItem {
    Q_OBJECT

public:
    LineNumberSurface(QQuickItem *parent=nullptr);
    ~LineNumberSurface() override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData) Q_DECL_OVERRIDE;

    Q_INVOKABLE void setComponents(TextEdit *te, LineManager *lm);

    void updateSize();
    void linesAdded();
    void linesRemoved();

    void collapseLines(int pos, int num);
    void expandLines(int pos, int num);

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    // void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void lineCountChanged();
    void setDirtyBlockPosition(int pos);
private:
    Q_DISABLE_COPY(LineNumberSurface)
    // Q_DECLARE_PRIVATE(LineNumberSurface)

    void init();

    TextEdit* textEdit;
    QTextDocument* textEditDocument;

    // Document containing line numbers. Allows us to use pre-existing text rendering structure beneath.
    QTextDocument* lineDocument;
    int prevLineNumber;
    int lineNumber;
    QFont font;

    //  Actual nodes to be drawn. One block - one node correspondence
    QList<TextNode*> lineNodes;

    // Index of the block that was changed?
    int dirtyPos;

    // number of lines added/removed
    int deltaLineNumber;

    LineManager *lineManager;

    // character width used for calculating the width of the line surface
    int visibleWidth;

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

    void showHideLines(bool show, int pos, int num);
    void changeLastCharInBlock(int blockNumber, char c);
    void replaceTextInBlock(int blockNumber, std::string s);
    void expandCollapseSkeleton(int pos, int num, QString &replacement, bool show);
    void updateLineDocument();
#ifdef COLLAPSE_DEBUG
    void testSetup1();
    void testSetup2();
#endif
};


}
#endif // LINENUMBERSURFACE_H

