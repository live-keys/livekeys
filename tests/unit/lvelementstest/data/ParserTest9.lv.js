// Create all Ids first
var root = new Container()
var rootChild = new Element()

// Declare all properties for ids
Element.addProperty(root, "x", {type: "int", notify: "xChanged"})

Element.addProperty(rootChild, "y", {type: "int", notify: "yChanged"})

// Assign module
module.exports["ParserTest9"] = (function(parent){
    this.setParent(parent)

    // assign properties
    this.x = 20

    // assign default property
    Element.assignDefaultProperty(this, [
        (function(parent){
            this.setParent(parent)
            this.y = 10
            return this
        }.bind(rootChild)(this))
    ])

    return this
}.bind(root)(null))
