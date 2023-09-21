# :seedling: Idyllib Webserver

This repository contains a simple C++ webserver that uses very rudimentary frameworks to serve a simple website with little interactivity.
You can use the database and the HTTP parser for your own projects in line with the MIT license.

### Building
Use `make all` to compile the source code into the bin folder.

### Features

* Can respond to GET requests.
* Safe against URL injections.
* Supports up to one connection.
* Uses a simple tabular database for serving files.
