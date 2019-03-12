# Plugin `lcvfeatures2d`

This module contains elements for feature detection and matching. It
resembles open cv's features 2D module.


```js
import lcvfeatures2d 1.0
```

{plugin:lcvfeatures}
{qmlSummary:lcvfeatures}


{qmlType:KeypointListView}
{qmlInherits:external.QtQuick#Column}
{qmlBrief:View for a list of keypoints.}

{qmlProperty:color headerColor}

Color for the header of the list.

{qmlProperty:color headerTextColor}

Color for the text.

{qmlProperty:color backgroundColor}

Main background color.

{qmlProperty:color itemBackgroundColor}

Background color for each item.

{qmlProperty:color selectedItemBackgroundColor}

Background color of selected item.

{qmlProperty:MathDraw keypointHighlighter}

Draws keypoints on a highlighter.

{qmlProperty:FeatureDetector detector}

Detector used.

{qmlProperty:DescriptorExtractor extractor}

Extractor used.

{qmlProperty:variant descriptorValues}

Given descriptor values.

{qmlSignal:keypointMouseEnter(int index, point pt)}

Triggered when the mouse entered a keypoint region.

{qmlSignal:keypointMouseExit(int index, point pt)}

Triggered when the mouse exited a keypoint section.

{qmlSignal:keypointMouseSelected(int index, point pt)}

Triggered when a keypoint region was selected.

{qmlProperty:alias currentIndex}

Current index of the selected keypoint.

{qmlType:FeatureDetectorSelection}
{qmlInherits:QtQuick.Rectangle}

{qmlProperty:string stateId}

Assigned when staticLoad is active.

{qmlSignal:configurationUpdate}

Signal for configuration update.

{qmlProperty:Mat detectorInput}

Input used for the detector.

{qmlProperty: alias selectedName}

Name of the detector.

{qmlProperty: alias selectedDetector}

Selected detector.

{qmlProperty: alias selectedPanelModel}

Selected panel model.

{qmlProperty: alias selectedIndex}

Selected index.

{qmlProperty: color backgroundColor}

Background color used throughout the selections.

{qmlProperty: color textColor}
{qmlBrief:Default text color}

{qmlProperty: int dropBoxHeight}

Height of the dropBox when selecting diferent feature detector.

{qmlSignal:comboClicked()}

Emited when selection happens.

{qmlType:DescriptorExtractorSelection}
{qmlInherits:QtQuick.Rectangle}
{qmlBrief:Contains a selection for a descriptor extractor.}

{qmlProperty: string stateId}

Assigned when staticLoad is active.

{qmlSignal: configurationUpdate}

Signal for configuration update.

{qmlProperty: FeatureDetector detector}

Detector used.

{qmlProperty: alias selectedName}

Name of the extractor.

{qmlProperty: alias selectedExtractor}

Selected extractor.

{qmlProperty: alias selectedPanelModel}

Selected panel model.

{qmlProperty: alias selectedIndex}

Selected index.

{qmlProperty: color backgroundColor}

Background color used throughout the selections.

{qmlProperty: color textColor}

Default text color.

{qmlProperty: int dropBoxHeight}

Height of the dropBox when selecting diferent feature detector.

{qmlSignal: comboClicked()}

Emited when selection happens.

{qmlType:FeatureObjectList}
{qmlInherits:QtQuick.Rectangle}
{qmlBrief:List of detected features for an object.}

{qmlProperty: FeatureDetector featureDetector}

FeatureDetector used.

{qmlProperty: DescriptorExtractor descriptorExtractor}

DescriptorExtractor used.

{qmlSignal: objectAdded(Mat descriptors, var points, var color)}

Triggered when an object was added.

{qmlSignal: objectListLoaded(MatList list, var keypoints, var corners, var colors)}

Triggered when the object list was loaded.

{qmlSignal: objectListCreated()}

Triggered when the object list was created.

{qmlProperty: alias objectList}

Actual list.

{qmlProperty: alias selectedIndex}

Index of the selected item.

{qmlType:FeatureObjectMatch}
{qmlInherits:QtQuick.Rectangle}
{qmlBrief:Object matcher for a list.

{qmlProperty:FeatureDetector trainFeatureDetector}

FeatureDetector used for training.

{qmlProperty:DescriptorExtractor trainDescriptorExtractor}

DescriptorExtractor used for training.

{qmlProperty:FeatureDetector queryFeatureDetector}

FeatureDetector used for querying.

{qmlProperty:var imageSource}

Image source.

{qmlProperty:var querySource}

Query source.

{qmlProperty:real minMatchDistanceCoeff}

Property for the actual matcher.

{qmlProperty:real matchNndrRation}

Property for the actual matcher.

{qmlSignal:objectListCreated()}

Emited when the list is created.

{qmlProperty:KeyPointVector drawQueryKeypoints}

Keypoints recieved from the trainFeatureDetector.

{qmlProperty:int drawMatchIndex}

Index where to draw the matches at.

{qmlProperty:Index of the training image matches}.

{qmlProperty: string stateId}

Static state id. See StaticItem for more details.

{qmlType:SelectionArea}
{qmlInherits:QtQuick.MouseArea}
{qmlBrief:Area that can be selected through a rectangle}

{qmlProperty:color selectionColor}

Color of the selection.

{qmlProperty:real selectionOpacity}

Opacity of the selection.

{qmlSignal:selected(int x, int y, int width, int height)}

Triggered when a selection happens.

{qmlProperty:Rectangle highlightItem}

Contains the rectangle of the highlighted item.

{qmlType:KeyPoint}
{qmlInherits:external.QtQml#QtObject}
{qmlBrief:Wrapper for a found keypoint.}

{qmlProperty:Point2f pt}

Coordinates of the keypoint.

{qmlProperty:float size}

Diameter of the meaningful keypoint neighborhood.

{qmlProperty:float angle}

Angle the keypoint was detected at.

{qmlProperty:float response}

The response by which the most strong keypoints have been selected.
Can be used for the further sorting or subsampling.

{qmlProperty:int octave}

Keypoint octave.

{qmlProperty:int classId}

Id of the detected class for the keypoint.

{qmlType:FeatureDetector}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Detector of features within an image. This is an abstract type.}

{qmlProperty:Mat input}

Image to detect the keypoints in.

{qmlProperty:output}

Drawn features from the input to another image.

{qmlProperty:Mat mask}

Limited amount of detections for a specific area.

{qmlProperty:params}

Parameters for detection.

{qmlProperty:keypoints}

Given keypoints.

{qmlType:FastFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}
{qmlBrief:FAST FeatureDetector}

Initialization parameters:

* **int threshold**

* **bool nonmaxSuppression**

{qmlType:BriskFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}
{qmlBrief:Brisk FeatureDetector}

Feature detector for the Brisk algorithm.

Initialization parameters:

* **int thresh**
* **int octave**
* **float patternScale**

{qmlType:OrbFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}
{qmlBrief:Orb FeatureDetector}

Feature detector for the Orb algorithm.

Initialization parameters:

* **int nfeatures**
* **float scaleFactor**
* **int nlevels**
* **int edgeThreshold**
* **int firstLevel**
* **int WTA_K**
* **int scoreType**

{qmlType:MSerFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}
{qmlBrief:MSer FeatureDetector}

Feature detector for the MSer algorithm:

* **int delta**
* **int minArea**
* **int maxArea**
* **float maxVariation**
* **float minDiversity**
* **int maxEvolution**
* **real areaThreshold**
* **real minMargin**
* **int edgeBlurSize**

{qmlType:simpleBlobDetector}
{qmlINherits:lcvfeatures2d#FeatureDetector}
{qmlBrief:Implements a simple algorithm for extracting blobs from an image.}

Algorithm:

1. Convert the source image to binary images by applying thresholding with several thresholds from
   `minThreshold` (inclusive) to `maxThreshold` (exclusive) with distance `thresholdStep` between
   neighboring thresholds.

2. Extract connected components from every binary image by findContours and calculate their
   centers.

3. Group centers from several binary images by their coordinates. Close centers form one group that
   corresponds to one blob, which is controlled by the minDistBetweenBlobs parameter.

4. From the groups, estimate final centers of blobs and their radiuses and return as locations and
   sizes of keypoints.

This class performs several filtrations of returned blobs. You should set `filterBy` to `true/false`
to turn the corresponding filtration **on** or **off**.

Params:

* **filterByInertia**
* **filterByConvexity**
* **filterByColor**
* **filterByCircularity**
* **filterByArea**

{qmlType:DescriptorExtractor}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Abstract base class for computing descriptors from an images keypoints.}

{qmlProperty:keypoints}

Detected keypoints of an image.

{qmlProperty:descriptors}

Computed result.

{qmlProperty:params}

Parameters given according to the type of descriptor extractor.

{qmlProperty:isBinary}

Wether this DescriptorExtractor is of binary type.

{qmlType:BriskDescriptorExtractor}
{qmlInherits:lcvfeatures2d#DescriptorExtractor}
{qmlBrief:Decsriptor extractor for the Brisk algorithm.}


{qmlType:OrbDescriptorExtractor}
{qmlInherits:lcvfeatures2d#DescriptorExtractor}
{qmlBrief:Decsriptor extractor for the Orb algorithm.}

Params:

* **int patchSize**


{qmlType:DMatchVector}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Vector of matches.}


{qmlType:DescriptorMatcher}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Abstract base class for matching keypoint descriptors.}

It has two groups of match methods: for matching descriptors of an image with another image or with an image set.

Params:

* **queryDescriptors** Query set of descriptors.
* **knn** Count of best matches found per each query descriptor or less if a query descriptor has less than k possible matches in total.

{qmlType:BruteForceMatcher}
{qmlInherits:lcvfeatures2d#DescriptorMatcher}
{qmlBrief:Finds the closest descriptor match.}

Params:

* **int normType**
* **bool crossCheck**

{qmlType:FlannBasedMatcher}
{qmlInherits:lcvfeatures2d#DescriptorMatcher}
{qmlBrief:Finds the closest descriptor k matches.}

Initialize params:

 * `KDTree`

    * `int trees`

 * `HierarchicalClustering`

 * `Lsh`

    * `int tableNumber`
    * `int keySize`
    * `int multiProbeLevel`


{qmlType:DrawMatches}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Draws matches between two sets of keypoints that were detected and matched.}

You can specify the match index to focus on specific match when drawing.


{qmlProperty:KeyPointVector keypoints1}

Keypoints from the first source.

{qmlProperty:KeyPointVector keypoints2}

Keypoints from the second source

{qmlProperty:DMatchVector matches1to2}

Matches from the first source to second one.

{qmlProperty:int matchIndex}

Draw matches at this specified index.

{qmlType:DescriptorMatchFilter}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Filter for descriptor matches.}

{qmlProperty:DMatchVector matches1to2}

Matches from the first source to second one.

{qmlProperty:DMatchVector matches1to2Out}

Output of matches1to2.

{qmlProperty:real minDistanceCoeff}

Minimum distance that the match has to the actual outcome.

{qmlProperty:real maxDistance}

Maximum distance allowed betwen set of two matches.

{qmlProperty:real nndrRatio}

The match betwen the first match and it's next consecutive match.


{qmlType:KeypointsToSceneMap}
{qmlInherits:external.QtQml#QtObject}
{qmlBrief:Contains a keypoints to scene map.}

{qmlProperty:int size()}

Size of the vector of the KeypointsToScene object.


{qmlType:MatchesToLocalKeypoint}
{qmlInherits:external.QtQml#QtObject}
{qmlBrief:Maps matches to local keypoint.}

{qmlProperty:DMatchVector matches1to2}

Matches from the first source to second one.

{qmlProperty:list<KeyPointVector> trainKeypointVectors}

List of trained keypoint vectors.

{qmlProperty:KeyPointVector queryKeypointVector}

Query keypoint vector.

{qmlProperty:KeyPointToSceneMap output}

The mapped object.

{qmlSignal:matches1to2Changed()}
{qmlBrief:Triggered when matches1to2 changed.}

{qmlSignal:trainKeypointVectorsChanged()}
{qmlBrief:Trigered when trainKeypointVectors is changed}

{qmlSignal:queryKeypointVectorChanged()}
{qmlBrief:Trigered when queryKeypointVector is changed}

{qmlSignal:outputChanged()}
{qmlBrief:Triggered when output is changed}

{qmlType:KeypointHomography}
{qmlInherits:lcvcore#MatDisplay}
{qmlBrief:Performs a homography based on the keypoints to scene.}

{qmlProperty:KeyPointToSceneMap keypointsToScene}

Keypoints to scene object.

{qmlProperty:Mat queryImage}

Query image used.

{qmlProperty:Array objectCorners}

An array of 4 points describing the obejct corners

{qmlProperty:Array objectColors}

Array of object colors.

{qmlSignal:keypointsToSceneChanged()}

Triggered when KeypointsToScene is changed.

{qmlSignal:queryImageChanged()}

Triggered when QueryImage is changed.

{qmlSignal:objectCornersChanged()}

Trigered when objectCorners is changed.

{qmlSignal:objectColorsChanged()}

Trigered when objectColors is changed.
