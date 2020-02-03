# Plugin 'fs' 

Contains functionality related to the file system.

```
import fs 1.0
```

{plugin:fs}
{qmlSummary:fs}

{qmlType:ListDir}
{qmlInherits:QtCore#Object}
{qmlBrief:Provides contents listing for the folder provided by a path.}

{qmlProperty:string path}

Path to the folder that we're listing.

{qmlProperty:int flags}

Flags to filter items to list.

{qmlProperty:list output}

Content output containing folder items as strings.

{qmlEnum:Flags}

Flags to notify if we're including subdirs and following symlinks. 
Identical to the ones in QDirOperator.

 * `ListDir.NoFlags`
 * `ListDir.Subdirectories`
 * `ListDir.FollowSymlinks`

{qmlSignal:pathChanged()}

Triggered when the path changes 

{qmlSignal:outputChanged()}

Triggered after we've processed the folder and the output is ready

{qmlSignal:flagsChanged()}

Triggered when the flags are changed



{qmlType:FileDescriptor}
{qmlInherits:QtCore#Object}
{qmlBrief:Wraps around multiple file read/write functionalities}

{qmlMethod:isValid()}

Returns an indicator that the file exists and is open.

{qmlMethod:setFile(QtCore#File f)}

Sets the file that we're going to be using

{qmlMethod:close()}

Closes the contained file.

{qmlMethod:seek(int pos)}

Seeks inside the file to a given position

{qmlMethod:write(ArrayBufer content)}

Writes given byte content inside file

{qmlMethod:writeString(string text)}

Writes text inside file (text file or otherwise)

{qmlMethod:read(int numOfBytes)}

Reads a given number of bytes, or the entirety of the file if the parameter is -1


{qmlType:Dir}
{qmlInherits:QtCore#Object}
{qmlBrief:Singleton that provides common directory functionality}

{qmlMethod:list(var path)}

Lists content of directory/folder

{qmlMethod:mkDir(var path)}

Creates a folder with a given path

{qmlMethod:mkPath(var path)}

Creates a folder with a given path, including missing parent folders

{qmlMethod:remove(var path)}

Removes folder given by path, recursively

{qmlMethod:rename(var old, var nu)}

Renames a folder and potentially moves it if the path is different

{qmlType:Path}
{qmlInherits:QtCore#Object}
{qmlBrief:Singleton that provides common path functionality}

{qmlMethod:name(string path)}

Returns filename given the path

{qmlMethod:join(var part1, var part2)}

Joins two strings into a single path

{qmlMethod:exists(var path)}

Returns indicator if the file exists or not

{qmlMethod:baseName(var path)}

Returns filename without extensions

{qmlMethod:suffix(var path)}

Returns the last extension of the file

{qmlMethod:completeSuffix(var path)}

Returns all extensions of the file

{qmlMethod:absolutePath(var path)}

Returns absolute path for a file given by a path parameter

{qmlMethod:isReadable(var path)}

Returns indicator that the file is readable

{qmlMethod:isWritable(var path)}

Returns indicator that the file is writable

{qmlMethod:isExecutable(var path)}

Returns indicator that the file is executable

{qmlMethod:isHidden(var path)}

Returns indicator that the file is hidden

{qmlMethod:isNativePath(var path)}

Returns indicator that the file path can be used directly with native APIs

{qmlMethod:isRelative(var path)}

Returns indicator that the given file path is relative

{qmlMethod:isAbsolute(var path)}

Returns indicator that the given file path is absolute

{qmlMethod:isFile(var path)}

Returns indicator that the given path is pointing to an existing file

{qmlMethod:isDir(var path)}

Returns indicator that the given path is pointing to an existing directory

{qmlMethod:isSymLink(var path)}

Returns indicator that the given path is pointing to an existing symlink

{qmlMethod:isRoot(var path)}

Returns indicator that the given path is pointing to a root directory (e.g. "D:/" on Windows)

{qmlMethod:isBundle(var path)}

Returns indicator that the given path is pointing to a bundle or to a symbolic link to a bundle on macOS and iOS
{qmlMethod:lastModified(var path)}

Returns time when the file was last modified

{qmlMethod:created(var path)}

Returns time when the file was created

{qmlMethod:owner(var path)}

Returns the owner of the file

{qmlMethod:ownerId(var path)}

Returns the owner id of the file on supported systems (non-Windows), otherwise -2

{qmlMethod:group(var path)}

Returns the group of the file on supported systems, otherwise an empty string

{qmlMethod:groupId(var path)}

Returns the group id of the file on supported systems (non-Windows), otherwise -2

{qmlMethod:parent(var path)}

Returns the parent folder of the given file/folder

{qmlMethod:followSymLink(var symLink)}

Returns the absolute path to the file or directory a symbolic link points to, or an empty string if the object isn't a symbolic link.

{qmlMethod:createSymLink(var symLinkPath, var path)}

Creates a symlink or shortcut with a given name to the given path

{qmlMethod:permissions(var path)}

Returns permissions flags for the given path, in the Read/Write/Exe order, per Owner/Group/User, 9 bits total.

{qmlMethod:setPermissions(var path, var val)}

Sets permission flags for the given path, in the Read/Write/Exe order, per Owner/Group/User, 9 bits total.

{qmlMethod:removeSlashes(string s)}

Functions that removes slashes at the start or end of a given string


{qmlType:File}
{qmlInherits:QtCore#Object}
{qmlBrief:Singleton that provides common file functionalities}

{qmlEnum:Flags}

Flags identical to the ones in QFile, to indicate multiple options.

* `File.NotOpen` - file is not open
* `File.ReadOnly` - file is open for reading
* `File.WriteOnly` - file is open for writing only
* `File.ReadWrite` - file is open for both reading and writing 
* `File.Append` - file is open for appending at the end
* `File.Truncate` - file is cleared 
* `File.Text` - file is open as text
* `File.Unbuffered` - file buffers are bypassed  
* `File.NewOnly` - file isn't opened if it already exists, otherwise it's created
* `File.ExistingOnly` - file is only opened if it already exists


{qmlMethod:remove(var path)}

Remove file with the given path

{qmlMethod:rename(var old, var nu)}

Renames the file, and potentially moves it if the parent folder is different for the two paths

{qmlMethod:size(var path)}

Returns the size of the file in bytes

{qmlMethod:open(var path, File.Flags flags)}

Opens the file with the given flags, and returns a FileDescriptor for further handling
