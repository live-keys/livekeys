export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
        Element.addProperty(this, 'a1', { type: 'int', notify: 'a1Changed'})
        Element.addProperty(this, 'a2', { type: 'int', notify: 'a2Changed'})
        Element.addProperty(this, 'a3', { type: 'int', notify: 'a3Changed'})
        Element.addProperty(this, 'a4', { type: 'int', notify: 'a4Changed'})
        this.a1 = 100
        this.a2 = 200
    Element.assignPropertyExpression(this,
    'a3',
    function(){ return this.a1}.bind(this),
    [[this,'a1']]
    )
        this.a4 = this.a2
        Element.assignPropertyExpression(this,
            'b1',
            function(){ return this.a1}.bind(this),
            [[this,'a1']]
        )
        this.b2 = this.a2
    }

}

export let x = (function(parent){
    this.setParent(parent)
        Element.addProperty(this, 'a1', { type: 'int', notify: 'a1Changed'})
        Element.addProperty(this, 'a2', { type: 'int', notify: 'a2Changed'})
        Element.addProperty(this, 'a3', { type: 'int', notify: 'a3Changed'})
        Element.addProperty(this, 'a4', { type: 'int', notify: 'a4Changed'})
    this.a1 = 100
    this.a2 = 200
    Element.assignPropertyExpression(this,
        'a3',
        function(){ return this.a1}.bind(this),
        [[this,'a1']]
    )
    this.a4 = this.a2
    Element.assignPropertyExpression(this,
        'b1',
        function(){ return this.a1}.bind(this),
        [[this,'a1']]
    )
    this.b2 = this.a2
    Element.complete(this)
    return this
}.bind(new X())(null))