var TodoListItem = imports.get('TodoListItem')
var Ul = imports.get('Ul')

module.exports["TodoList"] = class TodoList extends Ul{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}

        var todoList = this
        this.ids["todoList"] = todoList

        Element.addProperty(this, 'items', { type: "list", notify: "itemsChanged" })
        Element.addEvent(this, 'remove', [])
        Element.addEvent(this, 'markTodoDone', [])
        this.items = []
        this.classes = ['list-group']
        this.children = this.items.map((item, index) => {
            return(function(parent){
                this.setParent(parent)
                this.key = index;
                this.item = item;
                this.on('remove', function(index){ todoList.remove(index) }.bind(this));
                this.on('markDone', function(index){ todoList.markDone(index) }.bind(this));
                return this
            }.bind(new TodoListItem())(null))
        })
    }

}
