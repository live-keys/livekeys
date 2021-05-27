
module.exports["A"] = class A extends Element{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        Element.addProperty(this, 'x', { type: "var", notify: "xChanged" })
        Element.addProperty(this, 'y', { type: "bool", notify: "yChanged" })
        Element.assignPropertyExpression(this,
            'x',
            function(){ return this.y ? (function(x){
                    return x + 20
                })(100) : 15
            }.bind(this),
            [
                [ this, 'yChanged' ]
            ]
        )
        this.y = true
    }

}