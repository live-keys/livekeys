import {Test} from '__UNRESOLVED__'

export class C extends Element{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}

        var test = new Test()
        this.ids["test"] = test

        Element.addProperty(test, 'x', { type: "var", notify: "xChanged" })
        Element.addProperty(this, 'x', { type: "var", notify: "xChanged" })
        this.x = (function(){
            var d = (function(parent){
                this.setParent(parent)
                Element.assignId(test, "test")
                var test = this

                Element.addProperty(test, 'y', { type: 'int', notify: 'yChanged' })
                this.y = 10
                return this
            }.bind(new Element())(null))

            return d.y
        })()

        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignId(test, "test")
                this.x = 10
                return this
            }.bind(test)(this))
        ])
    }

}
