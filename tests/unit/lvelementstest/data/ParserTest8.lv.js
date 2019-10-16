module.exports["ParserTest8"] = (function(parent){
    this.setParent(parent)

    Element.assignDefaultProperty(this, [
        (function(parent){
            this.setParent(parent)
            return this
        }.bind(new H1("Paragraph 1"))(this)),
        (function(parent){
            this.setParent(parent)

            this.assignDefaultProperty(this, [
                (function(parent){
                    this.setParent(parent)
                    return this
                }.bind(new T("This is "))(this)),
               (function(parent){
                   this.setParent(parent)
                   return this
               }.bind(new B("paragraph 1"))(this)),
               (function(parent){
                   this.setParent(parent)
                   return this
               }.bind(new T("."))(this)),
            ])

            return this
        }.bind(new P())(this)),
      (function(parent){
          this.setParent(parent)
          return this
      }.bind(new H1("Paragraph 2"))(this)),
          (function(parent){
              this.setParent(parent)

              this.assignDefaultProperty(this, [
                  (function(parent){
                      this.setParent(parent)
                      return this
                  }.bind(new T("This is paragraph 2. Lines are merged into one. To add space and new lines we would you \\n and \\s characters. \n This will be "))(this)),
                 (function(parent){
                     this.setParent(parent)
                     return this
                 }.bind(new B("bold text. \n And this will be an "))(this)),
                 (function(parent){
                     this.setParent(parent)

                     this.href = "https://livekeys.io"

                     this.assignDefaultProperty(this, [
                        (function(parent){
                            this.setParent(parent)
                            return this
                        }.bind(new T("link"))(this))
                     ])

                     return this
                 }.bind(new A())(this)),
              ])

              return this
          }.bind(new P())(this))
    ])

    return this
}.bind(new Article())(null))
