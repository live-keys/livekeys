import QtQuick 2.3

Item{
    id: root

    width: 500
    height: 100

    property alias canvas: canvas
    property var paper: null

    function paint(){ canvas.requestPaint() }

    signal paperReady(var paper)
    signal painted()

    property string xmldomPath: ':/xmldom.js'
    property string domScriptPath: ':/paper-dom.js'
    property string paperScriptPath: ':/paper-full.js'
    property QtObject jsModuleLoader: {
        if (typeof jsModule !== 'undefined') {
            return jsModule
        }
        return null
    }

    property var dom: null
    property var loader: function(){
        if ( !jsModuleLoader )
            throw new Error("PaperCanvas: Property jsModuleLoader failed to initialize.")

        var xmldomObject = jsModuleLoader.require(xmldomPath, {
            module : { exports: {} }
        }, false)

        var xmldomExports = xmldomObject.module.exports

        var domModule = jsModuleLoader.require(domScriptPath, {
            module : { exports: {} },
            xmldom: xmldomExports
        }, false)
        var CreatePaperDom = domModule.module.exports

        root.dom = new CreatePaperDom(canvas)

        var paperScriptObject = jsModuleLoader.require(paperScriptPath, {
             module: { exports: {} },
             self: root.dom,
             setInterval: function(handleCallbacks, time){
                 canvas.handleCallbacks = handleCallbacks
                 timer.running = true
                 return true
             }
         }, true)

        var paperScope = paperScriptObject.module.exports
        return paperScope
    }

    function convertToMouseEvent(event){
        var paperEvent = new root.paper.MouseEvent()
        paperEvent.point = new root.paper.Point(event.x, event.y)
        paperEvent.modifiers.shift = event.modifiers & Qt.ShiftModifier ? true : false
        paperEvent.modifiers.control = event.modifiers & Qt.ControlModifier ? true : false
        paperEvent.modifiers.alt = event.modifiers & Qt.AltModifier ? true : false
        paperEvent.modifiers.meta = event.modifiers & Qt.MetaModifier ? true : false

        return paperEvent
    }

    Canvas{
        id: canvas
        anchors.fill: parent
        renderTarget : Canvas.FramebufferObject

        property var htmlCanvas : null
        property var handleCallbacks: function(){}

        function initialize(){
            var paperScope = root.loader()
            if ( paperScope ){
                htmlCanvas = root.dom.document.mainCanvas

                paperScope.setup(htmlCanvas)
                paperScope.view.autoUpdate = false
                canvas.onWidthChanged.connect(function(){
                    paperScope.view.size.width = canvas.width
                    paperScope.view.zoom = 1
                })
                canvas.onHeightChanged.connect(function(){
                    paperScope.view.size.height = canvas.height
                    paperScope.view.zoom = 1
                })

                root.paper = paperScope
                root.paperReady(paperScope)
            }
        }

        onPaint: {
            if ( !root.paper ){
                initialize()
            }
            root.paper.view.update()
        }
        onPainted: root.painted()
    }

    Timer{
        id: timer
        running: false
        interval: 1000 / 60
        repeat: true
        onTriggered: {
            canvas.handleCallbacks()
            canvas.requestPaint()
        }
    }
}

