#include "QMatState.hpp"

QMatState::QMatState():
    mat(0),
    textureIndex(-1),
    textureSync(false),
    linearFilter(true)
{
}

int QMatState::compare(const QMatState *other) const{
    if ( textureIndex == -1 && other->textureIndex == -1 ){
        if ( mat == other->mat )
            return 0;
        else if ( mat < other->mat )
            return -1;
        else
            return 1;
    } else {
        if ( textureIndex == other->textureIndex )
            return 0;
        else if ( textureIndex < other->textureIndex )
            return -1;
        else
            return 1;
    }
    return 0;
}
