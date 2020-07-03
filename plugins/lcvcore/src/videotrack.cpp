#include "videotrack.h"

namespace lv{

VideoTrack::VideoTrack(QObject *parent)
    : Track(parent)
    , m_surface(nullptr)
{
}

VideoTrack::~VideoTrack(){

}


}// namespace
