# ORAM

ORAM is an open-source library that aims to support multiple implementation of Oblivious Random Access Machines (ORAM) algorithms.

[![Build Status](https://travis-ci.org/rogerioacp/oram.png?branch=master)](https://travis-ci.org/rogerioacp/oram)


## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Contributing](#contributing)

<a name="features"></a>
## Features

The main feature of this library is to have a common library for different implementation to create not only a common development framework but also testing and evaluation. The goal is to completely decouple an ORAM algorithm implementation from an application-specific details with a set of common APIS. The API will provide the ORAM implementation access to the external world of an application and the API implementation will handle the details of writing to a file, position map or stash. The only task of the ORAM implementation is ensuring that the access to a file blocks are oblivious. The current APIS used by the an ORAM algorithm can be found on the header files in src/include.

Currently, the library support the following algorithms:
* [PathORAM](https://eprint.iacr.org/2013/280.pdf)

Furthermore, it provides in-memory implementation of a file, stash and position map used for testing the logic of the algorithm implementations.

<a name="installation"></a>
## Installation

### Prerequisites

The library has been successfully tested and installed on Linux (Ubuntu) and Mac OS. The library uses the GNU building system got generate makefiles and compilation objects specific to the underlying building system and as such requires the Autotools to be available. Furthermore, it uses the glib library which is used to create stash based on a linked list.

* [Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html#Autotools-Introduction)
* [glib](https://developer.gnome.org/glib/stable/)

### Installing

To install the library please download a stable release.
The release must be decompressed and installed using the standard GNU build system commands:

> ./configure && make && make install

These commands ensure that the library is correctly compiled in your system and installed on the appropriate folders.

### Running Tests

Before installing the library in the system it is advisable to run a few tests to exclude some simple problems.

> make check

<a name="contributing"></a>
## Contributing

1. File an issue to notify the maintainers about what you're working on.
2. Fork the repo, develop and test your code changes, add docs.
3. Make sure that your commit messages clearly describe the changes.
4. Send a pull request.

### File an Issue

Use the issue tracker to start the discussion. It is possible that someone
else is already working on your idea, your approach is not quite right, or that
the functionality exists already. The ticket you file in the issue tracker will
be used to hash that all out.

### Fork the Repository

Be sure to add the relevant tests before making the pull request. Comment the code as necessary to make the implementation accessible and add the original paper describing the algorithm implementation.


### Make the Pull Request

Once you have made all your changes, tests, and updated the documentation,
make a pull request to move everything back into the main branch of the
`repository`. Be sure to reference the original issue in the pull request.
Expect some back-and-forth with regards to style and compliance of these
rules.

