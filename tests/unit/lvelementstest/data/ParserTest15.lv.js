export class ParserTest15 extends Element{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        Element.addProperty(this, 'c', { type: "int", notify: "cChanged" })
        this.c = (function(){
            var newElement = (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'int', notify: 'yChanged' })
                this.y = 20
                return this
            }.bind(new Element())(null))

            return newElement.y
        })()

    }
}
