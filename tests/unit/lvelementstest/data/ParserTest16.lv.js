import {TodoListItem} from '__UNRESOLVED__'
import {Ul} from '__UNRESOLVED__'

export class TodoList extends Ul{

    constructor(){
        super()
        TodoList.prototype.__initialize.call(this)
    }

    __initialize(){
        Element.assignPropertyExpression(this,
            'children',
            function(){ return this.items.map((item, index) => {
                return (function(parent){
                    this.setParent(parent)
                    Element.complete(this)
                    return this
                }.bind(new TodoListItem())(null))
            })}.bind(this),
            [[this,'items']]
        )
    }

    run(){
        this.children = this.items.map( s => new TodoListItem(s))
        this.children = this.items.map( (s, e) => new TodoListItem(s, e))
    }
}
