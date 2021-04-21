import QtQuick 2.3
import base 1.0

Act{
    property var data: null

    run: function(){
        return JSON.stringify(data)
    }

}
