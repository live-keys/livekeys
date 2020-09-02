# Video Timeline

{using:resources}
{livekeysInit:open/resources/ShapeVideoTimeline.qml}

Timelines are components used in Livekeys to manage chronological events. They are highly
customizable, and can be combined with other components or pipelines to automate different
processes.

Customiation is done by adding different types of tracks. A track can manage animations, play video, display images, or
manage segments and effects defined by the user.

In this sample, we've set up a a timeline with 2 tracks. One for images and video, and one for
animations.

The "Video Track 1" is displayed in the VideoSurfaceView. You can click play to see it in action.
You can move the segments at different positions in the timeline by dragging them,
or you can strectch them by dragging their margins. You can add
new segments by clicking the options button next to the track. Removing segments
is done by selecting the segment, and pressing the delete button (or backspace on Mac).

The second track is a keyframe track, with 2 keyframes that control the blue rectangle at the bottom of the run view.
The track simply animates a number between a set of keyframes, and any property that's connected to it will
receive the animated value.

There's an intermediate KeyFrameValue object, that connects to the timeline and the keyframe track, which
is connected further to the blue rectangle. To create more number animations, you can add more keyframe tracks from
the timeline options menu. For each keyframe track, you need to add a KeyFrameValue object that connects to the timline
and references the newly created track. Then, to the KeyFrameValue, you can connect any number of properties that follow
the animation. To control the animation, you can move the keyframes around by dragging them, and also add new ones from the track options
menu.

The timeline is loaded from a file in the project directory, 'videotimeline.json'. You can save the timeline via the
timeline options menu.


