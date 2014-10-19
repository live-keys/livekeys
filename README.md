# Live CV

**Live Computer Vision Coding Environment**

Website : http://livecv.dinusv.com

Live CV is a computer vision coding environment that displays results in real time. It can be used to interact, link and adjust algorithms in order to create quick solutions within it's domain. 

The application is built using QtQuick, and uses Open CV library in the back to do the actual processing. From this perspective, Live CV can also be considered a QML wrapper for the Open CV library. 

An API is also available to link already built algorithms to Live CV's interface by simply exposing configurable variables to Qt's meta object system.

If you are interested in this project, please subscribe to the newsletter on the homepage. This helps me know people are interested, and I can invest more time in coding more features and provide faster releases.

## Dependencies

 * Qt5 or later
 * OpenCv 2.4 or later

## Compiler Configuration

All modules are available through qmake projects. 

 * In **windows**, it is required to configure paths for opencv include directories, libs and dlls. These can be found within the Config.pro file in the main project directory. Additionally, the version of opencv is required, so as to be appended when each lib or dll file is loaded. The below variables are the ones that require configuration :

	* PATH\_OPENCV\_INCLUDE
	* PATH\_OPENCV\_LIBRARIES
	* PATH\_OPENCV\_DLLS
	* VERSION\_OPENCV
	
	The faster version would be to simply copy Open CV's build directory into the opencv directory of live cv and just configure the VERSION_OPENCV varialbe in the config file.

 * In **unix**, by default, no configuration should be required. If you are having trouble though, you can consult articles on how to link open cv with qmake.
