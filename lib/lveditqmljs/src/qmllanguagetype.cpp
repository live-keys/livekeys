#include "qmllanguagetype.h"

namespace lv{

QmlLanguageType::QmlLanguageType(lv::QmlLanguageType::Language lang, const QString &name, const QString &path)
    : m_language(lang)
    , m_path(path)
    , m_name(name)
{
}

QmlLanguageType QmlLanguageType::split(const QString &id){
    int languageSplit = id.indexOf('/');
    int nameSplit     = id.lastIndexOf('#');
    if ( languageSplit != -1 ){
        QString langSeg = id.mid(0, languageSplit);
        QString nameSegment     = id.mid(nameSplit == -1 ? languageSplit + 1 : nameSplit + 1);
        QString pathSegment     = nameSplit == -1 ? "" : id.mid(languageSplit + 1, nameSplit);

        QmlLanguageType::Language lang = langSeg == "cpp"
                ? QmlLanguageType::Cpp
                : langSeg == "qml" ? QmlLanguageType::Qml : QmlLanguageType::Unknown;

        return QmlLanguageType(lang, nameSegment, pathSegment);
    } else {
        return QmlLanguageType(QmlLanguageType::Unknown, id.mid(nameSplit + 1), nameSplit == -1 ? "" : id.mid(0, nameSplit));
    }
}

QString QmlLanguageType::join() const{
    QString p = path();
    return languageString() + "/" + (p.isEmpty() ? "" : p + "#") + name();
}

}// namespace
