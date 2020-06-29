import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import editor 1.0
import workspace 1.0

Theme{
    id: root

    // Color Scheme

    property QtObject colorScheme: QtObject{

        property color background: '#03090d'
        property color backgroundOverlay: '#060e13'

        property color backgroundBorder: '#232f37'

        property color middleground: '#141b20'
        property color middlegroundOverlay: '#292d34'
        property color middlegroundOverlayDominant: '#3f444d'

        property color middlegroundBorder: '#232b30'
        property color middlegroundHighlight: '#062945'

        property color middlegroundOverlayDominantBorder: '#575b63'

        property color foregroundFaded: '#dbdede'
        property color foreground: '#fff'

        property color textSelection: '#0b273f'
    }

    // Icons

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

    // Panes

    property QtObject paneMenuStyle: QtObject{
        property color backgroundColor: colorScheme.middleground
        property double radius: 0

        property QtObject itemStyle: TextStyle{
            color: colorScheme.foregroundFaded
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Light,
                italic: false,
                pixelSize: 11
            })
        }
        property QtObject itemHoverStyle: TextStyle{
            color: Qt.lighter(colorScheme.foregroundFaded)
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Light,
                italic: false,
                pixelSize: 11
            })
        }
    }

    property color paneBackground: colorScheme.background
    property color panebackgroundOverlay: colorScheme.backgroundOverlay
    property color paneTopBackground: colorScheme.middleground
    property color paneTopbackgroundOverlay: colorScheme.middleground
    property color paneSplitterColor : colorScheme.middleground

    property color projectPaneItemBackground: "transparent"
    property color projectPaneItemEditBackground : "#1b2934"

    // Scroll

    property Component scrollStyle: Component{

        ScrollViewStyle {
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#1f2227"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: 'transparent'
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Item{}
            corner: Rectangle{color: 'transparent'}
        }

    }

    // Forms

    property color inputColor: '#3f444d'
    property color inputBorderColor: '#fff'
    property int   inputBorderWidth: 1

    property QtObject inputLabelStyle: QtObject{
        property color background: colorScheme.middleground
        property double radius: 3
        property QtObject textStyle: TextStyle{
            color: colorScheme.foregroundFaded
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Light,
                italic: false,
                pixelSize: 11
            })
        }
    }

    property QtObject inputStyle: QtObject{
        property QtObject textStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property double radius: 2
        property QtObject hintTextStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property color backgroundColor: colorScheme.middleground
        property color borderColor: colorScheme.backgroundBorder
        property double borderThickness: 1
        property color textSelectionColor: colorScheme.textSelection
    }


    property QtObject monoInputStyle: QtObject{
        property QtObject textStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property double radius: 3
        property QtObject hintTextStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property color backgroundColor: colorScheme.background
        property color borderColor: colorScheme.backgroundBorder
        property double borderThickness: 1
        property color textSelectionColor: colorScheme.textSelection
    }


    property QtObject formButtonStyle : QtObject{
        property QtObject textStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property QtObject hoverTextStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property color backgroundColor: colorScheme.middlegroundOverlayDominant
        property color backgroundHoverColor: Qt.lighter(colorScheme.middlegroundOverlayDominant, 1.2)
        property color borderColor: colorScheme.middlegroundOverlayDominantBorder
        property color borderHoverColor: colorScheme.middlegroundOverlayDominantBorder
        property double borderThickness: 1
        property double radius: 3
    }

    // Buttons

    property QtObject buttons: QtObject{

        property Component apply: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.formButtonStyle

            content: CheckMarkIcon{
                anchors.centerIn: parent
                width: parent.parent.width / 3
                height: parent.parent.height / 3
                strokeWidth: 1
            }
        }

        property Component connect: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.formButtonStyle

            content: Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-connections.png"
            }
        }
    }

    // Lists

    property QtObject selectableListView: QtObject{
        property QtObject labelStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property color backgroundColor: colorScheme.background
        property color selectionBackgroundColor: colorScheme.middlegroundOverlayDominant
        property double radius: 2
        property color borderColor: colorScheme.middlegroundOverlayDominant
        property double borderWidth: 1
        property double opacity: 0.95
        property Component scrollStyle: root.scrollStyle
    }

    // Node Editor

    property QtObject nodeEditor : QtObject{
        property color backgroundColor: root.colorScheme.background
        property color backgroundGridColor: root.colorScheme.middleground

        property color borderColor: root.colorScheme.middlegroundOverlayDominant
        property double borderWidth: 1
        property double radius: 3

        property color connectorEdgeColor: root.colorScheme.middlegroundOverlayDominant
        property color connectorColor: root.colorScheme.middlegroundOverlayDominant

        property color selectionColor: root.colorScheme.foreground
        property double selectionWeight: 1

        property QtObject objectNodeStyle : QtObject{
            property color background: root.colorScheme.middleground
            property double radius: 15
            property color borderColor: root.colorScheme.middlegroundOverlayDominant
            property double borderWidth: 1

            property color titleBackground: root.colorScheme.middlegroundOverlayDominant
            property double titleRadius: 3
            property QtObject titleTextStyle : TextStyle{}
        }

        property QtObject propertyDelegateStyle : QtObject{
            property color background: root.colorScheme.middlegroundOverlay
            property double radius: 5
            property QtObject textStyle: TextStyle{}
        }
    }
}
