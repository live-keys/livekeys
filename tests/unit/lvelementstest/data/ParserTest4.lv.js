var A = imports.requireAs("xmodule")

var not1 = imports.get("not1") // not1 is a local function and should be accessed by this
var not2 = imports.get("not2") // not2 is a local property and should be accessed by this

module.exports["B"] = class B extends Element{
    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}

        var C = this
        this.ids["C"] = C

        Element.addProperty(this, 'not2', {type: "int", notify: 'not2Changed'})
        this.not2 = 10

        // Element.assignDefaultProperty(null)
    }

    not1(d, e){
        var f = 5

        class G{}
        function H(){}

        console.log(A, B, C, d, e, f, G, not1, not2)
    }
}
