# calc.c

Simple (yet quite interesting to build) console calculator written in C.

**Disclaimer**

I dont know neither how to properly build this type of parsing nor how to properly code in plain C, so this is just for fun and exercise.


## Features
- Calculate any math expression of basic operators (`+`, `-`, `*`, `/`) and brackets `()`
- Parser error handling with handy messages

  ![image](https://user-images.githubusercontent.com/33173619/231845754-77abd70a-bd25-470a-9ee1-b1d81746b64e.png)
  
- Console arguments support:
  - `-h`/`--help` - Print help
  - `-v`/`--verbose` - Verbose mode to print intermediate parsing steps:
  
    ![image](https://user-images.githubusercontent.com/33173619/231848126-e87ffdb1-0ad4-4b76-9393-834c38f33af2.png)

  - `-i`/`--interactive` - Interactive mode to enter many expressions in a row
  - `-e`/`--expression` `<expression>` - Enter single expression to calculate
  
## How to use

Just use your terminal of choice and execute program:

```cmd
> .\calc.exe
```

or pass flag `-i` for interactive mode:

```cmd
> .\calc.exe -i
```

Use flag `-h` to view all possible arguments:

```cmd
> .\calc.exe -h
```


### Building

Project uses [xmake](https://xmake.io/) for building, so install it first.

To build, run

```cmd
> xmake build calc
```

To test builded executable run

```cmd
> xmake run calc
```


## Implementation details

Program makes roughly these steps to transform initial string expression and evaluate it:

1. Tokenization: each independent part of the expression (token) is defined and a flat list of these tokens is created.

   For example, if input string was `(1 + 1) / 2`, list of tokens will be `(, 1, +, 1, ), /, 2` (Not just list of substrings but anyway)
  
2. Block parsing: a flat list of tokens is transformed into a tree of (sort of) "blocks", so everything in parentheses will be recursively transformed into a separate list, and at the outer level there will be just a "block" token with a pointer to the list of underlying tokens.
  
   Running program with `-v` flag will represent this step:
  
   ![image](https://user-images.githubusercontent.com/33173619/231855165-bf21694f-a3db-4367-890a-cdaf02b34e0a.png)
   
3. Expression parsing: using a block tree, the program builds a binary tree of operations for each block.
   1. Simple tree building (For block `1 + 1` there will be tree with root node `+`, `1` nodes on left and right branches),
   2. Tree sorting to make sure that multiplication and division come first,
   3. Recursive calls to include inner blocks.

4. Tree evaluation: simplest part, just recursively call left and right branches, if it is a number node, return a number; then add, subtract, multiply or divide based on node operation.

