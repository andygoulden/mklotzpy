# Notes on Converting from C for DOS to Python

This document contains notes to help convert these utilities from C for DOS to Python

## Math functions

Due to the nature of machine shop calculations, a lot of these scripts use mathematical functions like logarithms and trigonometry.

Python has two main options for mathematical function libraries: the built-in 'math' library, and Numpy.

Numpy has a number of advantages over the built-in math library, such as providing its own types with fixed width, and trigonometric functions that take degrees (rather than radians).

However, for the sake of portability, I'm using the built-in library for now. This project is aimed at making these utilities easier to run on as many systems as possible, so requiring an additional package which isn't strictly necessary seems antithetical to the whole point of this project.

## The `vin()` function

A number of the utilities use a function called `vin()`, which is not a standard C function. To the best of my knowledge, this is a user-defined function that Marv created. It gives the user a prompt to enter some text, and provides a default value. If the user presses enter without typing any characters, then the default value is used.

Unfortunately, I can't find the source of it anywhere. However, the declaration is as follows:

```
void vin (char *prompt, void *x, char *format, dbl def, char *udef);
```

The parameters are as follows:

* **prompt** - A string printed to the user, prompting them to enter something.
* **x** - A variable to set with the result of this prompt.
* **format** - A formatting specifier for the default value, adhering to the syntax of `printf` format specifiers.
* **def** - The default value for the variable. This is printed to the user in square brackets, and is assigned to the variable if the user doesn't enter anything.
* **udef** - The unit for the default value. This doesn't have any effect on the function of the code, but is presented next to the default value in the square brackets.

Here is an example call to the `vin()` function, prompting the user to enter a thread pitch in millimetres:

```
vin ("Thread pitch",&tpi,"lf",1.,"mm");
```

This prints the following to the console:

```
Thread pitch [1 mm] ? 
```

If the user presses enter without typing anything else, the variable `tpi` is assigned with the value `1`. If the user enters a number, that is assigned to the variable `tpi`.

Here is a Python function call which is equivalent to the above:

```
tpi = float(input("Thread pitch [1 mm] ? ") or 1)
```

If you wanted to force the default value in the prompt to be the same as the actual default value (which is probably good practice), you could instead do the following:

```
default_tpi = 1
tpi = float(input(f"Thread pitch [{default_tpi} mm] ? ") or default_tpi)
```

This uses f-strings, which I prefer over the older methods for string formatting in Python. If you wanted to support versions of Python older than 3.6, then you could use the `.format()` function instead.

## Global variables

It's very common in C to use global variables, ie variables which are defined outside of any functions.

However, in Python, it's best to avoid this. One exception to this is things which you want to be globally available. For example, trigonometric functions in these utilities tend to supply angles in degrees, but the built-in math functions take radians. As such, I'm adding a global variable named `RPD` (radians per degree), which is the same as Marv uses in his `SIND`, `COSD` and `TAND` functions, where needed.

In future, it would be better if this were defined at the package level (ie inside `__init__.py` in `mklotzpy`), which the scripts would then all share. For now, I'm keeping each script the way Marv had them, which is to be executable on their own.

In places where Marv uses a global variable, if it's not something actually global (like the ratio of radians to degrees), define it inside a function. For the examples I've done so far, each script has a `run()` function, which is comparable to a `main()` function. That's the best place to initialise any values which are needed elsewhere in the script.
