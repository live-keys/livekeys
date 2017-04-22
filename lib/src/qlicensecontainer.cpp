#include "qlicensecontainer.h"

namespace lcv{

QLicenseContainer::QLicenseContainer(QObject *parent)
    : QObject(parent)
{

}

void QLicenseContainer::addLicense(const QString &id, const QString &type, const QString &file){
    QLiveCVLicense license;
    license.valid = false;
    license.id = id;
    license.file = file;
    license.type = type;
    m_licenses[id] = license;
}

bool QLicenseContainer::hasLicense(const QString &id){
    return m_licenses.contains(id);
}

}// namespace
