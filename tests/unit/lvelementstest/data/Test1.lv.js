export class Test1 extends Element{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        Element.addProperty(this, 'y', {type: "int", notify: 'yChanged'})
        Element.addProperty(this, 't', {type: "int", notify: 'tChanged'})
        Element.addProperty(this, 'x', {type: "int", notify: 'xChanged'})

        this.y = 20
        this.t = 30

        Element.assignPropertyExpression(
            this,
            "x",
            function(){ return this.y + this.t }.bind(this),
            [
                [this, "yChanged"],
                [this, "tChanged"]
            ]
        )
    }

}

