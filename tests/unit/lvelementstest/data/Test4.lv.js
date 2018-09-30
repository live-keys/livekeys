module.exports[__NAME__] = class extends Container{

    constructor(){
        super()
        var that = this;
        this.someVar = 20

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
            that.x = 100;
        });

        var child0 = new Element()
        Element.addProperty(child0, 'k', {
            type: "int",
            value: 20,
            notify: "kChanged"
        })

        var child1 = new Element()
        Element.addProperty(child1, 'message', {
            type: "string",
            value: "thirty",
            notify: "messageChanged"
        })

        Element.assignDefaultProperty(this, [
            child0,
            child1
        ])
    }

}




