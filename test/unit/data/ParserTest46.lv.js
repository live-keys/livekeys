import {A} from '__UNRESOLVED__'
import {B} from '__UNRESOLVED__'

export let b = (function(parent){
    this.setParent(parent)
    this.completed = function(){
        this.a = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new A())(null))
    }
    Element.complete(this)
    return this
}.bind(new B())(null))
