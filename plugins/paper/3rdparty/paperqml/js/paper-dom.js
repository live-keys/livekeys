
var createPaperDom = function(canvas){
    var paperDom = {
        navigator: {
            userAgent: "Custom"
        },
        document: {
            addEventListener: function(name, func, options){},
            createElement: function(arg){
                return paperDom.document.mainCanvas
            },
            mainCanvas: null,
            body : {},
            defaultView: {
                pageXOffset: 0,
                pageYOffset: 0
            },
            documentElement : {
                clientWidth: canvas.width,
                clientHeight: canvas.height
            },
            getElementsByTagName: function(arg){
                return []
            }
        },
        DOMParser : xmldom.DOMParser,
        DOMHandler : xmldom.DOMHandler,
        DOMHandlerInstance : null,
        window: {
            navigator: {
                pointerEnabled: true
            },
            HTMLCanvasElement: canvas.constructor,
            devicePixelRatio: {},
            location: {
                href: 'local'
            },

            addEventListener: function(name, func, options){},
        }
    }

    var HtmlCanvasElement = function(document){
        this.style = {}
        this.attributes = {}
        this.ownerDocument = document
        this.__canvas = canvas

        var ignore = function(){}

        Object.defineProperty(this, "width", {
            get(){ return this.__canvas.width },
            set(newValue){ ignore(newValue) }
        })

        Object.defineProperty(this, "height", {
            get(){ return this.__canvas.height },
            set(newValue){ ignore(newValue) }
        })

        this.getBoundingClientRect = function(){
            var width = this.__canvas.width
            var height = this.__canvas.height
            return {x: 0, y: 0, top: 0, left: 0, bottom: height, right: width, width: width, height: height}
        }

        this.hasAttribute = function(name){
            return name in this.attributes
        }

        this.getAttribute = function(name){
            if ( name in this.attributes){
                return this.atrributes[name]
            }
            return null
        }

        this.setAttribute = function(name, value){
            this.attributes[name] = value
        }

        this.getContext = function(arg){
            var ctx = this.__canvas.getContext(arg)
            Object.defineProperty(ctx, 'canvas', {
              value: this,
              writable: false
            })
            return ctx
        }

        this.addEventListener = function(name, func, options){}
    }

    paperDom.document.mainCanvas = new HtmlCanvasElement(paperDom.document)


    if ( paperDom.DOMHandler ){
        var dh = new xmldom.DOMHandler()
        dh.startDocument()
        dh.endDocument()
        paperDom.DOMHandlerInstance = dh
        paperDom.document.createElementNS = paperDom.DOMHandlerInstance.doc.createElementNS
        paperDom.document.createAttribute = paperDom.DOMHandlerInstance.doc.createAttribute
    }

    return paperDom
}

module.exports = createPaperDom
