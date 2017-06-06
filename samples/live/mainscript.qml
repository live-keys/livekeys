import live 1.0

// This sample shows how to create a main script
// with Live CV that can be executed from the command line

// Run this sample with ./livecv -p "Hello"

Main{
    options: [
        { key: ['-p', '--print'], type: 'string', describe: 'Data to print on the screen.', required: true }
    ]
    version : "1.0.0"
    onRun: {
        console.log('Given input value ' + option('-p') )
    }
}
