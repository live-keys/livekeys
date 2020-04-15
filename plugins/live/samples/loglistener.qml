import QtQuick 2.3
import live 1.0
import lcvcore 1.0

LogListener{
    
    // This sample show the usage of a network log listener.
    // Start another instance of livekeys with the following options and
    // log something, then come back and check the log on this instance:
    
    // ./livekeys --log-prefix "%p" --log-tonetwork "127.0.0.1:1590" 
    
    address: '127.0.0.1'
    onListening: console.log("Listening on: " + address + ":" + port)
}
