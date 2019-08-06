import QtQuick 2.3
import editor 1.0

Theme{
    property color background: "#fff"

    property color topBackground : "transparent"

    property string topNewIcon: "qrc:/images/top-icon-new.png"
    property string topSaveIcon: "qrc:/images/top-icon-save.png"
    property string topOpenFileIcon: "qrc:/images/top-icon-openfile.png"
    property string topOpenProjectIcon: "qrc:/images/top-icon-openproject.png"
    property string topViewLogIcon: "qrc:/images/top-icon-viewlog.png"
    property string topRunCommandIcon: "qrc:/images/top-icon-runcommand.png"
    property string topOpenSettingsIcon: "qrc:/images/top-icon-opensettings.png"
    property string topOpenLicenseIcon: "qrc:/images/top-icon-openlicense.png"

    property string topLiveModeIcon : "qrc:/images/top-icon-mode-live.png"
    property string topOnSaveModeIcon : "qrc:/images/top-icon-mode-onsave.png"
    property string topDisabledModeIcon: "qrc:/images/top-icon-mode-disabled.png"

    property color paneBackground: "#03090d"
    property color paneTopBackground: "#0b1015"
    property color paneTopBackgroundAlternate: "#050b12"
    property color paneSplitterColor : "#10171f"

    property color projectPaneItemBackground: "transparent"
    property color projectPaneItemEditBackground : "#1b2934"
}
