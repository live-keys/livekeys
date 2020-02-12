#ifndef LVQMLLANGUAGETYPE_H
#define LVQMLLANGUAGETYPE_H

#include "live/lveditqmljsglobal.h"

#include <QString>

namespace lv{

class LV_EDITQMLJS_EXPORT QmlLanguageType{

public:
    enum Language{
        Unknown = 0,
        Qml = 1,
        Cpp = 2
    };

public:
    QmlLanguageType(Language lang = QmlLanguageType::Unknown, const QString& name = "", const QString& path = "");

    static QmlLanguageType split(const QString& typeId);
    QString join() const;

    const QString& name() const;
    const QString& path() const;
    Language language() const;
    QString languageString() const;

    bool isEmpty() const;

private:
    Language m_language;
    QString  m_path;
    QString  m_name;
};

inline const QString &QmlLanguageType::name() const{
    return m_name;
}

inline const QString &QmlLanguageType::path() const{
    return m_path;
}

inline QmlLanguageType::Language QmlLanguageType::language() const{
    return m_language;
}

inline QString QmlLanguageType::languageString() const{
    return m_language == QmlLanguageType::Qml ? "qml" : QmlLanguageType::Cpp ? "cpp" : "u";
}

inline bool QmlLanguageType::isEmpty() const{
    return m_name.isEmpty();
}

}// namespace

#endif // LVQMLLANGUAGETYPE_H
