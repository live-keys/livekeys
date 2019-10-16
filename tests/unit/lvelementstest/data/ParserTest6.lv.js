// Note that when dealing with ids, components are only aware of their own context,
// any child or parent component context will not be available

module.exports["ParserTest6"] = class ParserTest6 extends Container{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        // Declare all parent properties
        Element.addProperty(this, "elemProp", {type: "component", notify: "elemPropChanged"})

        // Assign parent properties
        this.elemProp = class T extends Element{

            constructor(){
                super()
                this.__initialize()
            }

            __initialize(){
                this.ids = {}

                this.ids['twenty'] = this
                var twenty = this

                Element.addProperty(this, "x", {type: "int", notify: "xChanged"})
                this.x = 20
            }
        }

        // Assign all properties

        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', {type: "Element", notify: 'yChanged'})

                Element.assignPropertyExpression(
                    this,
                    "y",
                    function(){ return new parent.ElemProp() }.bind(this),
                    [[parent, "ElemPropChanged"]]
                )

                return this
            }.bind(new Element())(this))
        ])
    }
}
