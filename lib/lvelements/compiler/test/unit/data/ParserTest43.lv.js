export let elem = (function(parent){
    this.setParent(parent)
    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
    Element.assignPropertyExpression(this,
        'y',
        function(){
            this.a = this.b
            return 100
        }.bind(this),
        [[this, 'b']]
    )

    this.x = (function(){
        this.a = 100
        return 200
    }.bind(this)())

    Element.assignPropertyExpression(this,
        'z',
        function(){
            return this.c
        }.bind(this),
        [[this, 'c']]
    )
    Element.complete(this)
    return this
}.bind(new Element())(null))
