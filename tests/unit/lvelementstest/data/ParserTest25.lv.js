import {Test} from '__UNRESOLVED__'

export class B extends Element{

    constructor(){
        super()
        B.prototype.__initialize.call(this)
    }

    __initialize(){
            this.ids = {}

            var test = new Test()
            this.ids["test"] = test

            var extra = new Element()
            this.ids["extra"] = extra

            Element.addProperty(test, 'x', { type: "var", notify: "xChanged" })
            Element.addProperty(this, 'x', { type: "var", notify: "xChanged" })
            this.x = (function(){
                var d = (function(parent){
                    this.setParent(parent)
                    Element.assignId(did, "did")
                    var did = this
                    var nested = new Element()
                    
                    Element.addProperty(did, 'y', { type: 'int', notify: 'yChanged' })
                    this.y = 10
                    Element.assignChildrenAndComplete(this, [
                        (function(parent){
                            this.setParent(parent)
                            Element.assignId(nested, "nested")
                            Element.complete(this)
                            return this
                        }.bind(nested)(this))
                    ])
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
            }.bind(test)(this)),
            
            (function(parent){
                this.setParent(parent)
                Element.assignId(extra, "extra")
                Element.complete(this)
                return this
            }.bind(extra)(this))
        ])
    }

}
