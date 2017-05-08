#ifndef LIVE_PLUGIN_H
#define LIVE_PLUGIN_H

#include <QQmlExtensionPlugin>

class LivePlugin : public QQmlExtensionPlugin{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};
#endif // LIVE_PLUGIN_H
