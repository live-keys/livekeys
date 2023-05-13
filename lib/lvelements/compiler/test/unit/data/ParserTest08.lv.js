import {A} from '__UNRESOLVED__'
import {Article} from '__UNRESOLVED__'
import {B} from '__UNRESOLVED__'
import {H1} from '__UNRESOLVED__'
import {P} from '__UNRESOLVED__'
import {T} from '__UNRESOLVED__'

export let ParserTest08 = (function(parent){
    this.setParent(parent)

    Element.assignChildrenAndComplete(this, [
        (function(parent){
            this.setParent(parent)
            Element.complete(this)
            return this
        }.bind(new H1("Paragraph 1"))(this)),
        (function(parent){
            this.setParent(parent)

            Element.assignChildrenAndComplete(this, [
                (function(parent){
                    this.setParent(parent)
                    Element.complete(this)
                    return this
                }.bind(new T("This is "))(this)),
               (function(parent){
                   this.setParent(parent)
                    Element.complete(this)
                   return this
               }.bind(new B("paragraph 1"))(this)),
               (function(parent){
                   this.setParent(parent)
                    Element.complete(this)
                   return this
               }.bind(new T("."))(this))
            ])

            return this
        }.bind(new P())(this)),
      (function(parent){
          this.setParent(parent)
          Element.complete(this)
          return this
      }.bind(new H1("Paragraph 2"))(this)),
          (function(parent){
              this.setParent(parent)

              Element.assignChildrenAndComplete(this, [
                  (function(parent){
                      this.setParent(parent)
                      Element.complete(this)
                      return this
                  }.bind(new T("This is paragraph 2. Lines are merged into one. To add space and new lines we would use \\n and \\s characters. \n This will be "))(this)),
                 (function(parent){
                     this.setParent(parent)
                     Element.complete(this)
                     return this
                 }.bind(new B("bold text. \n And this will be an "))(this)),
                 (function(parent){
                     this.setParent(parent)
                     this.href = 'https://livekeys.io'

                     Element.assignChildrenAndComplete(this, [
                        (function(parent){
                            this.setParent(parent)
                            Element.complete(this)
                            return this
                        }.bind(new T("link"))(this))
                     ])

                     return this
                 }.bind(new A())(this))
              ])

              return this
          }.bind(new P())(this)),
         (function(parent){
            this.setParent(parent)
            Element.assignChildrenAndComplete(this, [
                (function(parent){
                    this.setParent(parent)
                    Element.complete(this)
                    return this
                }.bind(new T(" This paragraph will have an extra space   here, at the start and at the end of it. "))(this))

            ])
            return this
         }.bind(new P())(this))
    ])

    return this
}.bind(new Article())(null))
