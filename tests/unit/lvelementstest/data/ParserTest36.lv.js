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

}
