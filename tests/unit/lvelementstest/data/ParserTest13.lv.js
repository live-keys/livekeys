import {Scenario} from '__UNRESOLVED__'
import {TestCase} from '__UNRESOLVED__'

export let ParserTest13 = (function(parent){
    this.setParent(parent)
    Element.assignChildrenAndComplete(this, [
        (function(parent){
            this.setParent(parent)
            this.describe = "assert true test"
            this.run = (t) => {
                t.assert(true)
            }
            Element.complete(this)
            return this
        }.bind(new Scenario())(this))
    ])
    return this
}.bind(new TestCase())(null))
