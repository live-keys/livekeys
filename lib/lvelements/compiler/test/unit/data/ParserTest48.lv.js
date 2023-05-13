export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
        this.ids = {}

        var y = new Element()
        this.ids["y"] = y

        Element.addProperty(y, 'd', { type: '', notify: 'dChanged'})
        Element.addProperty(y, 'e', { type: 'int', notify: 'eChanged'})
        Element.addProperty(y, 'f', { type: '', notify: 'fChanged'})
        Element.addProperty(y, 'g', { type: 'int', notify: 'gChanged'})

        Element.addProperty(this, 'x', { type: '', notify: 'xChanged' })
        Element.addProperty(this, 'y', { type: 'int', notify: 'yChanged' })
        Element.addProperty(this, 'z', { type: 'Array<int>', notify: 'zChanged' })
        Element.addProperty(this, 'a', { type: '', notify: 'aChanged' })
        Element.addProperty(this, 'b', { type: 'int', notify: 'bChanged' })
        Element.addProperty(this, 'c', { type: 'Array<int>', notify: 'cChanged' })
        this.x = 100
        this.y = 150
        Element.assignPropertyExpression(this,
            'z',
            function(){ return this.y}.bind(this),
            [[this,'y']]
        )
        this.a = 100
        this.b = 150
        this.c = this.b

        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignId(y, "y")
                this.d = 100
                this.e = 100
                this.f = this.e
                this.g = this.e
                Element.complete(this)
                return this
            }.bind(y)(this))
        ])
    }
}
