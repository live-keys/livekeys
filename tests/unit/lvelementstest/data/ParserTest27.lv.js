import {A as __mod__A} from '__UNRESOLVED__'
import {B as __mod__B} from '__UNRESOLVED__'
import {C as __mod__C} from '__UNRESOLVED__'
import {D as __mod__D} from '__UNRESOLVED__'
import {E as __mod__E} from '__UNRESOLVED__'
let mod = {A: __mod__A, B: __mod__B, C:__mod__C, D: __mod__D, E: __mod__E}

export class ParserTest27 extends mod.A{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        Element.addProperty(this, 'b', { type: "string", notify: "bChanged" })
        Element.addProperty(this, 'c', { type: "string", notify: "cChanged" })
        Element.assignPropertyExpression(
            this,
            'b',
            function(){ return mod.B.value}.bind(this),
            [[ mod.B, 'valueChanged' ]]
        )
        Element.assignPropertyExpression(
            this,
            'c',
            function(){ return mod.C.value}.bind(this),
            [[ mod.C, 'valueChanged' ]]
        )
    }

    toString(){
        var e = mod.E.value
        return mod.D.value + e
    }
}
