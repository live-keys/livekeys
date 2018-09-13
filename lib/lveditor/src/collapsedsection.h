#ifndef COLLAPSEDSECTION_H
#define COLLAPSEDSECTION_H

#include "lveditorglobal.h"
#include <list>

namespace lv {

class LV_EDITOR_EXPORT CollapsedSection {
public:
    int position;
    int numberOfLines;
    std::list<CollapsedSection*> nestedSections;

    CollapsedSection(int pos = 1, int num = 1);
};

}

#endif // COLLAPSEDSECTION_H
