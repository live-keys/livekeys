import QtQuick 2.3
import base 1.0 as B
  
B.Act{
    property var input: null

    args: ['$input']
    run: function(input){
        return B.PromiseOp.create(function(resolve, reject){
            var result = []
            input.forward(
                (val) => {
                    result.push(val)
                },
                () => {
                    resolve(result)
                }
            )
        });
    }
}
