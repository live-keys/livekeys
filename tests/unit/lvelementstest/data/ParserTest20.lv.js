import {Button} from '__UNRESOLVED__'
import {Form} from '__UNRESOLVED__'
import {Input} from '__UNRESOLVED__'

export class TodoForm extends Form{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.ids = {}

        var itemName = new Input()
        this.ids["itemName"] = itemName

        Element.addEvent(this, 'addItem', [])
        this.on('submit', function(event){
            event.preventDefault();
            if(itemName.value) {
                this.addItem(itemName.value)
                this.reset()
            }
        }.bind(this));
        Element.assignDefaultProperty(this, [
            (function(parent){
                this.setParent(parent)
                Element.assignId(itemName, "itemName")
                this.type = "text";
                this.classes = "form-control";
                this.placeholder = "Add New TODO..."
                return this
            }.bind(itemName)(this)),
            (function(parent){
                this.setParent(parent)
                this.type = "submit";
                this.classes = "btn btn-default";
                this.text = "Add"
                return this
            }.bind(new Button())(this))
        ])
    }
}
