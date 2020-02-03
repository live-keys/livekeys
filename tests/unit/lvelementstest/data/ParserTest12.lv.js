imports.require('package')
var T = imports.get('T')

module.exports["ParserTest12"] = (function(parent){
    this.setParent(parent)
    this.a = 20
    return this
}.bind(new T())(null))
