imports.require('test')

var Scenario = imports.get('Scenario')
var TestCase = imports.get('TestCase')

module.exports["ParserTest13"] = (function(parent){
    this.setParent(parent)
    Element.assignDefaultProperty(this, [
        (function(parent){
            this.setParent(parent)
            this.describe = "assert true test"
            this.run = (t) => {
                t.assert(true)
            }
            return this
        }.bind(new Scenario())(this))
    ])
    return this
}.bind(new TestCase())(null))