#ifndef LVHOOKCONTAINER_H
#define LVHOOKCONTAINER_H

#include <QObject>
#include <QMap>
#include <QList>

#include "live/lveditorglobal.h"

namespace lv{

class Runnable;
class LV_EDITOR_EXPORT HookContainer : public QObject{

    Q_OBJECT

private:
    class Entry{
    public:
        QString builderId;
        QList<QObject*> builders;
    };

public:
    HookContainer(const QString& projectUrl, Runnable* r, QObject *parent = nullptr);
    ~HookContainer() override;

    void insertKey(const QString& sourceFile, const QString& id, QObject* obj);
    QMap<QString, QList<QObject*> > entriesForFile(const QString& sourceFile);
    QList<QObject*> entriesFor(const QString& file, const QString& id);

signals:
    void entryAdded(const QString& file, const QString& id, QObject* object);

private:
    Runnable* m_runnable;
    QString   m_projectPath;
    QMap<QString, QMap<QString, QList<QObject*> > > m_entries;
};

}// namespace

#endif // LVHOOKCONTAINER_H
