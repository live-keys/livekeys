import {TodoListItem} from '__UNRESOLVED__'
import {Ul} from '__UNRESOLVED__'

export class TodoList extends Ul{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.children = this.items.map((item, index) => {
            return(function(parent){
                this.setParent(parent)
                return this
            }.bind(new TodoListItem())(null))
        })
    }

}
