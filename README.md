# one-draft-in-c

A attempt to rewrite one language compiler in pure c without lexer and parser generators.

## Examples

```c
pow(int a, int b) {
    _ 456
}
main ()  {
    _ "Hello, World"
}
```

Output:

```
Define function pow
    ARG:variable a with a type
    ARG:variable b with b type
  PRINT
    VALUE:16,number
Define function main
  PRINT
    VALUE:14,string double
```

-----


```c
multiPrint() {
    _ 'g'
    _  1, 5 , 150, +50, -45, "hi"
}
```

Output:

```
Define function multiPrint
  PRINT
    VALUE:15,string single
  PRINT
    VALUE:16,number
    VALUE:16,number
    VALUE:16,number
    VALUE:16,number
    VALUE:16,number
    VALUE:14,string double
```

------


```c
multiPrint() {
    _ 'g';_  1, 5 , 150, +50, -45, "hi"
}
```

Output:

```
Define function multiPrint
  PRINT
    VALUE:15,string single
  PRINT
    VALUE:16,number
    VALUE:16,number
    VALUE:16,number
    VALUE:16,number
    VALUE:16,number
    VALUE:14,string double
```

------


```c
int _underline_allowed_in_variable_ident=4;
main() {
    _ 1;
}
```

Output:
```
Define global variable _underline_allowed_in_variable_ident
    VALUE:16,number
Define function main
  PRINT
    VALUE:16,number
```

------


```c
main() {
    if 1 {
        _ 1;
    }
}
```

-----

```c
pow(int a,int b) {
    _ "."
}
main ()  {
    _ "Hello, World"
}
```

Analyze Log:

```
------- ident 'pow'
Define function pow
-->ARG:variable a with a type
-->ARG:variable b with b type
==>STATEMENT:0,print
==>STATEMENT:14,string double
------- ident 'main'
Define function main
==>STATEMENT:0,print
==>STATEMENT:14,string double
```

Output (`$ gcc main.c -o one && ./one input.txt`):

```
Define function pow
    ARG:variable a with a type
    ARG:variable b with b type
  PRINT
    VALUE:14,string double
Define function main
  PRINT
    VALUE:14,string double
```

Output:
```
Define function main
  IF
    VALUE:16,number
  PRINT
    VALUE:16,number
```

