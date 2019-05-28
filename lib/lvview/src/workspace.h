#ifndef LVWORKSPACE_H
#define LVWORKSPACE_H

#include <QObject>
#include "lvviewglobal.h"
#include "live/mlnode.h"

namespace lv{

/// \private
class LV_VIEW_EXPORT Workspace : public QObject{

    Q_OBJECT

public:
    class LV_VIEW_EXPORT Message{

    public:
        enum Type{
            ProjectOpen,
            ProjectClose,
            ProjectActiveChange,
            DocumentOpen,
            DocumentClose,
            DocumentMonitor,
            DocumentContentsChange,
            WindowRectChange,
            WindowStateChange
        };

        typedef QSharedPointer<Message> Ptr;
        typedef QSharedPointer<const Message> ConstPtr;

    public:
        Message(Type type, const MLNode& data);

        Type type() const{ return m_type; }
        const MLNode& data() const{ return m_data; }

    private:
        Type   m_type;
        MLNode m_data;
    };

public:
    explicit Workspace(QObject *parent = nullptr);
    ~Workspace();

signals:

public slots:

};

}// namespace

#endif // LVWORKSPACE_H
