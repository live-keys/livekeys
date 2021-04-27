module.exports["TodoListItem"] = class TodoListItem extends Element{

    constructor(){
        super()
        this.__initialize()
    }

    __initialize(){
        this.on('remove', function(index){
            todoList.remove(index)
        }.bind(this));
        this.on('markDone', function(index){
            todoList.markTodoDone(index) 
        }.bind(this));
        this.key = index
        this.item = item
    }

}