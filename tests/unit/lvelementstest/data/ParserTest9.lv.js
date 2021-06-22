// Assign module
module.exports["ParserTest9"] = (function(parent){
    this.setParent(parent)
    Element.assignId(root, "root")

    var root = this
    var rootChild = new Element()
    
    Element.addProperty(root, 'x', { type: 'int', notify: 'xChanged' })
    Element.addProperty(rootChild, 'y', { type: 'int', notify: 'yChanged' })

    // assign properties
    this.x = 20

    // assign default property
    Element.assignDefaultProperty(this, [
        (function(parent){
            this.setParent(parent)
            Element.assignId(rootChild, "rootChild")
            this.y = 10
            // Element.assignDefaultProperty(null)
            return this
        }.bind(rootChild)(this))
    ])

    return this
}.bind(new Container())(null))
