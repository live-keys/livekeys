import {Button} from '__UNRESOLVED__'
import {Div} from '__UNRESOLVED__'
import {Li} from '__UNRESOLVED__'
import {Span} from '__UNRESOLVED__'
import {T} from '__UNRESOLVED__'

export class TodoListItem extends Li{

    constructor(){
        super()
        TodoListItem.prototype.__initialize.call(this)
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
        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignPropertyExpression(this,
                    'classes',
                    function(){ return todoListItem.item.done ? "done" : "undone"}.bind(this),
                    [ [ todoListItem.item, 'doneChanged' ] ]
                )
                Element.assignChildrenAndComplete(this, [
                    (function(parent){
                        this.setParent(parent)
                        this.classes = "glyphicon glyphicon-ok icon"
                        this.ariaHidden = true
                        this.on('click', function(){ todoListItem.markDone() }.bind(this));
                        Element.complete(this)
                        return this
                    }.bind(new Span())(this)),
                    (function(parent){
                        this.setParent(parent)
                        Element.assignPropertyExpression(this,
                            'text',
                            function(){ return todoListItem.item.value}.bind(this),
                            [
                                [ todoListItem.item, 'valueChanged' ]
                            ]
                        )
                        Element.complete(this)
                        return this
                    }.bind(new T())(this)),
                    (function(parent){
                        this.setParent(parent)
                        this.classes = "close"
                        this.text = "&times;"
                        this.on('click', function(){ todoListItem.remove()}.bind(this));
                        Element.complete(this)
                        return this
                    }.bind(new Button())(this))
                ])
                return this
            }.bind(new Div())(this))      
        ])
    }

}
