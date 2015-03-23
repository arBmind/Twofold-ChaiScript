# Twofold

**Generate correctly indented source code with the power of ChaiScript and Qt**

This is the second C++ implementation of Twofold.

## What is the best method to generate source code?

* more difficult than regular html. It requires tight controls about every whitespace and tools for indentation.
* often requires more control structures in order to access models or AST-like structures.

We faced these challanges and Twofold is the result.

## Features

* indentation handling
* source maps for generated javascript
  * javascript exceptions and syntax errors will point into your template
* nested source maps for generated targets
  * positions of all the calls that lead to any target character
* control of every generated line and every whitespace
* fully unit and integration tested

## Requirements

* C++11 compiler
  * Tested with GCC 4.9.1 on Linux (Travis) and MinGW (Windows)
  * Tested with Visual Studio 2013 Community Edition Compiler (x86 and amd64)
  * Tested with Clang 3.4 on Linux and Clang 3.6 on Windows
  * Other compilers might work, but are not currently tested
* Qt 5.4.x
  * Tested with the binary distributions of Qt 5.4.1 on Linux and Windows
  * Other Qt versions might work, but are not currently tested

## How does it work?

Twofold is basically Javascript that is extended with constructs to generate source code output.

```twofold
  \ indented output #{interpolated} without a line break
  | indented output #{interpolated} with a line break
  # include "indent_the_included_file.twofold"
  = change_indentation_of_javascript()
// every line is regular javascript
```

These control characters have to be the first non-whitespace character on each line.
Every whitespace before them is ignored.
Every whitespace after is used as the indentation. Indentation is cumulative and added to each generated line.

**Hint:** Use the whitespaces before control characters to indent them in a column deeper than any surrounding javascript.
This gives you a visual splitter.

This is basically everything you need to know.

## Example Template

```twofold
// file included.twofold
def methodArgs(args) {
  args.for_each_with_index(fun(arg, i){
    if (0!=i) {
        // use interpolation to preserve the whitespace
        \#{", "}
    }
    // use backslash to avoid linebreaks
        \#{arg}
  });
}
def showMethod(method) {
        |function #{method.name}(#{methodArgs(method.args)}) {
        |  #{method.body}
        |}
}
```

```twofold
// file main.twofold
#include "included.twofold"
        |function #{name}Class(#{methodArgs(args)}) {
methods.for_each_with_index(fun(method){
        =  showMethod(method)
});
        |
        |  return {
methods.for_each_with_index(fun(method, i){
        |    "#{method.name}": #{method.name}#{(i+1 < methods.size) ? "," : ""}
});
        |  };
        |}
```

If we run this template against the following data. See the BasicUsage example.

```json
{
"name": "TwofoldGenerated",
"args": [],
"methods": [
  {
  "name": "hello",
  "args": ["greeted"],
  "body": "console.log('Hello ' + greeted);"
  }
]
}
```

Twofold will generate the following output.

```javascript
function TwofoldGenerated() {
  function hello(greeted) {
    console.log('Hello ' + greeted);
  }

  return {
    "hello": hello
  };
}
```

## Roadmap

Twofold is simple. We find it very useful as it is.

* feel free to add your wish as an issue

## License

Apache License Version 2.0
See LICENSE file for more details

## Contribute

If you like this project give it a star.
If you don't like it or found a bug, please write an issue.
