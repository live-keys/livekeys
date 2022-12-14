import QtQuick 2.3
import editqml 1.0
import base 1.0
import base 1.0 as B

Item{
    anchors.fill: parent

   B.ValueFlowGraph{
       id: valueFlowGraph
       input: 'text1'
       returns: 'qml/string'

       ComponentSource{
           B.ValueFlow{
               B.ValueFlowInput{ id: valueFlowInput }
               B.TextConcat{ id: add; text1: valueFlowInput.result; text2: ' text2' }
               B.ValueFlowSink{ id: valueFlowSink; value: add.result }
           }
       }
   }

   Text{
       color: 'white'
       text: valueFlowGraph.result
   }

}
