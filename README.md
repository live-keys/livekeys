# Live CV

**Real Time Computer Vision Coding Environment**

Live CV combines the features of QTQuick and Open CV to create an 
environment for real-time computer vision coding.

## Source Dependencies

 * Qt5 or later
 * OpenCv 2.4 or later

## Compiler Configuration

All modules are available through qmake projects. Configuration deals
with setting up paths to opencv. To ease this process on windows, the
project  file "ConfigOpenCv.pro" handles most  of these dependencies.
It requires only 3 paths to be set :

 * PATH_OPENCV_INCLUDE
 * PATH_OPENCV_LIBRARIES
 * PATH_OPENCV_DLLS

To set these paths  without interfering with  the actual project, you
can add these in a separate file "Config.pro". The file is ignored by
git and can be used by each user in within his own environment.