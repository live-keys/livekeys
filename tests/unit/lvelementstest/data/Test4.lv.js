module.exports["Test4"] = class Test4 extends Container{

    constructor(var1, var2){
        super()
        this.__initialize()
        this.someVar = 20
    }

    __initialize(){
        // Declare all parent properties
        Element.addProperty(this, 'x', {type: 'int', notify: 'xChanged'})
        Element.addProperty(this, 'y', {type: 'int', notify: 'yChanged'})
        Element.addEvent(this, 'dataChanged', [])
        this.on("dataChanged", function(){
            this.x = 100;
        }.bind(this));

        // Assign parent properties
        this.x = 20
        this.y = 20

        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)

                Element.addProperty(this, 'k', {type: "int", notify: 'kChanged'})

                this.k = 20
                return this
            }.bind(new Element())(this)),
            (function(parent){
                this.setParent(parent)

                Element.addProperty(this, 'message', {type: 'string', notify: 'messageChanged'})
                this.message = "thirty"
                return this
            }.bind(new Element())(this))
        ])
    }
}




