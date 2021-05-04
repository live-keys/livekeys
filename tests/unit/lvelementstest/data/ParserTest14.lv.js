imports.require('test')
imports.require('language')

var LanguageScanner = imports.get('LanguageScanner')
var Scenario = imports.get('Scenario')
var TestCase = imports.get('TestCase')

module.exports["ParserTest14"] = (function(parent){
    this.setParent(parent)
    Element.addProperty(this, 'ls', { type: 'LanguageScanner', notify: 'lsChanged' })
    this.ls = (function(parent){
        this.setParent(parent)
        Element.addProperty(this, 'x', { type: 'int', notify: 'xChanged' } )
        this.x = 0
        return this
    }.bind(new LanguageScanner())(this))

    Element.assignDefaultProperty(this, [
        (function(parent){
            this.setParent(parent)
            this.run = (t) => {
                var ls = new LanguageScanner()
            }
            return this
        }.bind(new Scenario())(this))
    ])
    return this
}.bind(new TestCase())(null))
