
export class C extends Element{
    constructor(){
        super()
        C.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.addProperty(this, 'c1', { type: "int", notify: "c1Changed" })
        Element.addProperty(this, 'c2', { type: "int", notify: "c2Changed" })
        Element.addProperty(this, 'c3', { type: "int", notify: "c3Changed" })
        Element.addProperty(this, 'c4', { type: "int", notify: "c4Changed" })
        Element.addProperty(this, 'c5', { type: "var", notify: "c5Changed" })

        this.c1 = 100
        this.c3 = 300
        this.c5 = (function(){ return 10 }.bind(this))()
    }
}
