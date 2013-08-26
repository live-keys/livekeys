#include "QCodeDocument.hpp"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

QCodeDocument::QCodeDocument(QQuickItem *parent) :
    QQuickItem(parent)
{
}

QString QCodeDocument::openFile(const QString &content){
    if ( content != "" && content != "Rectangle{\n}" ){
        if ( QMessageBox::question(
                 0,
                 "Save File",
                 "The current code has been changed, would you like to save changes?"
        ) == QMessageBox::Yes )
            saveFile(content);
    }
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"), QString(), "QML Files (*.qml);;All Files (*.*)", 0 );
    if ( fileName != "" ){
        QFile file(fileName);
        if ( !file.open(QIODevice::ReadOnly ) ){
            QMessageBox::critical(0, tr("Error"), tr("Could not open file"));
            return "";
        }
        QTextStream in(&file);
        m_openedFile = fileName;
        return in.readAll();
    }
    return "";
}

void QCodeDocument::saveFile(const QString &content) const{
    QString fileName = QFileDialog::getSaveFileName(0, tr("Open File"), QString(), "QML Files (*.qml);;All Files (*.*)", 0 );
    if ( fileName != "" ){
        QFile file(fileName);
        if ( !file.open(QIODevice::WriteOnly ) ){
            QMessageBox::critical(0, tr("Error"), tr("Could not save file"));
        } else {
            QTextStream stream(&file);
            stream << content;
            stream.flush();
            file.close();
        }
    }
}

void QCodeDocument::newFile(const QString &content){
    if ( content != "" && content != "Rectangle{\n}" ){
        if ( QMessageBox::question(
                 0,
                 "Save",
                 "The current code has been changed, would you like to save changes?"
        ) == QMessageBox::Yes )
            saveFile(content);
    }
}
