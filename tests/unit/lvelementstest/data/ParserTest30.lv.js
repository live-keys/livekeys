
export class A extends Element{

    constructor(){
        super()
        A.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.addProperty(this, 'children', { type: "default", notify: "childrenChanged" })
        this.children = []

        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new Element())(this)),
            (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new Element())(this))
        ])
    }

}
