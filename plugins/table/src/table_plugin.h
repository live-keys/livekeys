#ifndef TABLEPLUGIN_H
#define TABLEPLUGIN_H

#include <QQmlExtensionPlugin>

class TablePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // TABLEPLUGIN_H
