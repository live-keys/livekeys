import {A} from './a.js'
import {B,C} from './b.js'

export class X extends A{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
        Element.addProperty(this, 'x', { type: 'var', notify: 'xChanged' })
        Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
        this.x =         (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new B())(this))

        this.y =         (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new C())(this))

    }

}
