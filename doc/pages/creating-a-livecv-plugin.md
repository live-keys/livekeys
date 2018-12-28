# Creating a Live CV based Plugin

In order to use Open CV types within QtQuick, Live CV provides a set of wrapper libraries. We will be using lcvcore for
this scenario since we will need the `QMat` type, so we will link to lcvcore and Open CV from our plugin. In qmake,
linking to the 2 libraries requires a lot of boiler plate code:

```

# Link to Open CV

win32{
	OPENCV_DIR_INCLUDE   = $$(OPENCV_DIR)/../../include
    OPENCV_DIR_LIBRARIES = $$(OPENCV_DIR)/lib
	
	INCLUDEPATH += $${OPENCV_DIR_INCLUDE}
	LIBS += -L$${OPENCV_DIR_LIBRARIES} -lopencv_world310
}
unix{
	CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

# Link to lcvcore

PATH_TO_LIVECV = '<path_to_livecv>'

INCLUDEPATH += $$PATH_TO_LIVECV/dev/include/plugins/lcvcore
LIBS += -L$$PATH_TO_LIVECV/dev/plugins/lcvcore -llcvcore
```

so to simplify this, Live CV already provides a set of qmake files that simplify these types of operations. The files
are located in the `project` folder of our Live CV installation directory. We will need the `functions.pri` file,
together with the `3rdparty/opencv.pri` file included in our `tutorial.pro`:

```
LIVECV_BIN_PATH = <path_to_livecv>
LIVECV_DEV_PATH = $$LIVECV_BIN_PATH/dev

include($$LIVECV_DEV_PATH/project/functions.pri)
include($$LIVECV_DEV_PATH/project/3rdparty/opencv.pri)
```

`functions.pri` expects us to define `LIVECV_BIN_PATH` and `LIVECV_DEV_PATH`, which are used as locations to
find the plugins we will be linking to. Note that on windows, in order to link to Open CV, you will also need to have
`OPENCV_DIR` [environment variable](http://docs.opencv.org/2.4/doc/tutorials/introduction/windows_install/windows_install.html)
set up.
After we have the functionality available, we can link to the lcvcore library:

```
linkPlugin(lcvcore, lcvcore)
```

The `linkPlugin` function takes 2 parameters, a location where the plugin resides, and a plugin name to be used.
Open CV is linked to automatically by just having the `opencv.pri` file included. After the appended lines
, the top part of `tutorial.pro` file should look something similar to:

```
TEMPLATE = lib
TARGET = tutorial
QT += qml quick
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = tutorial

LIVECV_BIN_PATH = <path_to_livecv>
LIVECV_DEV_PATH = $$LIVECV_BIN_PATH/dev

include($$LIVECV_DEV_PATH/project/functions.pri)
include($$LIVECV_DEV_PATH/project/3rdparty/opencv.pri)

linkPlugin(lcvcore,  lcvcore)

# ...
```

After solving our dependencies, we can now go back to our `CountNonZeroPixels` item. We're going to need 
an input matrix, and an output value for the number of pixels:

```
Q_PROPERTY(QMat* input         READ input         WRITE setInput         NOTIFY inputChanged)
Q_PROPERTY(int   nonZeroPixels READ nonZeroPixels NOTIFY nonZeroPixelsChanged)
```

The QMat element requires us to include the `qmat.h` file at the beginning:

```
#include "qmat.h"
```

Last, we need to add the 2 fields, setters, getters and notifiers, and we have the final version looking like this:

```
#include <QQuickItem>
#include "qmat.h"

class CountNonZeroPixels : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(CountNonZeroPixels)
    Q_PROPERTY(QMat* input         READ input         WRITE setInput         NOTIFY inputChanged)
    Q_PROPERTY(int   nonZeroPixels READ nonZeroPixels NOTIFY nonZeroPixelsChanged)

public:
    CountNonZeroPixels(QQuickItem *parent = 0);
    ~CountNonZeroPixels();

    QMat* input();
    void setInput(QMat* input);

    int nonZeroPixels() const;

signals:
    void inputChanged();
    void nonZeroPixelsChanged();

private:
    QMat* m_input;
    int   m_nonZeroPixels;
};

inline int CountNonZeroPixels::nonZeroPixels() const{
    return m_nonZeroPixels;
}

inline QMat*CountNonZeroPixels::input(){
    return m_input;
}

inline void CountNonZeroPixels::setInput(QMat* input){
    m_input = input;
    emit inputChanged();
    m_nonZeroPixels = cv::countNonZero(*m_input->cvMat());
    emit nonZeroPixelsChanged();
}
```

In the `setInput` method, we calculate the non zero pixels using the cv::CountNonZero method from Open CV. To
retrieve the actual cv::Mat contents of a QMat, we use the `cvMat()` getter, which retrieves a pointer to a cv::Mat
object.

We can now build the library and test it. Just like the steps in the previous part, go to the  **plugins** directory in
Live CVs installation path, and override the qmldir and the `so` or `dll` files with the newly build ones. Run
livecv, and you can use the following code to test your plugin:

```
import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0
import tutorial 1.0

Grid{

    columns : 2

    property string imagePath : 'path/to/image.jpg'

    ImRead{
        id : imgSource
        file : parent.imagePath
    }

    Canny{ // get the edges
        id : canny
        input: imgSource.output
        threshold1 : 70
        threshold2 : 210
    }

    CountNonZeroPixels{ // count the number of edge pixels
        id : count
        input : canny.output
    }

    Text{
        color : "#fff"
        text : count.nonZeroPixels
    }

}
```

Configure the imagePath property to open an image on your hard drive. If you see a text just below your image with
a large number, then you have succesfully created a Live CV plugin that counts the number of non zero pixels in an
image.


