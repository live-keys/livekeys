// Note that when dealing with ids, components are only aware of their own context,
// any child or parent component context will not be available

module.exports["Test3"] = class Test3 extends Container{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        // Create all Ids first
        this.ids = {}

        var twenty = new Element()
        this.ids["twenty"] = twenty

        // Declare all properties for ids
        Element.addProperty(twenty, "x", {type: "int", notify: "xChanged"})

        // Declare all parent properties
        Element.addProperty(this, "elemProp", {type: "Element", notify: "elemPropChanged"})

        // Assign parent properties
        this.elemProp = (function(parent){
            this.setParent(parent)
            Element.assignId(twenty, "twenty")
            this.x = 20
            return this
        }.bind(twenty)(this))

        // Assign all properties

        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', {type: "int", notify: 'yChanged'})

                Element.assignPropertyExpression(
                    this,
                    "y",
                    function(){ return twenty.x }.bind(this),
                    [[twenty, "xChanged"]]
                )

                return this
            }.bind(new Element())(this))
        ])
    }
}
