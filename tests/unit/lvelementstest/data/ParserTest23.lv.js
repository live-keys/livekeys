export class A extends Element{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
    }

    function c(){
        return (function(parent){
            this.setParent(parent)
            Element.addProperty(this, 'x', { type: 'var', notify: 'xChanged' })
            this.x = (function(){
                var d = (function(parent){
                    this.setParent(parent)
                    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                    this.y = 10
                    return this
                }.bind(new Element())(null))

                return d.y
            }())

            return this
        }.bind(new Element())(null))
    }
}
