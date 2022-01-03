import {TodoListItem} from '__UNRESOLVED__'
import {Ul} from '__UNRESOLVED__'

export class TodoList extends Ul{

    constructor(){
        super()
        TodoList.prototype.__initialize.call(this)
    }

    __initialize(){
        this.children = this.items.map((item, index) => {
            return(function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new TodoListItem())(null))
        })
    }

}
