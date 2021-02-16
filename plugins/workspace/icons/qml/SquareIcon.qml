import QtQuick 2.3
import visual.shapes 1.0

SvgView{
    property color color: 'white'
    content:
        '<svg width="1e3" height="1e3" version="1.1" xmlns="http://www.w3.org/2000/svg">' +
          '<title>square</title>' + 
          '<glyph glyph-name="square" horiz-adv-x="1000"/>' +
          '<path d="m865.3 62.5h-730.6c-40.1 0-72.2 32.5-72.2 72.2v730.7c0 39.7 32.1 72.2 72.2 72.2h730.7c39.7 0 72.2-32.5 72.2-72.2v-730.7c-0.1-39.7-32.6-72.2-72.3-72.2z"/>' +
          '<path d="m123.53 936.25c-21.051-3.5626-42.822-19.459-52.066-38.016-8.6706-17.406-7.985 18.559-7.6486-401.24l0.30309-378.17 3.0794-7.914c7.0351-18.08 19.223-31.66 35.744-39.828 17.743-8.7716-15.223-8.0992 397.06-8.0992 409.31 0 379.08-0.56552 395.85 7.4031 16.436 7.8088 30.708 23.507 37.236 40.958l2.798 7.48 0.31087 378.82 0.31086 378.82-2.4656 8.2353c-6.97 23.28-22.895 39.65-47.832 49.171l-6.7945 2.5939-376.47 0.13932c-207.06 0.0766-377.79-0.0847-379.41-0.35845z" fill="' + color + '"/>' +
        '</svg>'
}
