#ifndef QCODEDOCUMENT_HPP
#define QCODEDOCUMENT_HPP

#include <QQuickItem>

class QCodeDocument : public QQuickItem{

    Q_OBJECT
public:
    explicit QCodeDocument(QQuickItem *parent = 0);

public slots:
    QString openFile(const QString& content);
    void saveFile(const QString& content) const;
    void newFile(const QString& content);
    
private:
    QString m_openedFile;

};

#endif // QCODEDOCUMENT_HPP
