# Video Timeline

{using:resources}
{livekeysInit:open/resources/ShapeVideoTimeline.qml}

[Timelines]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#timeline-bound;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#timeline-bound}) are components used in Livekeys to manage chronological events. They are highly
customizable, and can be combined with other components or pipelines to automate different
processes.

Customization is done by adding different types of [tracks]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#tracks;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#tracks}). A track can manage animations, play video, display images, or
manage segments and effects defined by the user.

In this sample, we've set up a a [timeline]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#timeline-item;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#timeline-item}) with [2 tracks]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#tracks;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#tracks}). One for images and video, and one for
animations.

The [`Video Track 1`]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#video-track-1;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#video-track-1}) is displayed in the [`VideoSurfaceView`]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#videosurfaceview;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#videosurfaceview}). You can click [play]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#play-button;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#play-button}) to see it in action.
You can move the segments at different positions in the timeline by dragging them,
or you can strectch them by dragging their margins. You can add
new segments by clicking the [options button]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#options-button;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#options-button}) next to the track. Removing segments
is done by selecting the segment, and pressing the delete button (or backspace on Mac).

The second track is a [keyframe track]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#keyframe-track;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#keyframe-track}), with 2 keyframes that control the [blue rectangle]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#blue-rectangle;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#blue-rectangle}) at the bottom of the run view.
The track simply animates a number between a set of [keyframes]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#keyframes;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#keyframes}), and any property that's connected to it will
receive the animated value.

There's an intermediate [`KeyFrameValue`]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#keyframevalue;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#keyframevalue}) object, that connects to the [timeline]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#timeline-item;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#timeline-item}) and the [keyframe track]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#keyframe-track;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#keyframe-track}), which
is connected further to the [blue rectangle]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#blue-rectangle;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#blue-rectangle}). To create more number animations, you can add more keyframe tracks from
the [timeline options]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#timeline-options-button;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#timeline-options-button}) menu. For each keyframe track, you need to add a KeyFrameValue object that connects to the timeline
and references the newly created track. Then, to the KeyFrameValue, you can connect any number of properties that follow
the animation. To control the animation, you can move the keyframes around by dragging them, and also add new ones from the track options
menu.

The timeline is loaded from a [file]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#file-property;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#file-property}) in the project directory, 'videotimeline.json'. You can save the timeline via the
[timeline options]({livekeys-hover:livekeys://open/resources/VideoTimelineHighlight.qml#timeline-options-button;livekeys://open/resources/VideoTimelineRemoveHighlight.qml#timeline-options-button}) menu.


