# Plugin `lcvphoto`

  This module contains elements for photo processing. It resembles open cv's photo module.

```js
import lcvphoto 1.0
```

{plugin:lcvphoto}
{qmlSummary:lcvphoto}

{qmlType:FastNlMeansDenoising}
{qmlInherits:MatFilter}
{qmlBrief:Denoises a grayscale or color image.}

  Performs denoising using the Non-local Means Denoising algorithm.

`photo/fastnlmeansdenoising.qml`

{qmlProperty:bool colorAlgorithm}

  False to use fastNlMeansDenoising, true to use fastNlMeansDenoisingColored.
  By default, this is autodetected based on the number of channels in the source.


{qmlProperty:float h}

  Parameter regulating filter strength. Defaults to 3.

{qmlProperty:float hColor}

  The same as h but for color components. Defaults to 3.
  Only relevant when using the color algorithm (see colorAlgorithm).

{qmlProperty:int templateWindowSize}

  Size in pixels of the template patch that is used to compute weights.
  Should be odd. Defaults to 7.

{qmlProperty:int searchWindowSize}

  Size in pixels of the window that is used to compute weighted average for given pixel.
  Has a large performance impact. Should be odd. Defaults to 21.


{qmlType:FastNlMeansDenoisingMulti}
{qmlInherits:FastNlMeansDenoising}
{qmlBrief:Denoises a grayscale or color image using multiple frames.}

  Variant of FastNlMeansDenoising using a history of frames.
  Implemented with a sliding window, which makes it suitable for video input.

  All FastNlMeansDenoising options also apply to FastNlMeansDenoisingMulti.

  Usage notes for video input:
  * The algorithm is rather slow, which can be a problem for live video processing.
    Tweak the input framerate, searchWindowSize and temporalWindowSize as required.
  * The output will lag (temporalWindowSize-1)/2 frames behind the input.
  * Output will only start when the buffer is filled; expect temporalWindowSize-1
    black output frames directly after recompiling the QML.

{qmlProperty:int temporalWindowSize}

  Size of the sliding window in frames. Higher values keep a longer history,
  which requires more memory and processing power and increases output delay.
  Must be an odd number. Defaults to 3.
<<<<<<< HEAD

{qmlType:DenoiseTvl1}
{qmlInherits:MatFilter}
{qmlBrief:Denoises a grayscale or color image.}

  Performs denoising using the primal-dual algorithm.


{qmlProperty:int bufferSize}

  Number of observations or noised versions of the image to be restored.


{qmlProperty:real lambda}

  As it is enlarged, the smooth (blurred) images are treated more favorably than detailed
  (but maybe more noised) ones. Roughly speaking, as it becomes smaller, the result will be
  more blur but more sever outliers will be removed.

{qmlProperty:int nIters}

  Number of iterations that the algorithm will run. The more iterations the better, but it is
  hard to quantitatively refine this statement, so just use the default and increase it if the
  results are poor.
  
=======
>>>>>>> 1247952f130bf756fa08d4efce39faff91d8482b
