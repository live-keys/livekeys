module.exports["NestingTest"] = class NestingTest extends Container{
    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}

        // initialize ids
        var e1 = new Element()
        this.ids['e1'] = e1
        
        var enested = new Element()
        this.ids['enested'] = enested

        var c1  = new Container()
        this.ids['c1'] = c1

        // declare properties for ids
        Element.addProperty(e1, 'x', {type: 'int', notify: 'xChanged'})
        Element.addProperty(enested, 'y', {type: 'int', notify: 'yChanged'})

        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignId(e1,"e1")
                Element.assignPropertyExpression(
                    this,
                    "x",
                    function(){ return enested.y }.bind(this),
                    [[enested, "yChanged"]]
                )
                Element.assignDefaultProperty(null)
                return this
            }.bind(e1)(this)),
            (function(parent){
                this.setParent(parent)
                Element.assignId(c1, "c1")
                Element.assignDefaultProperty(this, [
                    (function(parent){
                        this.setParent(parent)
                        Element.assignId(enested, "enested")
                        this.y = 200
                        Element.assignDefaultProperty(null)
                        return this
                    }.bind(enested)(this))
                ])
                return this
            }.bind(c1)(this))
        ])
    }
}

module.exports["ShortIdTest"] = class ShortIdTest extends Container{
    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}
        var e1 = new Element()
        this.ids['e1'] = e1

        Element.addProperty(e1, 'x', {type: 'int', notify: 'xChanged'})

        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignId(e1, "e1")
                this.x = 10
                Element.assignDefaultProperty(null)

                return this
            }.bind(e1)(this))
        ])
    }
}
