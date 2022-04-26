// Assign module

import {Container} from '__UNRESOLVED__'

export let ParserTest09 = (function(parent){
    this.setParent(parent)
    this.ids = {}

    Element.assignId(root, "root")
    var root = this
    this.ids['root'] = root

    var rootChild = new Element()
    this.ids['rootChild'] = rootChild
    
    Element.addProperty(root, 'x', { type: 'int', notify: 'xChanged' })
    Element.addProperty(rootChild, 'y', { type: 'int', notify: 'yChanged' })

    // assign properties
    this.x = 20

    // assign default property
    Element.assignChildrenAndComplete(this, [
        (function(parent){
            this.setParent(parent)
            Element.assignId(rootChild, "rootChild")
            this.y = 10
            Element.complete(this)
            return this
        }.bind(rootChild)(this))
    ])

    return this
}.bind(new Container())(null))
