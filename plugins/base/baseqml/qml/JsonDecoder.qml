import QtQuick 2.3
import base 1.0

Act{
    property string data: ''

    run: function(){
        return JSON.parse(data)
    }

}
