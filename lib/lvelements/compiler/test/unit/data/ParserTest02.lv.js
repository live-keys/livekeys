export class FunctionComponent extends Element{
    constructor(){
        super()
        FunctionComponent.prototype.__initialize.call(this)
    }
    
    __initialize(){
    }

    test(param1, param2){
        console.log(param1, param2)
        
        class InsideComponent extends Element{
            constructor(){
                super()
                InsideComponent.prototype.__initialize.call(this)
            }

            __initialize(){
                Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
                this.x = 200
            }
        }
    }
}

export class FunctionCreatesComponent extends Element{
    constructor(){
        super()
        FunctionCreatesComponent.prototype.__initialize.call(this)
    }

    __initialize(){
    }

    test(param1, param2){
        console.log(param1, param2);

        (function(parent){
            this.setParent(parent)

            Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
            this.x = 200
             Element.complete(this)
            return this
        }.bind(new Element())(null))
    }
}
