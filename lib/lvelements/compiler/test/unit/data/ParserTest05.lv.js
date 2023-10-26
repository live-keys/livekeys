import {Container} from '__UNRESOLVED__'

export class ParserTest05 extends Container{

    constructor(){
        super()
        ParserTest05.prototype.__initialize.call(this)
    }

    __initialize(){
        // Declare all parent properties
        Element.addProperty(this, 'elemProp', {type: 'Element', notify: 'elemPropChanged'})

        // Assign parent properties
        this.elemProp = (function(parent){
            this.setParent(parent)
            // Declare all properties
            Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
            // Assign properties
            this.x = 20
            Element.complete(this)
            return this
        }.bind(new Element())(this))

        // Assign all properties

        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', {type: 'int', notify: 'yChanged'})

                this.y = 20
                Element.complete(this)
                return this
            }.bind(new Element())(this))
        ])
    }
}
