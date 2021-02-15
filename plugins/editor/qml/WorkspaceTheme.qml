import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import workspace 1.0
import workspace.icons 1.0
import timeline 1.0
import visual.input 1.0 as Input

Theme{
    id: root

    // Color Scheme

    property QtObject colorScheme: QtObject{

        property color background: '#05080a'
        property color backgroundOverlay: '#0a1014'

        property color backgroundBorder: '#232f37'

        property color middleground: '#141b20'
        property color middlegroundOverlay: '#292d34'
        property color middlegroundOverlayDominant: '#3f444d'

        property color middlegroundBorder: '#232b30'
        property color middlegroundHighlight: '#062945'

        property color middlegroundOverlayBorder: '#3f444d'
        property color middlegroundOverlayDominantBorder: '#575b63'

        property color foregroundFaded: '#dbdede'
        property color foreground: '#fff'

        property color textSelection: '#0b273f'
        property color scrollbarColor: '#1f2227'

        property color topIconColor: '#b3bdcc'
        property color topIconColorAlternate: '#3f4449'

        property color error: '#ba2020'
        property color warning: '#b99311'

        property color warningForeground: '#fcd612'
        property color errorForeground: '#ff4e4e'
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

    property Component stackIcon: StackIcon{}
    property Component fileIcon : Image{ source: "qrc:/images/palette-connection-file.png" }
    property Component componentIcon : Image{ source: "qrc:/images/palette-connection-component.png" }
    property Component watcherIcon : Image{ source: "qrc:/images/project-file-monitor.png" }


    // Menus

    property QtObject popupMenuStyle: PopupMenuStyle{
        backgroundColor: colorScheme.background
        highlightBackgroundColor: colorScheme.middlegroundOverlay
        borderColor: colorScheme.backgroundBorder
        borderWidth: 1
        radius: 2

        textStyle: TextStyle{
            color: colorScheme.foregroundFaded
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        highlightTextStyle: TextStyle{
            color: Qt.lighter(colorScheme.foregroundFaded)
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
    }

    // Panes

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

    // Labels

    property QtObject monoTextStyle : TextStyle{
        color: colorScheme.foreground
        font : Qt.font({
            family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
            weight: Font.Normal,
            italic: false,
            pixelSize: 12
        })
    }

    property QtObject smallLabelStyle: TextStyle{
        color: colorScheme.foregroundFaded
        font : Qt.font({
            family: 'Open Sans, sans-serif',
            weight: Font.Normal,
            italic: false,
            pixelSize: 8
        })
    }

    // Forms

    property QtObject inputLabelStyle: LabelOnRectangleStyle{
        background: colorScheme.middleground
        radius: 3
        textStyle: TextStyle{
            color: colorScheme.foregroundFaded
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
    }

    property QtObject inputStyle: InputBoxStyle{
        textStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        radius: 2
        hintTextStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        backgroundColor: colorScheme.middleground
        borderColor: colorScheme.backgroundBorder
        borderThickness: 1
        textSelectionColor: colorScheme.textSelection
    }


    property QtObject monoInputStyle: InputBoxStyle{
        textStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        radius: 3
        hintTextStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        backgroundColor: colorScheme.background
        borderColor: colorScheme.backgroundBorder
        borderThickness: 1
        textSelectionColor: colorScheme.textSelection
    }


    property QtObject formButtonStyle : TextButtonStyle{
        textStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        hoverTextStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        backgroundColor: colorScheme.middlegroundOverlayDominant
        backgroundHoverColor: Qt.lighter(colorScheme.middlegroundOverlayDominant, 1.2)
        borderColor: colorScheme.middlegroundOverlayDominantBorder
        borderHoverColor: colorScheme.middlegroundOverlayDominantBorder
        borderThickness: 1
        radius: 3
    }


    property QtObject iconButtonStyle : RectangleButtonStyle{
        backgroundColor: 'transparent'
        backgroundHoverColor: 'transparent'
        borderColor: 'transparent'
        borderHoverColor: root.colorScheme.middlegroundOverlayDominantBorder
        borderThickness: 1
        radius: 3
    }

    property QtObject minimalIconButtonStyle : RectangleButtonStyle{
        backgroundColor: 'transparent'
        backgroundHoverColor: 'transparent'
        borderColor: 'transparent'
        borderHoverColor: 'transparent'
        borderThickness: 0
        radius: 0

        property color foregroundColor: '#999'
    }

    property QtObject checkBoxStyle : Input.CheckBoxStyle{
        backgroundColor: colorScheme.middlegroundOverlayDominant
        borderColor: colorScheme.middlegroundOverlayDominantBorder

        checkMark: CheckMarkIcon{
            width: 6
            height: 6
            strokeWidth: 2
        }
    }

    property QtObject numberSpinBoxStyle: Input.NumberSpinBoxStyle{
        inputBox: inputStyle
        incrementButton: Rectangle{
            signal clicked()
            color: colorScheme.middlegroundOverlayDominant
            Text{
                anchors.centerIn: parent
                color: colorScheme.foreground
                text: '+'
            }
            MouseArea{
                anchors.fill: parent
                onClicked: parent.clicked()
            }
        }

        decrementButton: Rectangle{
            signal clicked()
            color: colorScheme.middlegroundOverlayDominant
            Text{
                anchors.centerIn: parent
                color: colorScheme.foreground
                text: '-'
            }
            MouseArea{
                anchors.fill: parent
                onClicked: parent.clicked()
            }
        }
    }

    property QtObject dropDownStyle : Input.DropDownStyle{
        backgroundColor: inputStyle.backgroundColor
        backgroundColorHighlight: Qt.lighter(inputStyle.backgroundColor)
        borderColor: inputStyle.borderColor
        borderSize: 1
        radius: inputStyle.radius
        textStyle: inputStyle.textStyle
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
            onClicked: parent.clicked()
        }
        property Component cancel: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.formButtonStyle

            content: XIcon{
                anchors.centerIn: parent
                width: parent.parent.width / 3
                height: parent.parent.height / 3
                strokeWidth: 1
            }
            onClicked: parent.clicked()
        }
        property Component add: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.formButtonStyle

            content: PlusIcon{
                anchors.centerIn: parent
                width: parent.parent.width / 3
                height: parent.parent.height / 3
                strokeWidth: 1
            }
            onClicked: parent.clicked()
        }

        property Component save: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.iconButtonStyle

            content: Image{
                anchors.centerIn: parent
                source: "qrc:/images/top-icon-save.png"
            }
            onClicked: parent.clicked()
        }

        property Component connect: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.formButtonStyle

            content: Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-connections.png"
            }
            onClicked: parent.clicked()
        }

        property Component penSize: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.iconButtonStyle

            content: PenSizeIcon{
                width: 25
                height: 25
            }
            onClicked: parent.clicked()
        }

        property Component editorCode: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.minimalIconButtonStyle

            content: CodeIcon{
                width: 15
                height: 13
                color: root.minimalIconButtonStyle.foregroundColor
            }

            onClicked: parent.clicked()
        }

        property Component editorShape: RectangleButton{
            width: parent ? parent.width : 20
            height: parent ? parent.height: 20

            style: root.minimalIconButtonStyle

            content: Image{
                 source : "qrc:/images/switch-to-design.png"
             }

            onClicked: parent.clicked()
        }
    }

    // Tooltip

    property QtObject tooltip: QtObject{
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
        property color borderColor: colorScheme.middlegroundOverlayDominant
        property double borderWidth: 1
        property double opacity: 0.9
        property double radius: 5
    }

    // Lists

    property QtObject selectableListView: SelectableListViewStyle{
        labelStyle: TextStyle{
            color: colorScheme.foreground
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        backgroundColor: colorScheme.background
        selectionBackgroundColor: colorScheme.middlegroundOverlayDominant
        radius: 2
        borderColor: colorScheme.middlegroundOverlayDominant
        borderWidth: 1
        opacity: 0.95
        scrollbarColor: colorScheme.scrollbarColor
    }

    // Node Editor

    property QtObject nodeEditor : QtObject{
        property color backgroundColor: root.colorScheme.background
        property color backgroundGridColor: root.colorScheme.middleground

        property color borderColor: root.colorScheme.middlegroundOverlayDominant
        property color highlightBorderColor: root.colorScheme.middlegroundOverlayDominantBorder
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

    // Timeline

    property QtObject timelineStyle : TimelineStyle{
        rowBackground: root.colorScheme.background
        borderColor: root.colorScheme.backgroundBorder
        topHeaderBackgroundColor: root.colorScheme.middleground
        headerBorderColor: root.colorScheme.middlegroundBorder
        headerRowBackground: root.colorScheme.middleground

        inputStyle: root.inputStyle

        timeLabelStyle: TextStyle{
            color: root.colorScheme.foregroundFaded
            font.family: "Source Code Pro, Ubuntu Mono"
            font.pixelSize: 14
            font.weight: Font.Normal
        }

        segmentBackground: root.colorScheme.middlegroundOverlay
        segmentBorder : root.colorScheme.middlegroundOverlayBorder
        segmentBorderFocus: root.colorScheme.foregroundFaded
    }
}
