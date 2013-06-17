/-----------------------------------------------------------------\
* 
*    Live CV [version 0.1]
*    =====================
* 
\-----------------------------------------------------------------/


 1. Compatibility
-----------------

 * Microsoft Visual Studio 2012
 * Microsoft Visual Studio 2010
 * Mingw 4.7
 * Gcc 4.4

 2. Dependencies
----------------

 * Qt5
 * OpenCv 2.4 or later

 3. Configuration
-----------------

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
