export class FunctionComponent extends Element{
    constructor(){
        super()
        this.__initialize()
    }
    
    __initialize(){
        // Element.assignDefaultProperty(null)
    }

    test(param1, param2){
        console.log(param1, param2)
        
        class InsideComponent extends Element{
            constructor(){
                super()
                this.__initialize()
            }

            __initialize(){
                Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
                this.x = 200
                // Element.assignDefaultProperty(null)
            }
        }
    }
}

export class FunctionCreatesComponent extends Element{
    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        // Element.assignDefaultProperty(null)
    }

    test(param1, param2){
        console.log(param1, param2)

        (function(parent){
            this.setParent(parent)

            Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
            this.x = 200
            // Element.assignDefaultProperty(null)
            return this
        }.bind(new Element())(null))
    }
}
