#ifndef LIVE_PLUGIN_H
#define LIVE_PLUGIN_H

#include <QQmlExtensionPlugin>

class LivePlugin : public QQmlExtensionPlugin{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri) Q_DECL_OVERRIDE;
};
#endif // LIVE_PLUGIN_H
