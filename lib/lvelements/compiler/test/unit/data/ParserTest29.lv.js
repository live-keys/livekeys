import {CustomElement} from 'custom/CustomElement.js'

export class C extends CustomElement{

    constructor(){
        super()
        C.prototype.__initialize.call(this)
    }

    __initialize(){

        CustomElement.addProperty(this, 'c1', { type: 'int', notify: 'c1Changed' })
        CustomElement.addProperty(this, 'c2', { type: 'int', notify: 'c2Changed' })

        this.on('remove', function(index){
            vlog.i('remove triggered at: ' + index)
        }.bind(this));

        this.c1 = 200
        this.c2 = 300
    }

    createA(){
        return "createA"
    }
}
