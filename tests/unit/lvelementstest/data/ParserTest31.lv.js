export class A extends Element{

    constructor(){
        super()
        A.prototype.__initialize.call(this)
    }
    __initialize(){
        Element.addProperty(this, 'a', { type: "var", notify: "aChanged" })
        this.a = function(x){
            return x + 1
        }.bind(this)
    }

}
