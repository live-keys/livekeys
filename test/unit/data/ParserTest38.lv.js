import {T} from '__UNRESOLVED__'

export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new T("        function(){\n        }"))(this)),
            (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new T("        First empty line\n\n        Second empty line\n"))(this))
        ])
    }

}
