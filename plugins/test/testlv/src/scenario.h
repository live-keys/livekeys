#ifndef LVSCENARIO_H
#define LVSCENARIO_H

#include "live/elements/engine.h"
#include "live/elements/element.h"

namespace lv{ namespace el{

class Scenario : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Scenario)
            .base<Element>()
            .constructor()
            .scriptProperty<std::string>("describe", &Scenario::describe, &Scenario::setDescribe, "describeChanged")
            .scriptProperty<Callable>("run", &Scenario::run, &Scenario::setRun, "runChanged")
            .scriptEvent("describeChanged", &Scenario::describeChanged)
            .scriptEvent("runChanged", &Scenario::runChanged)
        META_OBJECT_CLOSE
    }

public:
    Scenario(Engine* engine);
    ~Scenario() override;

    const std::string& describe() const;
    void setDescribe(const std::string& describe);

    Callable run() const;
    void setRun(Callable c);

public:
    Event describeChanged(){
        static Event::Id eid = eventId(&Scenario::describeChanged);
        return notify(eid);
    }
    Event runChanged(){
        static Event::Id eid = eventId(&Scenario::runChanged);
        return notify(eid);
    }

private:
    Callable    m_run;
    std::string m_describe;
};

inline const std::string &Scenario::describe() const{
    return m_describe;
}

inline Callable Scenario::run() const{
    return m_run;
}

}} // namespace lv, el

#endif // LVSCENARIO_H
