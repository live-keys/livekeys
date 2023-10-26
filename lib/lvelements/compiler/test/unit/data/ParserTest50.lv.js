import {X} from '__UNRESOLVED__'

export class ParserTest50 extends Element{

    constructor(){
        super()
        ParserTest50.prototype.__initialize.call(this)
    }
    __initialize(){
    }

    trycatch(){
        try{
            this.run()
            let x = (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new X())(null))
        } catch ( e ){
            console.log(e)
            let x =    (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new X())(null))
        } finally {
            let x = (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new X())(null))
        }

        try{
        } catch {
            let x = (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new X())(null))
        }
    }
}
