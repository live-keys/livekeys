import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import live 1.0
import editor 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons
import visual.input 1.0 as Input
import editqml 1.0

CodePalette{
    id: palette

    type : "qml/lcvimgproc#ProjectionMapper"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{
        property color toolbarColor: theme.colorScheme.middleground
        property color toolbarBorder: theme.colorScheme.middlegroundBorder
        property color boxColor: theme.colorScheme.background
        property color boxBorderColor: theme.colorScheme.backgroundBorder
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property color toolIconColor: theme.colorScheme.foregroundFaded
        property color toolIconHighlightBackground: theme.colorScheme.middlegroundOverlayDominant
        property QtObject labelStyle: theme.inputLabelStyle
        property Component saveButton: theme.buttons.save
        property Component applyButton: theme.buttons.apply
        property Component cancelButton: theme.buttons.cancel
    }

    property bool isEditing: false
    property int currentIndex: -1
    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions

    function findSurfaceFragment(){
        var childFrags = editFragment.getChildFragments()
        var j = 0
        var surfaceFragment = null
        for (var i=0; i < childFrags.length; ++i)
            if (childFrags[i].location === QmlEditFragment.Object){
                if (j === currentIndex){
                    surfaceFragment = childFrags[i]
                    break
                }
                ++j
            }
        return surfaceFragment
    }

    function commitDestination(fragment, p1, p2, p3, p4){
        var value = '['
        value += 'Qt.point(' + Math.round(p1.x) + ", " + Math.round(p1.y) +'), '
        value += 'Qt.point(' + Math.round(p2.x) + ", " + Math.round(p2.y) +'), '
        value += 'Qt.point(' + Math.round(p3.x) + ", " + Math.round(p3.y) +'), '
        value += 'Qt.point(' + Math.round(p4.x) + ", " + Math.round(p4.y) +')]'
        fragment.writeProperties({"destination": {"__ref": value}})
        fragment.language.openNestedFragments(fragment, ['properties'])

        var destProp = null
        var childFragments = fragment.getChildFragments()
        for ( var i = 0; i < childFragments.length; ++i ){
            var childFragment = childFragments[i]
            if ( childFragment.identifier() === 'destination' ){
                destProp = childFragment
                break
            }
        }
        destProp.commit([p1, p2, p3, p4])
    }

    item: Item{
        id: paletteItem
        width: 600
        height: 300

        function modifyEditResult(points){
            if (!projectionMapper) return
            projectionMapper.modifyEditResult(points)
        }

        property QtObject projectionMapper: null
        onProjectionMapperChanged: {
            if ( projectionMapper ){
                imageView.image = Qt.binding(function(){
                    return palette.isEditing
                        ? projectionMapper.liveResult
                        : projectionMapper.result
                })
                imageView.autoScale()
            }
        }

        Input.SelectableListView {
            width: 250
            height: 269
            anchors.top: parent.top
            anchors.topMargin: 31


            style: theme.selectableListView
            id: listView
            model: paletteItem.projectionMapper ?  paletteItem.projectionMapper.model : []
            delegate: Item {
                id: delegateItem
                height: 33
                width: 250

                function commitInput(){
                    if (!paletteItem.projectionMapper) return
                    commitButton.visible = false
                    var surfaceFragment = findSurfaceFragment()
                    if (!surfaceFragment) return
                    surfaceFragment.writeProperties({'input': {"__ref": input.text} })
                    surfaceFragment.language.openNestedFragments(surfaceFragment, ['properties'])

                    var inputProp = null
                    var childFragments = surfaceFragment.getChildFragments()
                    for ( var i = 0; i < childFragments.length; ++i ){
                        var childFragment = childFragments[i]
                        if ( childFragment.identifier() === 'input' ){
                            inputProp = childFragment
                            break
                        }
                    }

                    try{
                        var result = inputProp.bindExpression(input.text)
                        if ( !result ){
                            lk.layers.workspace.messages.pushError("Failed to match expression: " + input.text)
                            return
                        }

                        commitButton.visible = false

                    } catch (e){
                        var error = engine.unwrapError(e)
                        lk.layers.workspace.messages.pushError(error.message, error.code)
                    }
                }

                Input.InputBox{
                    id: input
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    width: 180
                    height: 25

                    style: theme.inputStyle

                    onTextChanged: {
                       commitButton.visible = true
                    }

                    onKeyPressed: {
                        if ( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ){
                            delegateItem.commitInput()
                            event.accepted = true

                        }
                    }
                }

                Input.Button{
                    id: commitButton
                    anchors.right: parent.right
                    anchors.rightMargin: 30
                    anchors.verticalCenter: parent.verticalCenter
                    width: 25
                    height: 25
                    content: theme.buttons.connect
                    visible: false
                    onClicked: {
                        delegateItem.commitInput()
                    }
                }

                Icons.PerspectiveIcon{
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    strokeWidth: 1.2
                    color: palette.style.toolIconColor
                    width: 12
                    height: 12

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (!paletteItem.projectionMapper) return
                            var pm = paletteItem.projectionMapper
                            palette.currentIndex = index
                            var focusSurface = pm.selectFocusSurface(palette.currentIndex)
                            var surfaceFragment = palette.findSurfaceFragment()
                            if (!surfaceFragment) return
                            imageView.autoScale()

                            if (!focusSurface.destination){
                                var dims = imageView.image.dimensions()
                                palette.commitDestination(
                                    surfaceFragment,
                                    Qt.point(0,0),
                                    Qt.point(dims.width, 0),
                                    Qt.point(dims.width, dims.height),
                                    Qt.point(0, dims.height)
                                )
                            }

                            paletteItem.activateToolWithPoints(focusSurface.destination)

                            palette.isEditing = true
                            paletteItem.projectionMapper.modifyLiveResult(focusSurface.destination)

                        }
                    }
                }
            }
        }

        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 0
            width: parent.width
            height: 25
            color: palette.style.toolbarColor
            border.color: palette.style.toolbarBorder
            border.width: 1

            Item{
                id: infobar
                anchors.left: parent.left
                anchors.leftMargin: 50
                height: parent.height
                clip: true
                width: parent.width - dimensionsPanel.width - 50 - anchors.leftMargin
            }

            Item{
                height: parent.height
                width: zoomInfo.width
                anchors.right: dimensionsPanel.left
                anchors.rightMargin: 20
                Input.Label{
                    id: zoomInfo
                    textStyle: palette.style.labelStyle.textStyle
                    anchors.verticalCenter: parent.verticalCenter
                    text: Math.floor(imageView.scale * 100) + '%'
                }
            }


            Item{
                id: dimensionsPanel
                height: parent.height
                width: imageInfo.width
                anchors.right: parent.right
                anchors.rightMargin: 10
                Input.Label{
                    id: imageInfo
                    textStyle: palette.style.labelStyle.textStyle
                    anchors.verticalCenter: parent.verticalCenter
                    text: {
                        if ( !imageView.image )
                            return '-'
                        var dim = imageView.image.dimensions()
                        return dim.width + 'x' + dim.height + ', ' + imageView.image.channels() + ' Ch'
                    }
                }
            }
        }

        Cv.NavigableImageView{
            id: imageView
            anchors.top: parent.top
            anchors.topMargin: 31

            anchors.left: parent.left
            anchors.leftMargin: 300

            width: paletteItem.width - 300
            height: paletteItem.height - 31

            style: QtObject{
               property Component scrollStyle: ScrollViewStyle {
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

            onImageChanged: {
                imageView.autoScale()
            }

        }

        Cv.PerspectiveTool {
            id: perspectiveTool

            canvas: imageView
            labelInfoStyle: palette.style.labelStyle.textStyle
            applyButton: palette.style.applyButton
            cancelButton: palette.style.cancelButton

            onApply: {
                if (!paletteItem.projectionMapper) return
                var pm = paletteItem.projectionMapper
                var focusSurface = pm.focusSurface
                var surfaceFragment = palette.findSurfaceFragment()
                if (!surfaceFragment) return

                palette.commitDestination(surfaceFragment, p1, p2, p3, p4)
                pm.createResult()
                paletteItem.deactivateTool()
                palette.isEditing = false
            }
            onCancel: {
                paletteItem.projectionMapper.createResult()
                paletteItem.deactivateTool()
                palette.isEditing = false
            }
            onShapeChanged: {
                paletteItem.projectionMapper.modifyLiveResult(points)
            }

        }

        function activateToolWithPoints(points){
            perspectiveTool.infoBarContent.parent = infobar
            perspectiveTool.activate()
            perspectiveTool.assignShapePoints(points[0], points[1], points[2], points[3])

        }

        function deactivateTool(){
            perspectiveTool.infoBarContent.parent = null
            perspectiveTool.deactivate()
        }


        ResizeArea{
            minimumWidth: 400
            minimumHeight: 200
        }

    }

    onInit: {
        paletteItem.projectionMapper = value
        if (value) value.createResult()
    }
    onValueFromBindingChanged: {
        paletteItem.projectionMapper = value
        if (value) value.createResult()
    }
}
