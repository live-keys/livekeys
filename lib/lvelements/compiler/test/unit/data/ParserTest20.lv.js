import {Button} from '__UNRESOLVED__'
import {Form} from '__UNRESOLVED__'
import {Input} from '__UNRESOLVED__'

export class TodoForm extends Form{

    constructor(){
        super()
        TodoForm.prototype.__initialize.call(this)
    }

    __initialize(){
        this.ids = {}

        var itemName = new Input()
        this.ids["itemName"] = itemName

        Element.addEvent(this, 'addItem', [['string', 'name']])
        this.on('submit', function(event){
            event.preventDefault();
            if(itemName.value) {
                this.addItem(itemName.value)
                this.reset()
            }
        }.bind(this));
        Element.assignChildren(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignId(itemName, "itemName")
                this.type = "text"
                this.classes = "form-control"
                this.placeholder = "Add New TODO..."
                Element.complete(this)
                return this
            }.bind(itemName)(this)),
            (function(parent){
                this.setParent(parent)
                this.type = "submit"
                this.classes = "btn btn-default"
                this.text = "Add"
                Element.complete(this)
                return this
            }.bind(new Button())(this))
        ])
    }
}
