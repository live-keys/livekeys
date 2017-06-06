#include "qlivecvscript.h"
#include "qprojectdocument.h"
#include "qprojectfile.h"

namespace lcv{

QLiveCVScript::QLiveCVScript(const QStringList &argvTail, QObject *parent)
    : QObject(parent)
    , m_argvTail(argvTail)
{
    m_argv.append("");
    m_argv.append(m_argvTail);
}

void QLiveCVScript::scriptChanged(QProjectDocument *active){
    if ( active ){
        m_argv[0] = active->file()->name();
    } else {
        m_argv[0] = "";
    }
}

}// namespace
