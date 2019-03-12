# Error handling

In LiveKeys, we support several ways of error handling in QML. 

When an exception is thrown, it can be handled locally through the usual try-catch scheme.

```
	try {
		...
		throw linkError(new Error("message"), this)
		...
	} catch(err) {
		// handle err
	}
``` 

If there's no try-catch block, we can provide an ErrorHandler object, by including it as a child of an object for which we 
want to catch exceptions. If an exception is thrown anywhere in the hierarchy of children of that parent, our provided error 
handler will catch it, and stop it from propagating along. Note that the exception has to be linked to the object like in the example above.

```
import QtQuick 2.3
import base 1.0

Item{

	id: root;
	property string errorMessage: 'empty';
	
	ErrorHandler{
		onError:   root.errorMessage   = e.message;
	}
	
	...
	
	ParentItem1 {
		...
		ParentItem2 {
			EngineTestStub{
				Component.onCompleted: {
					throw linkError(new Error("error message"), this)
				}
			}
		}
	}
	
}
```

In the provided example, the ErrorHandler will grab the exception via the e variable, and then we can use some of its properties: `message`, `fileName`,
`lineNumber`, `type`, `code`, `functionName`. The error itself is handled by the `onError` listener.

In case the exception gets handled by the ErrorHandler, it won't propagate further to the engine. However, if we choose to do so, we can simply invoke the `skip` function in our `onError` listener.  

```
	ErrorHandler{
		onError:   skip(e);
	}
```

Now, if our error is minor and/or doesn't affect the program corruptively, we can use a Warning instead. All of the above can be said for warning as well, except the obvious renaming to some functions and handlers. In ErrorHandler, the listener for warnings is `onWarning`.
We can use the same `skip` function to propagate the warning further
```
	ErrorHandler{
		onWarning:   skip(e);
	}
```

