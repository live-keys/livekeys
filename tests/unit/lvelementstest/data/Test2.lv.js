module.exports["Test2"] = class Test2 extends Container{

    constructor(){
        super()
        var that = this;

        Element.addProperty(this, 'x', {
            type: "int",
            value: 20,
            notify: 'xChanged'
        })

        var children = [];
        var child = null;

        child = new Element()
        child.setParent(this)
        Element.addProperty(child, 'l', {
            type: "int",
            value: function(){ return child.parent.x + 20 },
            bindings: [
                [child.parent, "xChanged"]
            ],
            event: 'lChanged'
        })

        children.push(child)

        Element.assignDefaultProperty(this, children)
    }
}
