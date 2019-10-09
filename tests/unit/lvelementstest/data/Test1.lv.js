module.exports["Test1"] = class Test1 extends Element{

    constructor(){
        super()

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
            value: function(){ return this.y + this.t }.bind(this),
            bindings: [
                [this, "yChanged"],
                [this, "tChanged"]
            ],
            notify: 'xChanged'
        })
    }

}

