# Deployment Scripts

Deployment scripts are part of Live CVs build, deploy and package process, offering a standardized way to
create packages. If you will look into Live CVs main repository or the tutorial repository, you will see a file named
_livecv.json_ and _live.tutorial.json_ respectively. These files describe the contents of the repository in terms of modules
and dependencies, and the steps required to build, deploy and package the repository with different compilers. The
files are used by Live CV's [deployment kit](https://github.com/livecv/livecv-deploy-kit) in order to determine the
repository configuration. Within a Live CV repository, the scripts expect to find a file that starts with _live_ and
ends with _.json_ extension.

The files are split into the following main sections:

* version: the version of the package
* name: the name of the package
* webpage: url of the package
* dependencies: a list of Live CV dependencies for the package. This does not include third party dependencies (e.g.
opencv, dlib, etc)
* components: the components or plugins inside the package
* releases: a map of releases the the process required to package a release

The _version_, _name_ and _webpage_ are pretty straightforward, so we will go into the other 3.

## Dependencies

_dependencies_ defines a list of livecv packages this current package is dependent on. For each list item, the
following fields are expected:

* name: name of the dependent package
* version: version of the dependent package
* repository: repository for the package

## Components

_components_ defines a map of components this package will export. The component name is specified by its key. The
version of the component is added through the version field:

```
"components" : {
    "tutorial" : {
        "version": "1.0"
    },
    "tutorial.extended" : {
        "version": "1.0"
    }
}
```

## Releases

_releases_ provides a map of items this package can be released to. The form for each release is to provide the key
as the release id, and the following set of values as part of the release object:


* compiler: the compiler id to use. This must be compatible with the qmake compiler naming scheme
* environment: the environment variables required to build and deploy this release. Usually \c{QTDIR} is standard here
* build: a list of steps required to build the application
* deploy: a list of steps required to deploy the application after the build process.

An example of a release for the gcc compiler in the tutorial package is as follows:

```
"gcc_64" : {
    "compiler" : "gcc_64",
    "environment" : {
        "OPENCV_DIR" : "opencv_dir",
        "QTDIR" : "qtdir"
    },
    "build" : [
        {"qmake" : ["-recursive"]},
        {"make" : []}
    ],
    "deploy" : [
        { "copy" : {
            "{release}/bin" : {
                "plugins" : {
                    "tutorial": "plugins/tutorial"
                }
            },
            "{source}/samples/tutorial" : "samples/tutorial"
          }
        }
    ]
}
```

The environment captures our `QTDIR` and `OPENCV_DIR` environment variables, during our build we launch our
_qmake_ and _make_ processes, and in the deployment part we copy the "plugins/tutorial" directory
from the release/bin directory, as well as the "samples/tutorial" directory from our source code. The _copy command
takes a set of keys that define the files or directories to be copied, and a set of relative paths to which the entries
will be copied to in the deployment dir (e.g. `"{source}/samples/tutorial" : "samples/tutorial"` will be copied to
our deployment directory in "samples/tutorial".
