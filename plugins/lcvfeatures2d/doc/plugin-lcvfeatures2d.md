# Plugin `lcvfeatures2d`

  This module contains elements for feature detection and matching. It resembles open cv's features 2D module.


```js
import lcvfeatures2d 1.0
```

{plugin:lcvfeatures2d}
{qmlSummary:lcvfeatures2d}


{qmlType:KeypointListView}
{qmlInherits:QtQuick.Column}
{qmlBrief:Add brief}

{qmlProperty:color headerColor}
{qmlProperty:color headerTextColor}

{qmlProperty:color backgroundColor}
{qmlBrief:Main background color}

{qmlProperty:color itemBackgroundColor}
{qmlBrief:Background color for each item}

{qmlProperty:color selectedItemBackgroundColor}
{qmlBrief:Background color of selected item}

{qmlProperty:MathDraw keypointHighlighter}
{qmlBrief:Draws keypoints on a highlighter.}

{qmlProperty:FeatureDetector detector}
{qmlBrief:add brief} 

{qmlProperty:DescriptorExtractor extractor}
{qmlBrief:add brief}

{qmlProperty:variant descriptorValues}
{qmlBrief:add brief}

{qmlSignal:keypointMouseEnter(int index, point pt)}
{qmlBrief:Mouse entering keypoint section.}

{qmlSignal:keypointMouseExit(int index, point pt)}
{qmlBrief:Mouse exiting keypoint section.}

{qmlSignal:keypointMouseSelected(int index, point pt)}
{qmlBrief:Keypoint section is selected.}

{qmlProperty:alias currentIndex keypointView.currentIndex}
{qmlBrief:Current index of the selected keypoint.}

{qmlType:FeatureDetectorSelection}
{qmlInherits:QtQuick.Rectangle}

{qmlProperty:string stateId}
{qmlBrief:Assigned when staticLoad is active.}

{qmlSignal:configurationUpdate}
{qmlBrief:Signal for configuration update.}

{qmlProperty:Mat detectorInput}
{qmlBrief:Input used for the detector.}

{qmlProperty: alias selectedName}
{qmlBrief:Name of the detector.}

{qmlProperty: alias selectedDetector}
{qmlBrief:Selected detector.}

{qmlProperty: alias selectedPanelModel}
{qmlBrief:Selected panel model.}

{qmlProperty: alias selectedIndex}
{qmlBrief:Selected index.}

{qmlProperty: color backgroundColor}
{qmlBrief:Background color used throughout the selections.}

{qmlProperty: color textColor}
{qmlBrief:Default text color}

{qmlProperty: int dropBoxHeight}
{qmlBrief:Height of the dropBox when selecting diferent feature detector.}

{qmlSignal: comboClicked}
{qmlBrief:Emited when selection happens.}

{qmlType:DescriptorExtractorSelection}
{qmlInherits:QtQuick.Rectangle}

{qmlProperty: string stateId}
{qmlBrief:Assigned when staticLoad is active.}

{qmlSignal: configurationUpdate}
{qmlBrief:Signal for configuration update.}

{qmlProperty: FeatureDetector detector}

{qmlProperty: alias selectedName}
{qmlBrief:Name of the extractor.}

{qmlProperty: alias selectedExtractor}
{qmlBrief:Selected extractor.}

{qmlProperty: alias selectedPanelModel}
{qmlBrief:Selected panel model.}

{qmlProperty: alias selectedIndex}
{qmlBrief:Selected index.}

{qmlProperty: color backgroundColor}
{qmlBrief:Background color used throughout the selections.}

{qmlProperty: color textColor}
{qmlBrief:Default text color}

{qmlProperty: int dropBoxHeight}
{qmlBrief:Height of the dropBox when selecting diferent feature detector.}

{qmlSignal: comboClicked}
{qmlBrief: Emited when selection happens.}

{qmlType:FeatureObjectList}
{qmlInherits:QtQuick.Rectangle}
{qmlBrief: List of features for an object}

{qmlProperty: FeatureDetector featureDetector}
{qmlProperty: DescriptorExtractor descriptorExtractor}

{qmlSignal: objectAdded(Mat descriptors, var points, var color)}
{qmlSignal: objectListLoaded(MatList list, var keypoints, var corners, var colors)}
{qmlSignal: objectListCreated()}

{qmlProperty: alias objectList}
{qmlBrief: Actual list.}

{qmlProperty: alias selectedIndex}
{qmlBrief: Index of the selected item.}

{qmlType:FeatureObjectMatch}
{qmlInherits:QtQuick.Rectangle}

{qmlProperty:FeatureDetector trainFeatureDetector}
{qmlProperty:DescriptorExtractor trainDescriptorExtractor}
{qmlProperty:FeatureDetector queryFeatureDetector}

{qmlProperty:var imageSource}
{qmlBrief:Image source}

{qmlProperty:var querySource}
{qmlBrief:Query source }

{qmlProperty:double minMatchDistanceCoeff}
{qmlBrief:Property for the actual matcher}

{qmlProperty:double matchNndrRation}
{qmlBrief:Property for the actual matcher}

{qmlSignal:objectListCreated}
{qmlBrief:Emited when the list is created}

{qmlProperty:KeyPointVector drawQueryKeypoints}
{qmlBrief:Keypoints recieved fr om the trainFeatureDetector}

{qmlProperty:int drawMatchIndex}
{qmlProperty:Index of the training images which match}

{qmlProperty: string stateId}
{qmlBrief:Static state id}

{qmlType:SelectionArea}
{qmlInherits:QtQuick.MouseArea}
{qmlBrief:Area that can be selected through a rectangle}

{qmlProperty:color selectionColor}
{qmlBrief:Color of the selection}

{qmlProperty:real selectionOpacity}
{qmlBrief:Opacity of the selection}

{qmlSignal:selected(int x, int y, int width, int height)}
{qmlBrief:Signal emited when selection happens}

{qmlProperty:Rectangle highlightItem}
{qmlBrief:Rectangle of the highlighted item}

{qmlType:KeyPoint}

{qmlProperty:Point2f pt}
{qmlBrief:Coordinates of the keypoints}

{qmlProperty:float size}
{qmlBrief:Diameter of the meaningful keypoint neighborhood}

{qmlProperty:float angle}
{qmlBrief:add brief}

{qmlProperty:float response}
{qmlBrief: The response by which the most strong keypoints have been selected}

Can be used for the further sorting or subsampling.

{qmlType:FeatureDetector}

{qmlProperty:input}
{qmlBrief: The actual image}

{qmlProperty:output}
{qmlBrief:Drawn features from the input to another image}

{qmlProperty:mask}
{qmlBrief:Limited amount of detections for a specific area}

{qmlProperty:params}
{qmlBrief:Given parameters}

{qmlProperty:keypoints}
{qmlBrief:Actual keypoints}


{qmlType:FastFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}
Adds separate params to FeatureDetector.

Initialize method arguments:

* **int threshold**

* **bool nonmaxSuppression**

{qmlType:BriskFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}

Feature detector for the brisk algorithm.

Initialize method arguments:

* **int thresh**

* **int octave**
* **float patternScale** 

{qmlType:OrbFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}

inherits feature detector

Initialize method arguments:

* **int nfeatures**
* **float scaleFactor**
* **int nlevels**
* **int edgeThreshold**
* **int firstLevel**
* **int WTA_K**
* **int scoreType**

{qmlType:MSerFeatureDetector}
{qmlInherits:lcvfeatures2d#FeatureDetector}

Initialize method arguments:

* **int delta**
* **int minArea**
* **int maxArea**
* **float maxVariation**
* **float minDiversity**
* **int maxEvolution**
* **double areaThreshold**
* **double minMargin**
* **int edgeBlurSize**

{qmlType:simpleBlobDetector}

implements a simple algorithm for extracting blobs from an image

The class implements a simple algorithm for extracting blobs from an image:
1. Convert the source image to binary images by applying thresholding with several thresholds from
   minThreshold (inclusive) to maxThreshold (exclusive) with distance thresholdStep between
   neighboring thresholds.

2. Extract connected components from every binary image by findContours and calculate their
   centers.

3. Group centers from several binary images by their coordinates. Close centers form one group that
   corresponds to one blob, which is controlled by the minDistBetweenBlobs parameter.
   
4. From the groups, estimate final centers of blobs and their radiuses and return as locations and
   sizes of keypoints.

This class performs several filtrations of returned blobs. You should set filterBy to **true/false**
to turn **on/off** corresponding filtration.

Params:

* **filterByInertia**
* **filterByConvexity**
* **filterByColor**
* **filterByCircularity**
* **filterByArea**

{qmlType:DescriptorExtractor}
{qmlBrief:Abstract base class for computing descriptors for image keypoints.}

{qmlProperty:keypoints}

{qmlProperty:descriptors}

{qmlProperty:params}

{qmlProperty:isBinary}

{qmlType:BriskDescriptorExtractor}
{qmlInherits:lcvfeatures2d#DescriptorExtractor}

{qmlType:OrbDescriptorExtractor}
{qmlInherits:lcvfeatures2d#DescriptorExtractor}

Params:

* **int patchSize**

{qmlType:DMatchVector}

{qmlInherits:QtQuick.Vector}

Data that is shared throughout qml.

{qmlType:DescriptorMatcher}
{qmlBrief:Abstract base class for matching keypoint descriptors.}

It has two groups of match methods: for matching descriptors of an image with another image or with an image set.

Params:

* **querdyDescriptors** Query set of descriptors.
* **knn** Count of best matches found per each query descriptor or less if a query descriptor has less than k possible matches in total.

{qmlType:BruteForceMatcher}
{qmlInherits:lcvfeatures2d#DescriptorMatcher}

params:

* **int normType**
* **bool crossCheck**

{qmlType:FlannBasedMatcher}
{qmlInherits:lcvfeatures2d#DescriptorMatcher}

params:

{qmlType:DrawMatches}
{qmlInherits:lcvfeatures2d#DescriptorMatcher}

Draws matches between two sets of keypoints that were detected and the matches that were found
Specify match index to focus on specific match

{qmlType:DescriptorMatchFilter}
{qmlInherits:lcvfeatures2d#DescriptorMatcher}

{qmlProperty:matches1to2}
{qmlBrief:Matches from the first source to second one.}

{qmlProperty:matches1to2Out}
{qmlBrief:Output of matches1to2.}

{qmlProperty:minDistanceCoeff}
{qmlBrief:Minimum distance that the match has to the actual outcome.}

{qmlProperty:maxDistance}
{qmlBrief:Maximum distance allowed betwen set of two matches.}

{qmlProperty:nndrRatio}
{qmlBrief:The match betwen the first match and it's next consecutive match.}

{qmlType:KeypointsToSceneMap}
{qmlInherits:QtQuick.object}
{qmlProperty:size}
{qmlBrief: Size of the vector of KeypointsToScene object}

{qmlType:MatchesToLocalKeypoint}
{qmlInherits:}

{qmlProperty:matches1to2}
{qmlBrief:Matches from the first source to second one.}

{qmlProperty:trainKeypointVectors}

{qmlProperty:queryKeypointVector}

{qmlProperty:output}

{qmlSignal:matches1to2Changed}
{qmlBrief:Triggered when matches from the first source to second one are changed.}

{qmlSignal:trainKeypointVectorsChanged}
{qmlBrief:Trigered when trainKeypointVectors is changed}

{qmlSignal:queryKeypointVectorChanged}
{qmlBrief:Trigered when queryKeypointVector is changed}

{qmlSignal:outputChanged}
{qmlBrief:Triggered when output is changed}

{qmlType:KeypointHomography}
{qmlBrief:Maps the keypoints in one image to the corresponding keypoints in the other image}

{qmlProperty:KeypointsToScene}

{qmlProperty:QueryImage}

{qmlProperty:objectCorners}

{qmlProperty:objectColors}

{qmlSignal:keypointsToSceneChanged}
{qmlBrief:Triggered when KeypointsToScene is changed.}

{qmlSignal:queryImageChanged}
{qmlBrief:Triggered when QueryImage is changed.}

{qmlSignal:objectCornersChanged}
{qmlBrief:Trigered when objectCorners is changed.}

{qmlSignal:objectColorsChanged}
{qmlBrief:Trigered when objectColors is changed.}
