import QtQuick 2.3
import base 1.0

Act{
    id: root

    property QtObject input: null
    default property list<QtObject> transformations

    run: function(){
        if ( !transformations.length ){
            return input
        }

        transformations[0].input = root.input
        for ( var i = 1; i < transformations.length; ++i ){
            transformations[i].input = transformations[i - 1].result
        }
        return transformations[transformations.length - 1].result
    }

}
