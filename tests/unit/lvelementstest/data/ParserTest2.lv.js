module.exports["FunctionComponent"] = class FunctionComponent extends Element{
    constructor(){
        super()
    }

    test(param1, param2){
        console.log(param1, param2)

        class InsideComponent extends Element{
            constructor(){
                super()
                __initialize()
            }

            __initialize(){
                Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
                this.x = 200
            }
        }
    }
}

module.exports["FunctionCreatesComponent"] = class FunctionCreatesComponent extends Element{
    constructor(){
        super()
    }

    test(param1, param2){
        console.log(param1, param2)

        (function(parent){
            this.setParent(parent)

            Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
            this.x = 200

            return this
        }.bind(new Element())(parent))
    }
}
