module.exports["A"] = class A extends Element{
    constructor(a, b){
        super()
        this.__initialize()
        this.a = a
        this.b = b
    }
    
    __initialize(){}

    otherFunction(a){
        this.a = a;
    }
}

module.exports["B"] = class B extends Element{
    constructor(){
        super()
        this.__initialize()
    }
    
    __initialize(){}

    createA(){
        return (function(parent){
            this.setParent(parent)
            return this
        }.bind(new A(12, 13))(null))
    }
}

module.exports["C"] = class C extends Element{
    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        Element.addProperty(this, 'c1', {type: "int", notify: "c1Changed" })
        Element.addProperty(this, 'c2', {type: "int", notify: "c2Changed" })

        this.c1 = 200
        this.c2 = 300
    }
}

module.exports["D"] = class D extends Container{
    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                return this
            }.bind(new A(12, 13))(this)),
            (function(parent){
              this.setParent(parent)
              return this
            }.bind(new A(14, 15))(this)),
            (function(parent){
              this.setParent(parent)
              return this
            }.bind(new A(16, 17))(this))
        ])
    }
}
