#ifndef QLICENSESETTINGS_H
#define QLICENSESETTINGS_H

#include <QObject>
#include <QHash>

namespace lcv{

class QLicenseContainer;
class QLicenseSettings{

public:
    explicit QLicenseSettings(const QString& licenseFile);
    ~QLicenseSettings();

public:
    void reparse();
    void save();
    QLicenseContainer* container();

private:
    QLicenseContainer* m_container;
    QString            m_licenseFile;

};

inline QLicenseContainer *QLicenseSettings::container(){
    return m_container;
}

}// namespace

#endif // QLICENSESETTINGS_H
