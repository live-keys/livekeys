/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "timeline_plugin.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "qrangeview.h"
#include "qabstractrangemodel.h"

#include "tracklistmodel.h"
#include "timelineheadermodel.h"
#include "timeline.h"
#include "timelineconfig.h"
#include "track.h"
#include "numbertrack.h"
#include "segment.h"
#include "segmentmodel.h"
#include "numberanimationsegment.h"

void TimelinePlugin::registerTypes(const char *uri){
    // @uri timeline
    qmlRegisterType<lv::Timeline>(      uri, 1, 0, "Timeline");
    qmlRegisterType<lv::Track>(         uri, 1, 0, "Track");
    qmlRegisterType<lv::NumberTrack>(   uri, 1, 0, "NumberTrack");
    qmlRegisterType<lv::Segment>(       uri, 1, 0, "Segment");
    qmlRegisterType<lv::SegmentModel>(  uri, 1, 0, "SegmentModel");
    qmlRegisterType<lv::TimelineConfig>(uri, 1, 0, "TimelineConfig");

    qmlRegisterUncreatableType<lv::TrackListModel>(
                uri, 1, 0, "TrackListModel", "TrackListModel is created as part of a track.");
    qmlRegisterUncreatableType<lv::TimelineHeaderModel>(
                uri, 1, 0, "TimelineHeaderModel", "TimelineHeaderModel is created as part of a Timeline.");

    qmlRegisterType<QRangeView>(uri, 1, 0, "RangeView");
    qmlRegisterUncreatableType<QAbstractRangeModel>(
                uri, 1, 0, "AbstractRangeModel", "AbstractRangeModel is of abstract type.");

    qmlRegisterType<lv::NumberAnimationSegment>(uri, 1, 0, "NumberAnimationSegment");
}

void TimelinePlugin::initializeEngine(QQmlEngine *, const char *){
}
