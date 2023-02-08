import {Test} from '__UNRESOLVED__'

export class C extends Element{

    constructor(){
        super()
        C.prototype.__initialize.call(this)
    }

    __initialize(){
        this.ids = {}

        var test = new Test()
        this.ids["test"] = test

        Element.addProperty(test, 'x', { type: 'var', notify: 'xChanged' })
        Element.addProperty(this, 'x', { type: 'var', notify: 'xChanged' })
        this.x = (function(){
            var d = (function(parent){
                this.setParent(parent)
                this.ids = {}

                Element.assignId(test, "test")
                var test = this
                this.ids["test"] = test

                Element.addProperty(test, 'y', { type: 'int', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
            }.bind(new Element())(null))

            return d.y
        }.bind(this))()

        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignId(test, "test")
                this.x = 10
                Element.complete(this)
                return this
            }.bind(test)(this))
        ])
    }

}
