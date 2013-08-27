#include "QCodeDocument.hpp"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QDebug>

QCodeDocument::QCodeDocument(QQuickItem *parent) :
    QQuickItem(parent)
{
}

QString QCodeDocument::openFile(const QUrl &file){
    QString fileName = file.toLocalFile();
    if ( fileName != "" ){
        QFile file(fileName);
        if ( !file.open(QIODevice::ReadOnly ) ){
            return QString("Could not open file : ") + fileName;
        }
        QTextStream in(&file);
        m_openedFile = fileName;
        return in.readAll();
    }
    return "";
}

void QCodeDocument::saveFile(const QUrl& file, const QString &content) const{
    QString fileName = file.toLocalFile();
    if ( fileName != "" ){
        QFile file(fileName);
        if ( !file.open(QIODevice::WriteOnly ) ){
            return;
        } else {
            QTextStream stream(&file);
            stream << content;
            stream.flush();
            file.close();
        }
    }
}
