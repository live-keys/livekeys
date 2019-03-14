# Logging

LiveKeys supports complex logging schemes through the use of various options. What makes it special is the possibility of logging _visuals_ as well, 
images in particular. We have a globally available configuration (that starts out as a pre-defined default configuration) that can be modified via command line arguments or during runtime, which will be explained gradually. On top of the "global" configuration, we can also use our own specific named configurations which we'll refer to as _tags_. The configurable options are as follows:
 * `level` - In the application, this represents the least level a message should have in order to be displayed. 
 * `defaultLevel` - This represents the default level of a message if not specified otherwise.
 * `file` - In case we're outputting the log to a file, this parameter represents the file path
 * `logDaily` - Indicator that we should output a daily log
 * `toConsole` - Indicator that the log entries should be outputted to console
 * `toExtensions` - Indicator that the log entries should be passed to other transports
 * `toView` - Indicator that the log entries should be passed to LiveKeys' visual logger
 * `logObjects` - Flags collection to represent the output 
 * `prefix` - Formatted message prefix (see below)

## Log levels

Log levels are the following, in order of importance:
 * `Fatal` - Means the program should halt at this point, since the error is irrecoverable and potentially harmful.
 * `Error` - Means the program entered a faulty state and should end. 
 * `Warning` - A potentially harmful mistake that should be rectified, but the program will run nonetheless.
 * `Info` - Harmless message to inform the user of any internals that should be relevant to him
 * `Debug` - To be used for internal purposes in order to e.g. check the state of variables and function calls 
 * `Verbose` - Very detailed messages that could, for example, include the time stamp, locations etc.

In the default configuration, the application message level is `Debug`, while the default message level is `Info`.

In Qml, an example logging of a message would like this:

```qml
	vlog.i("Test message")
```
## Command line arguments

We can separate logging command line arguments into those which don't need an argument (flags) and those that do.
We have the following flags:
  *	`--log-toconsole (short version: -c)`   
	Setting this flag enables the logger to output messages to the console (this is enabled by default)
   
  * `--log-noview`  
	Setting this argument will disable logging to the LiveKeys interface.
	
  * `--log-daily`  
	Setting this flag will create a daily log file, using the log date format set for LiveKeys (see the prefixes section)
	Uses the --log-file argument for us to specify the path of the file(s) where we will store the log files.
	The filename would be something like 
	```
		path/to/logfile_%Y_%m_%d.txt
	```

The other arguments demand an extra string argument, so they will be considered in greater detail.

  *	`--log-level <level>`   
	Sets up the application log level as mentioned above. Can be given by the string of the wanted level
	```
	--log-level "Verbose"
	--log-level "Error"
	...
	```

	
  * `--log-tonetwork <url>`  
	Send log data to the given network url through TCP/IP.
	
  * `--log-file <path> (short version: -o <path>)`  
	Outputs the log data to the file given by the path.

  * `--log-config-file <path>`  
	JSON file containing log parameter values for one or several configurations. Each configuration is given by its name and an object containing other values, like in the following example. Our global configuration can be configured simply by using the name "global".
	```
	[
		{"global": {
			"level": "Verbose",
			"defaultLevel": "Info",
			...
		}},
		{"tag1": {
			"level": "Info",
			"defaultLevel": "Error",
			...
		}}
	]
	```
  * `--log-config <string>`  
	Config string is given as an array of semicolon-separated strings in the form of `<option>=<value>` or `<tag>:<option>=<value>`. In the first variant, the tag defaults to "global". If used, this overrides all other given arguments, since we assume this is where the tag(s) will be configured completely. 
	```
		--log-config "level=Info;defaultLevel=Verbose;tag1:toView=true;tag1:level=Debug"
	```
	
  * `--log-prefix <string>`  
    We offer a variety of predefined prefix patterns. We can include any of the following in the string, to be expanded:
	* `%p` - a very verbose pattern, including the potential remote, detailed timestamp, message level, function name and line number.  
	An example log message is given below.
	
	```
		remote> 2019-03-04 21:34:28.036 error expression for onObjectCreationError@1036: Error: :6 Syntax error
	```  
	* `%r` - remote
	* `%F` - full file path
	* `%N` - file name
	* `%U` - function name
	* `%L` - line numuber
	* `%V` - message level
	* `%v` - lowercase message level
	* `%w` - short day name (e.g. Mon) 
	* `%W` - long day name	
	* `%b` - short month name
	* `%B` - long month name
	* `%d` - day number, zero-padded to two digits e.g. 02, 10, 31
	* `%e` - day number, without modifications
	* `%f` - day number, padded to two spaces e.g. _2, _4, 10
	* `%m` - month number, zero-padded to two digits
	* `%n` - month number, without modifications
	* `%o` - month number, padded to two spaces
	* `%y` - last two digits of the year
	* `%Y` - year
	* `%H` - hour, zero-padded to two digits
	* `%I` - hour by the 12-hour clock convention, zero-padded to two digits 
	* `%a` - am/pm (lowercase)
	* `%A` - AM/PM (uppercase)
	* `%M` - minute, zero-padded to two digits
	* `%S` - second, zero-padded to two digits
	* `%s` - second with milliseconds, zero-padded to two and three digits respectively (e.g. "02.031")
	* `%i` - milliseconds, zero-padded to three digits
	* `%c` - tenths of seconds 

Any other character will be copied as is. 
We can create our own prefix such as 
```
	--log-prefix "my remote %r at %I%A >" 
	--log-prefix "error at line %L in file %N _________"
	...
```

### vlog

`vlog` is our default object that we use for logging throughout LiveKeys. It is defined in such a way that it always fetches 
the function, line number and file where it's inserted, and we can use it as a parametrized object, similar to cout or console.log.
The most usual parameter for vlog is the tag name e.g. `vlog("tag1")` would use the configuration associated with the "tag1" during logging. 
As mentioned above, we can define the level of a message by appending an appropriate function call to the `vlog` object, e.g. `vlog("tag1").d()`.
We can also configure if as follows
```
	vlog.configure("test", {
        "level": VisualLog.Error,
        "defaultLevel": VisualLog.Info}
  });
 ```
 
Messages are logged by using the `<<` operator, as seen in several instances above.

```
    vlog("tag1", "Debug message for tag1")
```

The use of `vlog` in LiveKeys is slightly different - we use it as an object, define the log level similarly, with the level functions called, and
the arguments included in parentheses:

```
	vlog.d("message1")
    vlog.e("Error")
```
 
### Log tags

As mentioned previously, tags are fundamentally just a combination of parameters saved under a single name (the tag name).  
Tags are created immediately when we use their name. For example, even if it didn't exist before, a call to `vlog("tag1")` or `vlog().configure("vlog1", {...})` would create the configuration under the name "tag1" that would inherit from the default one and build upon that. 
In the `--log-config` command line argument explained above, we've shown a way to initialize a tag in the command line, by setting its parameters.  
A tag can additionally be configured by the `vlog(...).configure()` function.  

### Visual logging

An interesting feature of our log is the ability to log images as well! They have to be in a QMat form (see documentation of QMat), a matrix representation of the image. The usual source is an ImRead (see documentation).   

![example of visual logging](images/visual-log.png)

In the image above, we note the ImRead object, whose output can be displayed. It can be used as a object to be logged (note the `vlog` call), with the result available below, in the log window. 

### Log window

LiveKeys has a log window which can be opened either by using the `Ctrl+L` shortcut or clicking on the log icon ![log icon](images/logicon.jpg). 
This opens the log window where we can see all of our logged messages. In the header, we are given several options:
* The largest input box can be used to search through the log messages.   
* The first button opens an additional search box which can be used to search for a particular tag.  
* The second button opens a sidebar where each message's prefix is displayed. The sidebar also contains a search box for prefixes.
* The third button opens the logger in a separate window, or returns it to the main window if it's already separate. 

