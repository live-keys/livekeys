# Plugin `lcvphoto`

Wrappers for opencv.photo library.

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
