import {Container} from '__UNRESOLVED__'

export class ParserTest07 extends Container{

    constructor(){
        super()
        ParserTest07.prototype.__initialize.call(this)
    }

    __initialize(){
        // Declare all parent properties
        Element.addProperty(this, "a", {type: "int", notify: "aChanged"})
        Element.addProperty(this, "b", {type: "int", notify: "bChanged"})
        Element.addProperty(this, "c", {type: "int", notify: "cChanged"})

        this.a = 20
        this.b = 30
        this.c = 20

        Element.assignChildren(this, [

            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'x', {type: "int", notify: 'xChanged'})

                Element.assignPropertyExpression(
                    this,
                    "x",
                    function(){ return parent.a + parent.b }.bind(this),
                    [[parent, 'a', 'b']]
                )
                 Element.complete(this)
                return this
            }.bind(new Element())(this)),

            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', {type: "int", notify: 'yChanged'})

                Element.assignPropertyExpression(
                    this,
                    "y",
                    function(){
                        function a(){
                            return parent.a
                        }
                        return a() + parent.b
                    }.bind(this),
                    // only b gets bound, 'parent.a' is not in the binding scope
                    [[parent, 'b']]
                )
                 Element.complete(this)
                return this
            }.bind(new Element())(this)),


            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'z', {type: "int", notify: 'zChanged'})

                Element.assignPropertyExpression(
                    this,
                    "z",
                    function(){
                        var result = 0
                        for ( var i = 0; i < 3; ++i ){
                            result += parent.a
                        }
                        if ( parent.b > 0 )
                            result += parent.b
                        return result
                    }.bind(this),
                    // conditionals and loops or any other scopes that are not functions/classes/components
                    // should activate their bindings
                    [[parent, 'a', 'b']]
                )
                 Element.complete(this)

                return this
            }.bind(new Element())(this)),

            (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'w', {type: "int", notify: 'wChanged'})

                this.w = (function(){
                    var result = 0;

                    class A{
                        constructor(){
                            this.field = parent.a
                        }
                    }

                    result += (new A()).field;
                    return result
                }.bind(this)())
                 Element.complete(this)

                return this
            }.bind(new Element())(this))
        ])
    }
}
