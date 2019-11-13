#ifndef LVQMLLANGUAGEOBJECT_H
#define LVQMLLANGUAGEOBJECT_H

#include <QObject>

namespace lv{

class QmlLanguageObject : public QObject{

    Q_OBJECT

public:
    explicit QmlLanguageObject(const void* language, QObject *parent = nullptr);

    const void* language(){ return m_language; }

private:
    const void* m_language;
};

}// namespace

#endif // LVQMLLANGUAGEOBJECT_H
