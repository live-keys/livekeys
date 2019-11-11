#ifndef LVPROJECTLVSCANNER_H
#define LVPROJECTLVSCANNER_H

#include <QObject>

namespace lv{

class ProjectLvScanner : public QObject{

    Q_OBJECT

public:
    explicit ProjectLvScanner(QObject *parent = nullptr);
    ~ProjectLvScanner() override;

signals:

public slots:

};

}// namespace

#endif // PROJECTLVSCANNER_H
