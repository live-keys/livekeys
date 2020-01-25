imports.require('package')

var T = imports.get('T')

(function(parent){
    this.setParent(parent)
    this.a.b.c = 20
    return this
}.bind(new T())(null))
