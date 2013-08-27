#ifndef QCODEDOCUMENT_HPP
#define QCODEDOCUMENT_HPP

#include <QQuickItem>

class QCodeDocument : public QQuickItem{

    Q_OBJECT
public:
    explicit QCodeDocument(QQuickItem *parent = 0);

public slots:
    QString openFile(const QUrl& file);
    void saveFile(const QUrl &file, const QString& content) const;
    
private:
    QString m_openedFile;

};

#endif // QCODEDOCUMENT_HPP
