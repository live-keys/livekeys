# Palettes in editqml

Palettes contain user interfaces that allow editing certain parts of a qml document opened inside an editor.For example, sliders that are used to edit a numerical property value in code are called palettes.

Palettes work together with edit fragments, which allow editing declarations in code. Palettes can also be combined to form hierarchies of the declared object tree inside a qml document.

This page will describe the API behind palettes. 

## Document structure

To understand the API, we must first look at how a qml document
is structured:

```
imports

object
   properties...
   eventListeners...
   objects...
      ...
```

The document has an imports section, and an object tree. The object tree starts off with a root object, and continues to declare properties, eventListeners and children for that object. Children are also objects that can further declare properties and children of their own.

Based on the above, we can have the following 4 types of declarations:

 * imports: describes all the imports inside the document
 * object: describes a declared object in the document
 * property: describes a declared properyt in the document
 * event listener: describes a declared event listener in the document

For each declaration, the API can provide an editing fragment which can be used to edit that declaration. And for each editing fragment, we can add one or more palettes, depending on the type of the fragment.

# API Objects

There are 2 main objects used to access the API for a qml document:

 * `PaletteFunctions` provides a higher level API to create palettes and UIs for object trees.
 * `LanguageQmlHandler` has a lower level API that works more directly with the document (parser, type and imports info, direct edit fragment management). 

As a general rule, unless something really custom is needed, the `PaletteFunctions` object covers the functionality behind palettes.

On top of these 2 main objects, there's the `QmlEditFragment` object, which is created through `PaletteFunctions` or `LanguageQmlHandler`, and which manages palettes, editing the declaraiton in code and whithin the application, and handles bindings between expressions.

The `PaletteFunctions` object can be captured from the `editqml` extension, while the `LanguageQmlHandler` is created for each editor instance:

```js
var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
var languageHandler = currentEditor.code.language
```

The APIs for each object have been added to their own sections:

 * PaletteFunctions
 * LanguageQmlHandler
 * QmlEditFragment

