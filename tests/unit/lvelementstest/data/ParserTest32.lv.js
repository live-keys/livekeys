
export let ParserTest32 = (function(parent){
    this.setParent(parent)
    this.x = function(){
        return (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new Element())(null))
    }
    this.y = function(){
        var a = (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new Element())(null))
        var run = function(e){ return e }
        var b = run((function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new Element())(null)))
        var c = function(){
            return (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new Element())(null))
        }
        return { a: a, b: b, c: c }
    }
    Element.complete(this)
    return this
}.bind(new Element())(null))
