import {Container} from '__UNRESOLVED__'

export class A extends Element{
    constructor(a, b){
        super()
        A.prototype.__initialize.call(this)
        this.a = a
        this.b = b
    }
    
    __initialize(){
    }

    otherFunction(a){
        this.a = a;
    }
}

export class B extends Element{
    constructor(){
        super()
        B.prototype.__initialize.call(this)
    }
    
    __initialize(){
    }

    createA(){
        return (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new A(12, 13))(null))
    }
}

export class C extends Element{
    constructor(){
        super()
        C.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.addProperty(this, 'c1', {type: 'int', notify: 'c1Changed' })
        Element.addProperty(this, 'c2', {type: 'int', notify: 'c2Changed' })

        this.c1 = 200
        this.c2 = 300
    }
}

export class D extends Container{
    constructor(){
        super()
        D.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                 Element.complete(this)
                return this
            }.bind(new A(12, 13))(this)),
            (function(parent){
              this.setParent(parent)
                Element.complete(this)
              return this
            }.bind(new A(14, 15))(this)),
            (function(parent){
              this.setParent(parent)
              Element.complete(this)
              return this
            }.bind(new A(16, 17))(this))
        ])
    }
}

