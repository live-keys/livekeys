#ifndef LVQMLLANGUAGEOBJECT_H
#define LVQMLLANGUAGEOBJECT_H

#include <QObject>

namespace lv{

class QmlLanguageObject : public QObject{

    Q_OBJECT

public:
    explicit QmlLanguageObject(void* language, QObject *parent = nullptr);

    void* language(){ return m_language; }

private:
    void* m_language;
};

}// namespace

#endif // LVQMLLANGUAGEOBJECT_H
