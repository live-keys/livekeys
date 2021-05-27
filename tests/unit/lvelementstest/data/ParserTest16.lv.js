var TodoListItem = imports.get('TodoListItem')
var Ul = imports.get('Ul')

module.exports["TodoList"] = class TodoList extends Ul{

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