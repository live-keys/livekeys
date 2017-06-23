#ifndef QMATLOADER_H
#define QMATLOADER_H

#include <QQuickItem>

class QMat;
class QJSValue;

class QMatLoader : public QQuickItem{

    Q_OBJECT

public:
    explicit QMatLoader(QQuickItem* parent = 0);
    ~QMatLoader();

public slots:
    QMat* staticLoad(const QString& id, const QJSValue& params);

};

#endif // QMATLOADER_H
