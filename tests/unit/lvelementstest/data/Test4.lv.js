module.exports["Test4"] = class Test4 extends Container{

    constructor(){
        super()

        Element.addProperty(this, 'x', {
            type: "int",
            value: 20,
            notify: "xChanged"
        })
        Element.addProperty(this, 'y', {
            type: "int",
            value: 20,
            notify: "yChanged"
        })
        Element.addEvent(this, 'dataChanged', [])
        this.on("dataChanged", function(){
            this.x = 100;
        }.bind(this));

        var children = []
        var child = null

        child = new Element()
        Element.addProperty(child, 'k', {
            type: "int",
            value: 20,
            notify: "kChanged"
        })

        children.push(child)

        child = new Element()
        Element.addProperty(child, 'message', {
            type: "string",
            value: "thirty",
            notify: "messageChanged"
        })
        children.push(child)

        Element.assignDefaultProperty(this, children)

        this.someVar = 20
    }

}




