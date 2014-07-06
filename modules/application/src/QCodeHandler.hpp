#ifndef QCODEHANDLER_HPP
#define QCODEHANDLER_HPP

#include <QQuickItem>

class QQuickTextDocument;

class QCodeJSHighlighter;
class QCodeHandler : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target READ target WRITE setTarget NOTIFY targetChanged)

public:
    explicit QCodeHandler(QQuickItem *parent = 0);
    ~QCodeHandler();

    QQuickTextDocument *target();
    void setTarget(QQuickTextDocument *target);

signals:
    void targetChanged();

private:
    QQuickTextDocument*  m_target;
    QCodeJSHighlighter*  m_highlighter;

};

inline QQuickTextDocument *QCodeHandler::target(){
    return m_target;
}

#endif // QCODEHANDLER_HPP
