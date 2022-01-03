
export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
    }

    static prop1 = 100
    static prop2 = { x: 1, y: 2}
    static prop3 = (function(){ return 100 })()
    static prop4
}
