import {Test} from '__UNRESOLVED__'

export class B extends Element{

    constructor(){
        super()
        this.__initialize()
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
                    Element.assignDefaultProperty(this, [
                        (function(parent){
                            this.setParent(parent)
                            Element.assignId(nested, "nested")
                            return this
                        }.bind(nested)(this))
                    ])
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
            }.bind(test)(this)),
            
            (function(parent){
                this.setParent(parent)
                Element.assignId(extra, "extra")
                return this
            }.bind(extra)(this))
        ])
    }

}
