
module.exports["A"] = class A extends Element{

    constructor(){
        super()
        this.__initialize()
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
                return this
            }.bind(new Element())(null))

            return d.y
        })()

    }

}