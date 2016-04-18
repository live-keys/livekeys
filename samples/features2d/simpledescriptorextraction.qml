import QtQuick.Controls 1.2
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0
import lcvcontrols 1.0

Column {

    // This sample shows how to extract descriptors out of an image. Descriptors are mostly
    // used to train matchers for different objects. Here, we only use it to output the results
    // into a text section. You can substitute the BriefDescriptorExtractor with 'OrbDescriptorExtractor',
    // 'FreakDescriptorExtractor', etc to view different results.

    property string imagePath: 
        codeDocument.path + '/../_images/object_101_piano_train1.jpg'

    spacing: 10

    Row{

        ImRead{
            id : inputImage
            file : imagePath
        }

        FastFeatureDetector{
            id : featureDetector
            input : inputImage.output
            Component.onCompleted : {
                initialize({
                    threshold: 20,
                    nonmaxSuppression: true
                })
            }
        }

        BriefDescriptorExtractor{
            id : descriptorExtractor
            keypoints : featureDetector.keypoints
            onDescriptorsChanged : {
                var values = descriptors.data().values()
                var len = values.length
                var text = ''
                for (var i = 0; i < values.length; ++i)
                    text += 'Descriptors[' + i + ']: ' + values[i].toString() + '\n'
                descriptorValues.text = text
            }
        }
    }

    Rectangle{
        width : 520
        height : 300
        color : "#1a1f2a"

        ScrollView {
            anchors.fill : parent
            style : LiveCVScrollStyle {}

            Text {
                id : descriptorValues
                text : ""
                color : "#eee"
                font.family : "Source Code Pro, Courier New, Courier"
                lineHeight : 1.4
            }
        }
    }
}
