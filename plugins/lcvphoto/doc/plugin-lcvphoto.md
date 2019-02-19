# Plugin `lcvphoto`

This module contains elements for photo processing. It resembles open cv's photo module.

```js
import lcvphoto 1.0
```

{plugin:lcvphoto}
{qmlSummary:lcvphoto}

{qmlType:FastNlMeansDenoising}
{qmlInherits:lcvcore#MatFilter}
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
{qmlInherits:lcvphoto#FastNlMeansDenoising}
{qmlBrief:Denoises a grayscale or color image using multiple frames.}

Variant of FastNlMeansDenoising using a history of frames.
Implemented with a sliding window, which makes it suitable for video input.

All FastNlMeansDenoising options also apply to FastNlMeansDenoisingMulti.

Usage notes for video input:

- The algorithm is rather slow, which can be a problem for live video processing.
  Tweak the input framerate, searchWindowSize and temporalWindowSize as required.
- The output will lag (temporalWindowSize-1)/2 frames behind the input.
- Output will only start when the buffer is filled; expect temporalWindowSize-1
  black output frames directly after recompiling the QML.

{qmlProperty:int temporalWindowSize}

Size of the sliding window in frames. Higher values keep a longer history,
which requires more memory and processing power and increases output delay.
Must be an odd number. Defaults to 3.

{qmlType:DenoiseTvl1}
{qmlInherits:lcvcore#MatFilter}
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

{qmlType:HueSaturationLightness}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Controls the hue and saturation of an image}

{qmlProperty:int hue}
{qmlBrief: Controls the hue}

{qmlProperty:int saturation}
{qmlBrief: Controls the saturation}

{qmlProperty:int lightness}
{qmlBrief: Controls the lightness}

{qmlType:Levels}
{qmlInherits:lcvcore#MatFilter}


{qmlProperty:Lightness}
{qmlBrief:Controls the lightness overall channels}

`[white, contrast, black]`

{qmlProperty:channels}
{qmlBrief:Recieves the channel index and lightness parameters for the image}

`channel index: [white, contrast, black]`

```
Levels{
        lightness: [10, 1.0, 255]
        channels: {
            0: [0, 1.0, 170]
        }
        ```
{qmlType:AutoLevels}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Given the histogram AutoLevels will aoutmatically compute levels of an image.}


{qmlType:BrightnessAndContrast}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Adjust the brightness and the contrast}

{qmlProperty:double brightness}

Can be any value

{qmlProperty:double contrast}

Should be between 0 and 10

{qmlType:Sticher}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Stiches a set of images toghether based on the list of matrixes and a set of parameters}

{qmlProperty:lcvcore#MatList input}

Matrix input list to the Stitcher.

{qmlProperty:Object params}

Input parameters:

 * `mode` : `Stitcher.Panorama` or `Stitcher.Scans`
 * `tryUseGpu` : `true` for gpu usage, `false` otherwise


{qmlType:AlignMTB}
{qmlInherits:MatFilter}
{qmlBrief:Takes a list of inputs with a set of parameters and it converts them}

Params:

* **int maxBits**
* **int excludeRange**
* **bool cut**

{qmlType:CalibrateDebevec}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Calibrates the lightness of the image in regard to exsposure setting the image was shot.}

{qmlProperty:QMatList input}

List of images

{qmlProperty:QList times}
Exposure time

{qmlProperty:QVariantMap params}
Calibration parameters

Params:

* **int samples**
* **float lambda**
* **bool random**

{qmlProperty:QMat output}
{qmlBrief:Outputs single image based on calibrations}

Output will contain much more data than the input.

{qmlType:CalibrateRobertson}
{qmlInherits:lcvcore#MatFilter}
{qmlBrief:Calibrates the lightness of the image in regard to exsposure setting the image was shot.}

{qmlProperty:QMatList input}
{qmlBrief:List of images}

{qmlProperty:QList times}
{qmlBrief:Exposure time}

{qmlProperty:QVariantMap params}
{qmlBrief:Calibration parameters}

Params:

* **int maxIter**
* **float threshold**

{qmlProperty:QMat output}
{qmlBrief:Outputs single image based on calibrations}

Output will contain much more data than the input.

{qmlType:MergeDebevec}
{qmlBrief:The resulting HDR image is calculated as weighted average of the exposures considering exposure values and camera response.}

Merges images.

If calibrated with **CalibrateDebevec**, **MergeDebevec** must be used.

Parameters:

* **src** vector of input images
* **dst** result image
* **times** vector of exposure time values for each image
* **response** 256x1 matrix with inverse camera response function for each pixel value, it should have the same number of channels as images.


{qmlType:MergeRobertson}
{qmlBrief:The resulting HDR image is calculated as weighted average of the exposures considering exposure values and camera response.}

Merges images.

If calibrated with **CalibrateRobertson**, **MergeRobertson** must be used.

Parameters:

* **src** vector of input images
* **dst** result image
* **times** vector of exposure time values for each image
* **response** 256x1 matrix with inverse camera response function for each pixel value, it should have the same number of channels as images.

{qmlType:ToneMap}
{qmlBrief:First step of transforming the image from the HDR to something that is viewable by the user}

{qmlProperty:QVariantMap input}
{qmlProperty:QVariantMap params}

{qmlType:TonemapDrago}
{qmlInherits:lcvphoto#ToneMap}

Initialize method arguments:

* **float gamma**
* **float saturation**
* **float bias**

{qmlType:TonemapDurand}
{qmlInherits:lcvphoto#ToneMap}

Initialize method arguments:

* **float gamma**
* **float contrast**
* **float saturation**
* **float sigmaSpace**
* **float sigmaColor**

{qmlType:TonemapMantiuk}
{qmlInherits:lcvphoto#ToneMap}

Initialize method arguments:

* **float gamma**
* **float scale**
* **float saturation**

{qmlType:TonemapReinard}
{qmlInherits:lcvphoto#ToneMap}
Initialize method arguments:
    
* **float gamma**
* **float intensity**
* **float lightAdapt**
* **float colorAdapt**

{qmlType:LevelSliders}
{qmlInherits:external.QtQuick#Rectangle}

{qmlProperty:alias input}

{qmlProperty:lightness}

{qmlProperty:levelByChannel}


{qmlType:HueSaturationLightnessSliders}

{qmlProperty:alias hue}
{qmlInherits:external.QtQuick#Rectangle}
{qmlBrief:Hue that's controled by the slider}

{qmlProperty:alias saturation}
{qmlBrief:Saturation that's controled by the slider}

{qmlProperty:alias lightness}
{qmlBrief:Lightness that's controled by the slider}
