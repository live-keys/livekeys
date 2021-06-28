import QtQuick 2.3
import QtWebEngine 1.7
import editor 1.0
import fs 1.0 as Fs

WebEngineView{
    id: root
    width: 100
    height: 100
    anchors.fill: parent ? parent : undefined

    function loadDocumentationHtml(fileUrl){
        root.url = fileUrl
        root.loadedHovers = null
    }

    property string styleSheet : __defaultStyleSheet
    property string __defaultStyleSheet:
        'body{ background-color: #03090d; } ' +
        '#main{ padding: 0px; } ' +
        '#wrapper{ padding: 15px; max-width: 100%;flex: 0 0 100%;background-color: #03090d;} ' +
        '#indexList{ display: none;} '

    property string styleSourceCode :
        "\nvar node = document.createElement('style'); " +
        "node.innerHTML = \"" + root.styleSheet + "\";" +
        "document.getElementsByTagName('head')[0].appendChild(node); " +
        "var hash = location.hash.substr(1).trim();" +
        "if (hash){ document.getElementById(hash).scrollIntoView(); }\n"

    property string hoverScanCode :
        'var hovers = document.getElementsByClassName("livekeys-hover");' +
        'var inits  = document.getElementsByClassName("livekeys-init");' +
        'var result = { hovers: {}, inits: [] }; ' +
        'for (var i = 0; i < hovers.length; ++i){' +
            'var hover = hovers[i];' +
            'var hoverId = i;' +
            'result.hovers[hoverId] = [hover.getAttribute("enter"), hover.getAttribute("leave")];' +
            'var hoverData = hover.innerHTML;' +
            'hover.innerHTML = "<a href=\\\"livekeys-hover://id/" + hoverId + "\\\">" + hoverData + "</a>";' +
        '}\n' +
        'for (var i = 0; i < inits.length; ++i){' +
            'var init = inits[i];' +
            'result.inits.push(init.getAttribute("src"));' +
        '}\n' +
        'result;'

    property string activeHover: ''
    property var loadedHovers : { return {} }
    property var hoverState : { return {} }

    property string prevUrl: ''
    signal runJavaScriptCallbackError(var e)
    property WebEngineScript injectStyleScript : WebEngineScript{
        name: "injectStyleScript"
        sourceCode: styleSourceCode
    }

    userScripts: [injectStyleScript]
    onJavaScriptConsoleMessage: {
        if ( level === WebEngineView.ErrorMessageLevel ){
            console.error("WebEngine Error: " + message + ' ' + sourceID)
        } else {
            console.log("WebEngine: " + message)
        }
    }

    onLinkHovered: {
        var scheme = Fs.UrlInfo.scheme(hoveredUrl)
        if ( scheme === 'livekeys-hover' ){
            var path = Fs.UrlInfo.path(hoveredUrl)
            path = path.length > 0 ? path.substring(1) : path
            if ( path.length > 0 && root.loadedHovers !== null && root.loadedHovers.hasOwnProperty(path) ){
                var enterFunction = root.loadedHovers[path][0][0].run
                activeHover = path
                enterFunction(lk.layers.workspace, hoverState, root.loadedHovers[path][0][1])
            }

        } else if ( hoveredUrl.toString() === '' && activeHover.length && root.loadedHovers && root.loadedHovers.hasOwnProperty(root.activeHover) ){
            var leaveFunction = root.loadedHovers[activeHover][1][0].run
            leaveFunction(lk.layers.workspace, hoverState, root.loadedHovers[activeHover][1][1])
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

                for (var key in result.hovers) {
                    var enterPath = Fs.Path.join(path, Fs.UrlInfo.path(result.hovers[key][0]))
                    var enterPathHash = Fs.UrlInfo.hasFragment(result.hovers[key][0]) ? Fs.UrlInfo.fragment(result.hovers[key][0]) : ''
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

                    var leavePath = Fs.Path.join(path, Fs.UrlInfo.path(result.hovers[key][1]))
                    var leavePathHash = Fs.UrlInfo.hasFragment(result.hovers[key][0]) ? Fs.UrlInfo.fragment(result.hovers[key][0]) : ''
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

                    root.loadedHovers[key] = [[enterControl, enterPathHash], [leaveControl, leavePathHash]]
                }

                for (var index = 0; index < result.inits.length; ++index){
                    var currentInitializerPath = Fs.Path.join(path, Fs.UrlInfo.path(result.inits[index]))
                    currentInitializerPath = "file:///" + currentInitializerPath


                    var component = Qt.createComponent(currentInitializerPath);
                    if (component.status === Component.Ready){
                        var initializerControl = component.createObject(root);
                        if ( !initializerControl || typeof initializerControl.run !== "function" ){
                            lk.engine.throwError(new Error("Error creating object from component: " + currentInitializerPath), root)
                            return
                        }
                        try {
                            initializerControl.run(lk.layers.workspace, project)
                        } catch (e){
                            root.runJavaScriptCallbackError(e)
                        }
                    } else if (component.status === Component.Error) {
                        lk.engine.throwError(new Error("Error loading component: " + component.errorString()), root)
                        return
                    }
                }
            });
        }
    }

    onRunJavaScriptCallbackError: {
        lk.engine.throwError(e, root)
    }

    onNewViewRequested: {
        if ( request.userInitiated && request.destination === WebEngineView.NewViewInTab){

            var scheme = Fs.UrlInfo.scheme(request.requestedUrl)
            if ( scheme === 'livekeys' ){

                if ( Fs.UrlInfo.host(request.requestedUrl) === "open" ){
                    var path = Fs.UrlInfo.path(request.requestedUrl)
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
                } else if ( Fs.UrlInfo.host(request.requestedUrl) === "open-project" ){
                    var path = Fs.UrlInfo.path(request.requestedUrl)
                    path = Fs.Path.join(lk.layers.workspace.pluginsPath(), path)

                    lk.layers.workspace.project.openProjectPath(path)
                }
            }
        }
    }

    onNavigationRequested: {
        if ( request.navigationType === WebEngineNavigationRequest.LinkClickedNavigation){

            var url = request.url

            var scheme = Fs.UrlInfo.scheme(request.url)
            if ( scheme === 'livekeys' ){
                prevUrl = root.url
                request.action = WebEngineNavigationRequest.IgnoreRequest
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
