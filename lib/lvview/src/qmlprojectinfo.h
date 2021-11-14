#ifndef LVQMLPROJECTINFO_H
#define LVQMLPROJECTINFO_H

#include <QObject>
#include <QUrl>

namespace lv{

class QmlProjectInfo : public QObject{

    Q_OBJECT

public:
    explicit QmlProjectInfo(const QString& path, const QUrl& programPath, QObject *parent = nullptr);
    ~QmlProjectInfo();

public slots:
    QUrl program() const;
    QString dir() const;
    QString path(const QString& relative) const;

private:
    QString m_path;
    QUrl    m_programPath;

};

}// namespace

#endif // LVQMLPROJECTINFO_H
