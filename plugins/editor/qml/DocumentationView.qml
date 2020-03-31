import QtQuick 2.3
import QtWebEngine 1.7
import editor 1.0
import fs 1.0 as Fs

WebEngineView{
    id: root
    anchors.fill: parent ? parent : undefined

    function loadDocumentationHtml(filePath){
        root.url = "file:///" + filePath
        root.loadedHovers = null
    }

    property string styleSheet :
        '#main{ padding: 0px; } ' +
        '#wrapper{ padding: 15px;max-width: 100%;flex: 0 0 100%;background-color: #03090d;} ' +
        '#indexList{ display: none;} '

    property string styleSourceCode :
        "var node = document.createElement('style'); " +
        "node.innerHTML = \"" + root.styleSheet + "\";" +
        "document.body.appendChild(node);"

    property string hoverScanCode :
        'var hovers = document.getElementsByClassName("livekeys-hover");' +
        'var result = {}; ' +
        'for (var i = 0; i < hovers.length; ++i){' +
            'var hover = hovers[i];' +
            'var hoverId = i;' +
            'result[hoverId] = [hover.getAttribute("enter"), hover.getAttribute("leave")];' +
            'var hoverData = hover.innerHTML;' +
            'hover.innerHTML = "<a href=\\\"livekeys-hover://id/" + hoverId + "\\\">" + hoverData + "</a>";' +
        '}' +
        'result;'

    property string activeHover: ''
    property var loadedHovers : { return {} }
    property var hoverState : { return {} }

    property string prevUrl: ''

    property WebEngineScript injectStyleScript : WebEngineScript{
        name: "injectStyleScript"
        sourceCode: styleSourceCode
    }

    userScripts: [injectStyleScript]
    onJavaScriptConsoleMessage: {
        console.log("WebEngine: " + message)
    }

    onLinkHovered: {
        var scheme = Fs.UrlInfo.scheme(hoveredUrl)
        if ( scheme === 'livekeys-hover' ){
            var path = Fs.UrlInfo.path(hoveredUrl)
            path = path.length > 0 ? path.substring(1) : path
            if ( path.length > 0 && root.loadedHovers !== null && root.loadedHovers.hasOwnProperty(path) ){
                var enterFunction = root.loadedHovers[path][0].run
                activeHover = path
                enterFunction(lk.layers.workspace, hoverState)
            }

        } else if ( hoveredUrl.toString() === '' && activeHover.length && root.loadedHovers && root.loadedHovers.hasOwnProperty(root.activeHover) ){
            var leaveFunction = root.loadedHovers[activeHover][1].run
            leaveFunction(lk.layers.workspace, hoverState)
            activeHover = ''
        }
    }
    onLoadingChanged: {
        // wrapper around a bug opening chromeerror://chromedata even
        // though the opening of an undefined link was ignored below
        if ( loadRequest.errorCode === 420 ){
            root.url = root.prevUrl
        } else if ( loadRequest.status === WebEngineLoadRequest.LoadSucceededStatus ){
            root.runJavaScript(hoverScanCode, function(result) {

                var path = Fs.Path.dir(Fs.UrlInfo.toLocalFile(root.url))

                root.loadedHovers = {}

                for (var key in result) {
                    var enterPath = Fs.Path.join(path, Fs.UrlInfo.path(result[key][0]))
                    enterPath = "file:///" + enterPath
                    var enterControl = null

                    var component = Qt.createComponent(enterPath);
                    if (component.status === Component.Ready){
                        enterControl = component.createObject(root);
                        if ( !enterControl || typeof enterControl.run !== "function" ){
                            root.loadedHovers = {}
                            lk.engine.throwError(new Error("Error creating object from component: " + enterPath), root)
                            return
                        }
                    } else if (component.status === Component.Error) {
                        root.loadedHovers = {}
                        lk.engine.throwError(new Error("Error loading component: " + component.errorString()), root)
                        return
                    }

                    var leavePath = Fs.Path.join(path, Fs.UrlInfo.path(result[key][1]))
                    leavePath = "file:///" + leavePath
                    var leaveControl = null

                    component = Qt.createComponent(leavePath);
                    if (component.status === Component.Ready){
                        leaveControl = component.createObject(root);
                        if ( !leaveControl || typeof leaveControl.run !== "function" ){
                            root.loadedHovers = {}
                            lk.engine.throwError(new Error("Error creating object from component: " + leavePath), root)
                            return
                        }
                    } else if (component.status === Component.Error) {
                        root.loadedHovers = {}
                        lk.engine.throwError(new Error("Error loading component: " + component.errorString()), root)
                        return
                    }

                    root.loadedHovers[key] = [enterControl, leaveControl]
                }
            });
        }
    }
    onNavigationRequested: {
        if ( request.navigationType === WebEngineNavigationRequest.LinkClickedNavigation){
            request.action = WebEngineNavigationRequest.IgnoreRequest
            prevUrl = root.url

            var url = request.url

            var scheme = Fs.UrlInfo.scheme(request.url)
            if ( scheme === 'livekeys' ){
                request.action = WebEngineNavigationRequest.IgnoreRequest

                if ( Fs.UrlInfo.host(url) === "open" ){
                    var path = Fs.UrlInfo.path(request.url)
                    path = Fs.Path.join(Fs.Path.dir(Fs.UrlInfo.toLocalFile(root.url)), path)

                    path = "file:///" + path
                    var component = Qt.createComponent(path);
                    if (component.status === Component.Ready){
                        var control = component.createObject(root);

                        if (typeof control.run === "function") {
                            control.run(lk.layers.workspace, project)
                        }

                        control.destroy()
                    } else if (component.status === Component.Error) {
                        lk.engine.throwError(new Error("Error loading component: " + component.errorString()), root)
                    }
                }
            }
        }
    }
}
