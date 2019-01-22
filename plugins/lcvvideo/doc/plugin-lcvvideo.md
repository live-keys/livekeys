# Plugin `lcvvideo`

 This module contains elements for motion analysis and object tracking. It resembles open cv's video module.

```js
import lcvvideo 1.0
```

{plugin:lcvvideo}
{qmlSummary:lcvvideo}

{qmlType:CalcOpticalFlowPyrLK}
{qmlInherits:MatFilter}
{qmlBrief:Sparse optical flow filter.}

{qmlMethod:CalcOpticalFlowPyrLK addPoint(Point p)}

  Add a [point to track]().

  Calculate an optical flow for a sparse feature set using the iterative Lucas-Kanade method with pyramids. The function
  implements a sparse iterative version of the Lucas-Kanade optical flow in pyramids.

  See [motion analysis and object tracking](http://docs.opencv.org/modules/video/doc/motion_analysis_and_object_tracking.html for details. 
  )
  Note that this element manages it's points automatically. You can add a point by using the addPoint() function, or
  you can get a list of all the points by using the points() getter.

  ```
  var points = lkflow.points()
  ```

  A sample is available in **samples/video/lktracking.qml** :

  `video/lktracking.qml`


{qmlBrief:Adds a [point to the vector of points that are currently tracked]().}
  See also [CalcOpticalFlowPyrLK::addPoint]().


 
{qmlMethod:list<Point> CalcOpticalFlowPyrLK points()}

  This method retrieves a copy of the list of points that are currently traked.

{qmlBrief:Returns the total number of points that are currently tracked as a list of points.}
  See also [CalcOpticalFlowPyrLK::points()]().

{qmlMethod:int CalcOpticalFlowPyrLK totalPoints()}

  Returns the total number of points that are currently tracked.

{qmlBrief:Returns the total number of points that are currently tracked.}

{qmlMethod:CalcOpticalFlowPyrLK staticLoad(string key)}

  Loads the CalcOpticalFlowPyrLK state from the given \a key.

{qmlProperty:Size CalcOpticalFlowPyrLK winSize}

  Size of the search window at each pyramid level.

{qmlProperty:int maxLevel}

  0-based maximal pyramid level number; if set to 0, pyramids are not used (single level), if set to 1, two levels are
  used, and so on; if pyramids are passed to input then algorithm will use as many levels as pyramids have but no more
  than 'maxLevel'.

{qmlProperty:real minEigThreshold}

  The algorithm calculates the minimum eigen value of a 2x2 normal matrix of optical flow equations (this matrix is
  called a spatial gradient matrix in [Bouguet00]_), divided by number of pixels in a window; if this value is less than
  'minEigThreshold', then a corresponding feature is filtered out and its flow is not processed, so it allows to remove
  bad points and get a performance boost.

{qmlType:BackgroundSubtractor}
{qmlInherits:MatFilter}
{qmlBrief:Background subtractor base type.}

{qmlProperty:string learningRate}

  Learning rate for updating the background model (0 to 1, default is 0).

{qmlType:BackgroundSubtractorMog2}
{qmlInherits:BackgroundSubtractor}

{qmlBrief:Gaussian mixture based background/foreground segmentation algorithm.This is a **static item**.}


`video/backgroundsubtractormog2.qml`

{qmlProperty:Mat backgroundModel}

  Snapshot of the background model computed by the MOG2 algorithm.

{qmlProperty:int history}

  Length of the history. Defaults to 500.

{qmlProperty:int nmixtures}

  Maximum allowed number of mixture components. Defaults to 5.

{qmlProperty:int nShadowDetection}

  The value for marking shadow pixels in the output foreground mask.
  Must be in the range 0-255. Defaults to 127.

{qmlProperty:bool detectShadows}

  Whether shadow detection should be enabled. Defaults to false.

{qmlProperty:double backgroundRatio}

  Threshold defining whether the component is significant enough to be included into the background model.
  Defaults to 0.9.

{qmlProperty:double ct}

  Complexity reduction parameter. Defaults to 0.05.

{qmlProperty:double tau}

  Shadow threshold. Defaults to 0.5.

{qmlProperty:double varInit}

  Initial variance for the newly generated components. Defaults to 15.

{qmlProperty:double varMin}

  Parameter used to further control the variance. Defaults to 4.

{qmlProperty:double varMax}

  Parameter used to further control the variance. Defaults to 75.

{qmlProperty:double varThreshold}

  Threshold on the squared Mahalanobis distance to decide whether it is well described by the background model.
  Defaults to 16.

{qmlProperty:double varThresholdGen}

  Threshold for the squared Mahalanobis distance that helps decide when a sample is close to the existing components.
  Defaults to 9.

{qmlMethod:BackgroundSubtractorMog2 staticLoad(string key)}

  Loads the BackgroundSubtractorMog2 state from the given [key]().

{qmlType:BackgroundSubtractorKnn}
{qmlInherits:BackgroundSubtractor}
{qmlBrief:K-nearest neigbours based background/foreground segmentation algorithm.  This is a **static item**.}

{qmlProperty:bool detectShadows}

  Enables or disables shadow detection.

{qmlProperty:double dist2Threshold}

  Threshold on the squared distance.

{qmlProperty:int history}

  Number of last frames that affect the background model.

{qmlProperty:int knnSamples}
 
  How many nearest neigbours need to match.

{qmlProperty:int nSamples}

  Number of data samples in the background model.

{qmlProperty:double shadowThreshold}

  Shadow threshold.

{qmlProperty:int shadowValue}

  Pixel value for pixels detected as shadow.

{qmlMethod:BackgroundSubtractorKnn staticLoad(string key)}

  Loads the BackgroundSubtractorKnn state from the given [key]().









