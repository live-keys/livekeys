import QtQuick 2.3
import visual.shapes 1.0

SvgView{
    property color color: '#fff'
    content:
        '<svg xmlns="http://www.w3.org/2000/svg"  width="94" height="94" viewBox="0 0 94 94">' +
            '<defs><style>' +
                '.cls-1 { fill: ' + color + '; fill-rule: nonzero; }' +
            '</style></defs>' +
            '<path id="_" class="cls-1" d="M1,48A47,47,0,1,0,48,1,47.053,47.053,0,0,0,1,48Zm5.01,0A41.99,41.99,0,1,1,48,89.99,42.038,42.038,0,0,1,6.01,48ZM48.5,19A29.5,29.5,0,1,1,19,48.5,29.5,29.5,0,0,1,48.5,19Z" transform="translate(-1 -1)"/>' +
        '</svg>'
}
