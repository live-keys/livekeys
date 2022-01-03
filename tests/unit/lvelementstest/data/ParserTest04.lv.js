import {not1} from '__UNRESOLVED__'
import {not2} from '__UNRESOLVED__'

export class B extends Element{
    constructor(){
        super()
        B.prototype.__initialize.call(this)
    }

    __initialize(){
        this.ids = {}

        var C = this
        this.ids["C"] = C

        Element.addProperty(this, 'not2', {type: "int", notify: 'not2Changed'})
        this.not2 = 10
    }

    not1(d, e){
        var f = 5

        class G{}
        function H(){}

        console.log(A, B, C, d, e, f, G, not1, not2)
    }
}
