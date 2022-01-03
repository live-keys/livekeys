export class Test2 extends Container{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        Element.addProperty(this, 'x', {type: "int", notify: 'xChanged'})

        this.x = 20

        Element.assignDefaultProperty(this, [
            (function(parent){
                Element.addProperty(this, 'l', {type: "int", event: 'lChanged'})

                Element.assignPropertyExpression(
                    this,
                    "l",
                    function(){ return parent.x + 20 }.bind(this),
                    [[parent, "xChanged"]]
                )
                return this
            }.bind(new Element())(this))
        ])
    }
}
