// Note that when dealing with ids, components are only aware of their own context,
// any child or parent component context will not be available

module.exports["Test3"] = class Test3 extends Container{

    constructor(){
        super()
        this.ids = {}

        var twenty = new Element()
        twenty.setParent(this)
        Element.assignId(twenty, "twenty")
        Element.addProperty(twenty, 'x', {
            type: "int",
            value: 20,
            notify: 'xChanged'
        })

        this.ids["twenty"] = twenty

        Element.addProperty(this, "elemProp", {
            type: "Element",
            value: twenty,
            notify: "elemPropChanged"
        })

        var children = []
        var child = null

        child = new Element();
        Element.addProperty(child, 'y', {
            type: "int",
            value: function(){ return twenty.x }.bind(this),
            notify: 'yChanged',
            bindings: [[twenty, "xChanged"]]
        })
        children.push(child)

        Element.assignDefaultProperty(this, children)
    }
}
