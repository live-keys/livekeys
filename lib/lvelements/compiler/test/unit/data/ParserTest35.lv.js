
export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
    }
}


X.prop1 = 100
X.prop2 = { x: 1, y: 2}
X.prop3 = (function(){ return 100 })()
X.prop4

X.keys = {
    'F' : (function(parent){
        this.setParent(parent)
        Element.complete(this)
        return this
    }.bind(new Element())(null))
}
