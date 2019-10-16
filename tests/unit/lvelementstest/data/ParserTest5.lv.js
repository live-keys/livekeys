// Note that when dealing with ids, components are only aware of their own context,
// any child or parent component context will not be available

module.exports["ParserTest5"] = class ParserTest5 extends Container{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        // Declare all parent properties
        Element.addProperty(this, "elemProp", {type: "Element", notify: "elemPropChanged"})

        // Assign parent properties
        this.elemProp = (function(parent){
            this.setParent(parent)
            // Declare all properties
            Element.addProperty(this, "x", {type: "int", notify: "xChanged"})
            // Assign properties
            this.x = 20
            return this
        }.bind(new Element())(this))

        // Assign all properties

        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', {type: "int", notify: 'yChanged'})

                this.y = 20

                return this
            }.bind(new Element())(this))
        ])
    }
}
