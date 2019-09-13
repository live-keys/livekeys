# Plugin API

In Livekeys, some plugins are also treated as libraries for other C++ plugins. Plugins communicate with each other 
through common types, therefore it's important that a plugin is able to understand the type sent by another plugin.

Only some plugins export their types, as most types are made common through available libraries:

 * [live](lib_plugin-live.md) plugin.
 * [lcvcore](lib_plugin-lcvcore.md) plugin.
 * [lcvfeatures2d](lib_plugin-lcvfeatures2d.md)  plugin.
 * [lcvvideo](lib_plugin-lcvvideo.md) plugin.