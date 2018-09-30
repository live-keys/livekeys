// Note that when dealing with ids, components are only aware of their own context,
// any child or parent component context will not be available

module.exports[__NAME__] = class extends Container{

    constructor(){
        super()
        var that = this
        this.ids = {}

        var elemProp = new Element()
        this.ids["twenty"] = elemProp
        elemProp.setParent(this)

        Element.assignId(elemProp, "twenty")
        Element.addProperty(elemProp, 'x', {
            type: "int",
            value: 20,
            notify: 'xChanged'
        })

        Element.addProperty(this, "elemProp", {
            type: "Element",
            value: elemProp,
            notify: "elemPropChanged"
        })

        var child0 = new Element();
        Element.addProperty(child0, 'y', {
            type: "int",
            value: function(){ return that.ids["twenty"].x },
            notify: 'yChanged',
            bindings: [[that.ids["twenty"], "xChanged"]]
        })

        Element.assignDefaultProperty(this, [child0])
    }
}
