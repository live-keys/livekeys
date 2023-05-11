
import {X} from '__UNRESOLVED__'

export class ParserTest49 extends Element{

    constructor(){
        super()
        ParserTest49.prototype.__initialize.call(this)
    }
    __initialize(){
        Element.addProperty(this, 'x', { type: 'var', notify: 'xChanged'})
        Element.addProperty(this, 'y', { type: 'var', notify: 'yChanged'})
        Element.addProperty(this, 'z', { type: 'var', notify: 'zChanged'})
        Element.addProperty(this, 'w', { type: 'var', notify: 'wChanged'})
        Element.addProperty(this, 't', { type: 'var', notify: 'tChanged'})
        this.x = class extends Element{

            constructor(){
                super()
                new.target.prototype.__initialize.call(this)
            }
            __initialize(){
            }

        }

        this.y = class extends X{

            constructor(){
                super()
                new.target.prototype.__initialize.call(this)
            }
            __initialize(){
            }

        }

        this.z = class extends X{

            constructor(){
                super()
                new.target.prototype.__initialize.call(this)
            }
            __initialize(){
            }

        }

        this.w = class extends X{

            constructor(){
                super()
                new.target.prototype.__initialize.call(this)
            }
            __initialize(){
                this.ids = {}

                var x = this
                this.ids["x"] = x

            }

            completed(){
            let a = 5
            let b = class extends X{

                    constructor(){
                        super()
                        new.target.prototype.__initialize.call(this)
                    }
                    __initialize(){
                    }

                }

            }
        }

        this.t = class extends X{

            constructor(){
                super()
                new.target.prototype.__initialize.call(this)
            }
            __initialize(){
            }

            completed(){
                let a =class extends X.Y{

                    constructor(){
                        super()
                        new.target.prototype.__initialize.call(this)
                    }
                    __initialize(){
                    }

                }

            }
        }

    }

}
