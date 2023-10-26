export class A extends Element{

    constructor(){
        super()
        A.prototype.__initialize.call(this)
    }

    __initialize(){
    }

    c(){
        return (function(parent){
            this.setParent(parent)
            Element.addProperty(this, 'x', { type: 'var', notify: 'xChanged' })
            this.x = (function(){
                var d = (function(parent){
                    this.setParent(parent)
                    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                    this.y = 10
                    Element.complete(this)
                    return this
                }.bind(new Element())(null))

                return d.y
            }.bind(this)())

            Element.complete(this)
            return this
        }.bind(new Element())(null))
    }
}
