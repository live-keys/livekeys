import base 1.0

Act{
    property var input
    property int nanValue: NaN

    run: function(input, nanValue){
        var v = parseInt(input)
        return v === Nan ? nanValue : v
    }
    args: ["$input", "$nanValue"]
}
