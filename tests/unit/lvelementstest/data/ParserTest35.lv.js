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
