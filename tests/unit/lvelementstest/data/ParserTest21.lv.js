import {Div} from '__UNRESOLVED__'
import {TodoForm} from '__UNRESOLVED__'
import {TodoHeader} from '__UNRESOLVED__'
import {TodoList} from '__UNRESOLVED__'
import {todoItems} from '__UNRESOLVED__'

export class TodoApp extends Div{

    constructor(){
        super()
        TodoApp.prototype.__initialize.call(this)
    }

    __initialize(){
        this.ids = {}

        var main = this
        this.ids["main"] = main

        Element.addProperty(this, 'todoItems', { type: "var", notify: "todoItemsChanged" })
        this.todoItems = todoItems


        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.complete(this)
                return this
            }.bind(new TodoHeader())(this)),
            (function(parent){
                this.setParent(parent)
                Element.assignPropertyExpression(this,
                    'items',
                    function(){ return this.todoItems}.bind(this),
                    [
                        [ this, 'todoItems' ]
                    ]
                )
                this.on('remove', function(index){
                    this.todoItems.splice(index, 1);
                    this.todoItemsChanged()
                }.bind(this));
                this.on('markTodoDone', function(itemIndex){
                    var todo = todoItems[itemIndex];
                    todoItems.splice(itemIndex, 1);
                    todo.done = !todo.done;
                    todo.done ? todoItems.push(todo) : todoItems.unshift(todo);
                    this.todoItemsChanged()
                }.bind(this));
                Element.complete(this)
                return this
            }.bind(new TodoList())(this)),
            (function(parent){
                this.setParent(parent)
                this.on('addItem', function(name){ this.todoItems.unshift({ index: todoItems.length+1, value: name, done: false }) }.bind(this));
                Element.complete(this)
                return this
            }.bind(new TodoForm())(this))
        ])
    }

}
