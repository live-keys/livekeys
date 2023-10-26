import {LanguageScanner} from '__UNRESOLVED__'
import {Scenario} from '__UNRESOLVED__'
import {TestCase} from '__UNRESOLVED__'

export let ParserTest14 = (function(parent){
    this.setParent(parent)
    Element.addProperty(this, 'ls', { type: 'LanguageScanner', notify: 'lsChanged' })
    this.ls = (function(parent){
        this.setParent(parent)
        Element.addProperty(this, 'x', { type: 'int', notify: 'xChanged' } )
        this.x = 0
        Element.complete(this)
        return this
    }.bind(new LanguageScanner())(this))

    Element.assignChildrenAndComplete(this, [
        (function(parent){
            this.setParent(parent)
            this.run = (t) => {
                var ls = new LanguageScanner()
            }
            Element.complete(this)
            return this
        }.bind(new Scenario())(this))
    ])
    return this
}.bind(new TestCase())(null))
