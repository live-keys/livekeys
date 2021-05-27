var Button = imports.get('Button')
var Div = imports.get('Div')
var Li = imports.get('Li')
var Span = imports.get('Span')
var T = imports.get('T')

module.exports["TodoListItem"] = class TodoListItem extends Li{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}
        var todoListItem = this
        this.ids["todoListItem"] = todoListItem

        Element.addProperty(this, 'item', { type: "var", notify: "itemChanged" })
        Element.addEvent(this, 'remove', [])
        Element.addEvent(this, 'markDone', [])
        this.item = null
        this.classes = "list-group-item"
        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignPropertyExpression(this,
                    'classes',
                    function(){ return item.done ? "done" : "undone"}.bind(this),
                    [
                        [ item, 'doneChanged' ]
                    ]
                )
                Element.assignDefaultProperty(this, [
                    (function(parent){
                        this.setParent(parent)
                        this.classes = "glyphicon glyphicon-ok icon";
                        this.ariaHidden = true;
                        this.on('click', function(){ todoListItem.markDone() }.bind(this));
                        return this
                    }.bind(new Span())(this)),
                    (function(parent){
                        this.setParent(parent)
                        Element.assignPropertyExpression(this,
                            'text',
                            function(){ return item.value}.bind(this),
                            [
                                [ item, 'valueChanged' ]
                            ]
                        )
                        return this
                    }.bind(new T())(this)),
                    (function(parent){
                        this.setParent(parent)
                        this.classes = "close";
                        this.text = "&times;"
                        this.on('click', function(){ todoListItem.remove()}.bind(this));
                        return this
                    }.bind(new Button())(this))
                ])
                return this
            }.bind(new Div())(this))      
        ])
    }

}