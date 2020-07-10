import QtQuick 2.3
import base 1.0

Item{

    Exec{
        id: exec
        path: "cmd.exe"
        args: ["/c", "dir", "/b", ".."]
        onReady: run()
        onFinished:
            wcexec.closeInput()
    }

    Exec{
        id: wcexec
        input: exec.out
        path: "powershell.exe"
        args : ["(@(While($l=(Read-Host).Trim()){$l}) -join(\"`n\")) | Measure-Object -Line"]
        onReady: run()
    }

    StreamLog{
        id: streamLog
        stream: wcexec.out
    }

}
