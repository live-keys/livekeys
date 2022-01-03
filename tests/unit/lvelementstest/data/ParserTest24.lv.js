
export class A extends Element{

    constructor(){
        super()
        A.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.addProperty(this, 'x', { type: "var", notify: "xChanged" })
        this.x = (function(){
            var d = (function(parent){
                this.setParent(parent)
                Element.assignId(did, "did")
                var did = this

                Element.addProperty(did, 'y', { type: 'int', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
            }.bind(new Element())(null))

            return d.y
        }.bind(this))()

    }

}
