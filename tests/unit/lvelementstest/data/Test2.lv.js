module.exports[__NAME__] = class extends Container{

    constructor(){
        super()
        var that = this;

        Element.addProperty(this, 'x', {
            type: "int",
            value: 20,
            notify: 'xChanged'
        })

        var child0 = new Element()
        child0.setParent(this)
        Element.addProperty(child0, 'l', {
            type: "int",
            value: function(){ return child0.parent.x + 20 },
            bindings: [
                [child0.parent, "xChanged"]
            ],
            event: 'lChanged'
        })

        Element.assignDefaultProperty(this, [
            child0
        ])
    }
}
