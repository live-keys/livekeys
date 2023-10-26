import {Container} from '__UNRESOLVED__'

export let app = (function(parent){
    this.setParent(parent)
    this.ids = {}

    Element.assignId(app, "app")
    var app = this
    this.ids["app"] = app

    var elemX = new Element()
    this.ids["elemX"] = elemX

    var elemChild = new Element()
    this.ids["elemChild"] = elemChild

    Element.addProperty(app, 'x', { type: 'var', notify: 'xChanged' })

    this.x = (function(parent){
        this.setParent(parent)
        Element.assignId(elemX, "elemX")
        Element.complete(this)
        return this
    }.bind(elemX)(this))

    Element.assignChildrenAndComplete(this, [
        (function(parent){
            this.setParent(parent)
            Element.assignId(elemChild, "elemChild")
            Element.complete(this)
            return this
        }.bind(elemChild)(this))
    ])
    return this
}.bind(new Container())(null))
