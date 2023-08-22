
export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
    }

    static test1(x){
        return 100
    }
    static async test2(y){
        return 200
    }
    static test3(){
        return 100
    }
    test4(x){
        return 200
    }
    async test5(){
        return 2
    }
}

export let x = (function(parent){
    this.setParent(parent)
    this.test2 = async function(y){
        return 200
    }
    this.test3 = function(){
        return 100
    }
    Element.complete(this)
    return this
}.bind(new X())(null))

