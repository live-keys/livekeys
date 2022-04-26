
export let elem = (function(parent){
    this.setParent(parent)
    Element.addEvent(this, 'add', [])
    Element.addEvent(this, 'remove', [])
    this.on('remove', function(index){
        this.items.remove(index)
    }.bind(this));
    this.on('add', function(item){
        this.items.push(item)
    }.bind(this));
    this.insert = function(item,index){
        this.items.insert(item, index)
    }
    Element.complete(this)
    return this
}.bind(new Element())(null))

export class X extends Element{

    constructor(){
        super()
        X.prototype.__initialize.call(this)
    }
    __initialize(){
        Element.addEvent(this, 'add', [])
        Element.addEvent(this, 'remove', [])
        this.on('remove', function(index){
            this.items.remove(index)
        }.bind(this));
        this.on('add', function(item){
            this.items.push(item)
        }.bind(this));
    }

    insert(item,index){
        this.items.insert(item, index)
    }
}
