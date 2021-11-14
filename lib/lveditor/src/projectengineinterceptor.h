#ifndef LVPROJECTENGINEINTERCEPTOR_H
#define LVPROJECTENGINEINTERCEPTOR_H

#include <QObject>
#include "live/viewengineinterceptor.h"

namespace lv{

class Project;
class ViewEngine;
class ProjectEngineInterceptor : public ViewEngineInterceptor{

public:
    explicit ProjectEngineInterceptor(ViewEngine* engine, Project* project);

public:
    QUrl interceptUrl(const QUrl &path, QQmlAbstractUrlInterceptor::DataType dataType) override;
    ContentResult interceptContent(const QUrl& path) override;

private:
    Project*    m_project;
};

}// namespace

#endif // LVPROJECTENGINEINTERCEPTOR_H
