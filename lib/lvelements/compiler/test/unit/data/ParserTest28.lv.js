import {A as __mod__A} from '__UNRESOLVED__'
import {B as __mod__B} from '__UNRESOLVED__'
import {C as __mod__C} from '__UNRESOLVED__'
import {D as __mod__D} from '__UNRESOLVED__'
let mod = {A:__mod__A, B:__mod__B, C:__mod__C, D:__mod__D}
import {X as __mod2__X} from '__UNRESOLVED__'
import {Y as __mod2__Y} from '__UNRESOLVED__'
let mod2 = {X:__mod2__X, Y:__mod2__Y}

export let ParserTest28 = (function(parent){
    this.setParent(parent)
    Element.addProperty(this, 'b', { type: 'string', notify: 'bChanged' })
    Element.addProperty(this, 'c', { type: 'string', notify: 'cChanged' })
    Element.addProperty(this, 'd', { type: 'string', notify: 'dChanged' })
    Element.addProperty(this, 't', { type: 'Element', notify: 'tChanged' })
    Element.assignPropertyExpression(this,
        'b',
        function(){ return mod.B.value}.bind(this),
        [[mod,['B','value']]]
    )
    Element.assignPropertyExpression(this,
        'c',
        function(){ return mod.C.value}.bind(this),
        [[mod,['C','value']]]
    )
    Element.assignPropertyExpression(this,
        'd',
        function(){ return mod.D.value + mod2.Y.value}.bind(this),
        [[mod,['D','value']],[mod2,['Y','value']]]
    )
    this.t = (function(parent){
        this.setParent(parent)
        Element.addProperty(this, 'a', { type: 'int', notify: 'aChanged' })
        Element.assignPropertyExpression(this,
            'a',
            function(){ return mod2.Y.value}.bind(this),
            [[mod2,['Y','value']]]
        )
        Element.complete(this)
        return this
    }.bind(new mod2.X())(this))

    Element.complete(this)
    return this
}.bind(new mod.A())(null))
