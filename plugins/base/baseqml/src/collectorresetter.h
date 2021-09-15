#ifndef LVCOLLECTORRESETTER_H
#define LVCOLLECTORRESETTER_H

namespace lv{

class QmlCollector;
class CollectorResetter{
public:
    virtual void assignColector(QmlCollector*) = 0;
};

}// namespace

#endif // COLLECTORRESETTER_H
