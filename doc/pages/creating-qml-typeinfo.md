# Creating qml typeinfo


For shaping and other UI related features to work, livekeys collects information about qml types in the background.
This information is collected in 2 ways. One is by parsing the actual qml files the types
are declared in, and the other is by reading a file named `plugins.qmltypes` at the root of the plugin. The file describes
types that are made available through compiled libraries, that would otherwise require to load the library into memory
for the types to become available.

Some newer plugins might be missing the `plugins.qmltypes` file, or it can be outdated as new types are added. To update this,
users can run livekeys manually on a specified plugin to get the file.

There's a script available in `editqml` plugin called `plugininfo` that will output these types:

```
./livekeys --global editqml/plugininfo <name_of_plugin>
```

The `--global` argument tells livekeys to look into it's installation directory for the specified script, The name of
the plugin can be any plugin from the installed ones:

```
livekeys --global editqml/plugininfo base
```

The above command will output tyep information about the `base` plugin in the command line. You can pipe it directly to
the desired plugin:

```
./livekeys --global editqml/plugininfo base > plugins/base>
```

### Note if you're compiling livekeys manually

Due to versioning conflicts in some versions of Qt, the extraction of these types is disabled by default. You can enable
it by setting the `ENABLE_PLUGINTYPES` flag to `true` in `lveditqmljs.pro`.
