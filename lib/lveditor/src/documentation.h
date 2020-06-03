#ifndef LVDOCUMENTATION_H
#define LVDOCUMENTATION_H

#include <QQuickItem>

namespace lv{

class ViewEngine;
class PackageGraph;
class DocumentationLoader;
class Documentation : public QObject{

    Q_OBJECT

public:
    Documentation(const PackageGraph* pg, QObject* parent = nullptr);
    ~Documentation();

    QJSValue load(const QString& language, const QString& path, const QString& detail);

public slots:
    QJSValue load(const QString& path);

private:
    DocumentationLoader* createLoader(ViewEngine *engine, const QString& path);

    const PackageGraph*  m_packageGraph;
    DocumentationLoader* m_defaultLoader;
};

}// namespace

#endif // DOCUMENTATION_H
