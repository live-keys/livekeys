import {A} from './a.js'
import {B,C} from './b.js'
import D from './d.js'
import E, F from './e.js'

export class X extends A{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
        Element.addProperty(this, 'x', { type: 'var', notify: 'xChanged' })
        Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
        Element.addProperty(this, 'z', { type: 'var', notify: 'zChanged'})
        Element.addProperty(this, 't', { type: 'var', notify: 'tChanged'})
        Element.addProperty(this, 'w', { type: 'var', notify: 'wChanged'})
        this.x = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new B())(this))

        this.y = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new C())(this))

        this.z = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new D())(this))

        this.t = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new E())(this))

        this.w = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new F())(this))

    }

}
