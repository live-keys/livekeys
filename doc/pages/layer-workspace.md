# Workspace Layer

The workspace layer adds full support for a development environment, with a large number of features:

 * Project management: Opening, closing projects
 * Project file system manamgent: Creating, Editing, Deleting files
 * Pane management with various pane types:
    * Code Editor
    * Logging
    * Project view
    * Runnable view
    * Documentation view
    * Minimal Browser
 * Customisable Themes
 * Code editor with highlighting and auto-completion support
 * Customisable Palettes
 * Keyboard shortcuts
 * Commands

The workspace layer is loaded by default when starting livekeys.


## Workspace Layer API

Access to the workspace layer is done via `lk.layers` property: `lk.layers.workspace`

The API is split in the following sections:

 * Project: Project functionality, like closing the project, opening a new file, monitoring file changes.
 * Wizzards: Provides wizzards for users to work with the project such as opening/closing files, etc
 * Panes: Working with panes: splitting, creating, removing different pane types
