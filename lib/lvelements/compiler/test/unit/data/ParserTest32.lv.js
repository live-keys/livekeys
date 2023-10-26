

export class ParserTest32 extends Element{

    constructor(){
        super()
        ParserTest32.prototype.__initialize.call(this)
    }
    __initialize(){
    }

    x(){ return
        (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new Element())(null))
    }

    y(){
        var a = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new Element())(null))

        var run = function(e){ return e }

        var b = run(
            (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new Element())(null))
        )
        var c = function(){ return
            (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new Element())(null))
        }
        return { a: a, b: b, c: c }
    }
}
