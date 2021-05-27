var Div = imports.get('Div')
var TodoForm = imports.get('TodoForm')
var TodoHeader = imports.get('TodoHeader')
var TodoList = imports.get('TodoList')

module.exports["TodoApp"] = class TodoApp extends Div{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}

        var main = this
        this.ids["main"] = main

        Element.addProperty(this, 'todoItems', { type: "var", notify: "todoItemsChanged" })
        this.todoItems = todoItems


        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                return this
            }.bind(new TodoHeader())(this)),
            (function(parent){
                this.setParent(parent)
                Element.assignPropertyExpression(this,
                    'items',
                    function(){ return this.todoItems}.bind(this),
                    [
                        [ this, 'todoItemsChanged' ]
                    ]
                )
                this.on('remove', function(index){ this.todoItems.splice(index, 1); this.todoItemsChanged() }.bind(this));
                this.on('markTodoDone', function(index){ 
                    var todo = todoItems[itemIndex];
                    todoItems.splice(itemIndex, 1);
                    todo.done = !todo.done;
                    todo.done ? todoItems.push(todo) : todoItems.unshift(todo);
                    this.todoItemsChanged()
                }.bind(this));
                return this
            }.bind(new TodoList())(this)),
            (function(parent){
                this.setParent(parent)
                onAddItem(name) => {
                    this.todoItems.unshift({ index: todoItems.length+1, value: name, done: false })
                }
                return this
            }.bind(new TodoForm())(this))
        ])
    }

}
