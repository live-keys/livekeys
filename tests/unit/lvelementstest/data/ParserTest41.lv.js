import {Y} from '__UNRESOLVED__'

export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.addProperty(this, 'dExpr', { type: "var", notify: "dExprChanged" })
        Element.addProperty(this, 'dBlk', { type: "var", notify: "dBlkChanged" })
        Element.addProperty(this, 'dBindExpr', { type: "var", notify: "dBindExprChanged" })
        Element.addProperty(this, 'dBindBlock', { type: "var", notify: "dBindBlockChanged" })

        this.dExpr = '10' + (function(parent){
            this.setParent(parent)
            Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
            this.y = 10
            Element.complete(this)
            return this
        }.bind(new Element())(null)).toString()

        this.dBlk = (function(){
            return '10' + (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
            }.bind(new Element())(null)).toString()
        }.bind(this))()

        Element.assignPropertyExpression(this,
            'dBindExpr',
            function(){ return this.dExpr +
                (function(parent){
                    this.setParent(parent)
                    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                    this.y = 10
                    Element.complete(this)
                    return this
                }.bind(new Element())(null)).toString()
            }.bind(this),
            [[this,'dExpr']]
        )

        Element.assignPropertyExpression(this,
            'dBindBlock',
            (function(){
                return this.dExpr + (function(parent){
                    this.setParent(parent)
                    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                    this.y = 10
                    Element.complete(this)
                    return this
                }.bind(new Element())(null)).toString()
            }.bind(this)),
            [[this,'dExpr']]
        )

        this.aExpr = '10' + (function(parent){
            this.setParent(parent)
            Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
            this.y = 10
            Element.complete(this)
            return this
        }.bind(new Element())(null)).toString()

        this.aBlk = (function(){
            return '10' +
                (function(parent){
                    this.setParent(parent)
                    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                    this.y = 10
                    Element.complete(this)
                    return this
                }.bind(new Element())(null)).toString()
        }.bind(this)())

        Element.assignPropertyExpression(this,
            'aBindExpr',
             function(){ return this.dExpr +
                (function(parent){
                    this.setParent(parent)
                    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                    this.y = 10
                    Element.complete(this)
                    return this
                }.bind(new Element())(null)).toString()
            }.bind(this),
            [[ this, 'dExpr' ]]
        )

        Element.assignPropertyExpression(this,
            'aBindBlock',
            function(){ return this.dExpr +
                (function(parent){
                    this.setParent(parent)
                    Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                    this.y = 10
                    Element.complete(this)
                    return this
                }.bind(new Element())(null)).toString()
            }.bind(this),
            [[ this, 'dExpr' ]]
        )
    }
}

export let y = (function(parent){
    this.setParent(parent)
        Element.addProperty(this, 'dExpr', { type: 'var', notify: 'dExprChanged' })
        Element.addProperty(this, 'dBlk', { type: 'var', notify: 'dBlkChanged' })
        Element.addProperty(this, 'dBindExpr', { type: 'var', notify: 'dBindExprChanged' })
        Element.addProperty(this, 'dBindBlock', { type: 'var', notify: 'dBindBlockChanged' })
        this.dExpr = '10' + (function(parent){
            this.setParent(parent)
            Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
            this.y = 10
            Element.complete(this)
            return this
        }.bind(new Element())(null)).toString()

        this.dBlk = (function(){
            return '10' + (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
            }.bind(new Element())(null)).toString()
        }.bind(this)())

        Element.assignPropertyExpression(this,
            'dBindExpr',
            function(){ return this.dExpr + (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
            }.bind(new Element())(null)).toString()}.bind(this),
            [[ this, 'dExpr' ]]
        )

        Element.assignPropertyExpression(this,
            'dBindBlock',
            function(){
                return this.dExpr + (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
                }.bind(new Element())(null)).toString()
            }.bind(this),
            [[this, 'dExpr']]
        )

        this.aExpr = '10' + (function(parent){
            this.setParent(parent)
            Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
            this.y = 10
            Element.complete(this)
            return this
        }.bind(new Element())(null)).toString()

        this.aBlk = (function(){
            return '10' + (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
            }.bind(new Element())(null)).toString()
        }.bind(this)())

    Element.assignPropertyExpression(this,
        'aBindExpr',
        function(){ return this.dExpr + (function(parent){
            this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
            this.y = 10
            Element.complete(this)
            return this
        }.bind(new Element())(null)).toString()}.bind(this),
        [[this, 'dExpr']]
    )

    Element.assignPropertyExpression(this,
        'aBindBlock',
        function(){
            return this.dExpr + (function(parent){
                this.setParent(parent)
                Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged' })
                this.y = 10
                Element.complete(this)
                return this
            }.bind(new Element())(null)).toString()
        }.bind(this),
        [[this, 'dExpr']]
    )

    Element.complete(this)
    return this

}.bind(new Y())(null))
