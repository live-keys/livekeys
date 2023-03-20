export class A extends Element{

    constructor(){
        super()
        A.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.addProperty(this, 'x', {
            type: 'int',
            notify: 'xChanged' ,
            get: function(){ return Element.getProperty('x') },
            set: function(val){ Element.setProperty('x', val) }
        })
        Element.addProperty(this, 'z', {
            type: 'int',
            notify: 'zChanged' ,
            get: function(){
                return (function(parent){
                    this.setParent(parent)
                    Element.complete(this)
                    return this
                }.bind(new Element())(null))
            },
            set: function(val){
                (function(parent){
                    this.setParent(parent)
                        Element.addProperty(this, 'x', { type: 'int', notify: 'xChanged' })
                    this.x = val
                    Element.complete(this)
                    return this
                }.bind(new Element())(null))
            }
        })
        this.x = 20
        this.z = 30
    }

    get y(){ return this._y }
    set y(val){ this._y = val }
}
