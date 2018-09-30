module.exports[__NAME__] = class extends Element{

    constructor(){
        super()
        var that = this;

        Element.addProperty(this, 'y', {
            type: "int",
            value: 20,
            notify: 'yChanged'
        })
        Element.addProperty(this, 't', {
            type: "int",
            value: 30,
            notify: 'tChanged'
        })
        Element.addProperty(this, 'x', {
            type: "int",
            value: function(){ return that.y + that.t },
            bindings: [
                [this, "yChanged"],
                [this, "tChanged"]
            ],
            notify: 'xChanged'
        })
    }

}

