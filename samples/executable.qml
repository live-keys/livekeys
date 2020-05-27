import QtQuick 2.3
import base 1.0

Item{

    Exec{
        id: exec
        path: "ls"
        args : ["-la", ".."]
        onFinished: wcexec.closeInput()
    }
    
    Exec{
        id: wcexec
        input: exec.out
        path: "wc"
        args : "-l"
        onReady: run()
    }
    
    StreamLog{
        id: streamLog
        stream: wcexec.out
    }    
}
