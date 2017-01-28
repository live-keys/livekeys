# ChangeLog

This file summarises notable changes between Live CV versions.

## 1.3.0

### Compatibility:
   - All files now require manual import of QtQuick module. (e.g. ```import QtQuick 2.3```)
   - Use ```project.dir()``` instead of ```codeDocument.path``` when refering tothe path of the project

### Features:
   - Code Completion support
   - Project management 
   - Project file navigation
   - Script execution and the *live* module (```livecv script.qml```)
   - Argument forwarding to qml

## 1.2.0

### Features:
   - Added lcvfeatures module.
   - Added build scripts.

## 1.1.0

### Compatibility:

   - Using ```import lcvcore 1.0``` instead of ```import "lcvcore 1.0" ```

### BugFixes:

   - Files are now able to import local directories relative to their location 

## 1.0.0

