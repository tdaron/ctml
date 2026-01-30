# CTML

This library is a small single header file (~125 SLoC of C) HTML templating engine
for the C programming language.

CTML is macro-based as the objective is to provide a nice to use 
DSL inside of the C programming language. (however it does not hide
anything of the control flow).


## Features

- Directly embeded inside C code
- Really lightweight
- Not any dependencies (not even libc)
- C functions as components

As this library is a header-only library, at (only) one place
in your code you need to define `CTML_IMPLEMENTATION` to include
the implementation of the library.


## Quick Example

### With shorthands
```c
#include <stdio.h>
#define CTML_PRETTY
#define CTML_IMPLEMENTATION
#include "ctml.h"
#include "ctml_short.h"

int main() {
	ctml(
		.sink=(ctmlSink)printf
	) {
		html(.lang="en") {
			div(.class="nice") {
				t("hello, world");
			}
		}
	}
}
 
```

### Without shorthands
```c
#include <stdio.h>
#define CTML_PRETTY
#define CTML_IMPLEMENTATION
#include "ctml.h"

int main() {
	ctml(
		.sink=(ctmlSink)printf
	) {
		h(html, .lang="en") {
			h(div, .class="nice") {
				TEXT("hello, world");
			}
		}
	}
}
 
```

This code will print this html on stdout:
```html
<html lang="en">
  <div class="nice">
   hello, world
 </div>
</html>
```

NOTE: The sink function is a void function taking char* as input,
where ctml will send the generated HTML.
ctml will send data in multiple times and not only once with the 
full generated HTML. 

The sink function can also take `char*, void* userData` with user data
given to `ctml()` using `.userData = ...`

## Usage

The api of the library is really simple. It only consists of 3(4) 
macros and 1 type. 

The first macro is `h(tag, attributes*)` that is used to create
an HTML tag. 

You'll also need the `ctml` macro that will create a context 
containing the sink as well as the indentation state. 

Two last macros allow you to put text inside of the HTML. 
The first one is `TEXT(some text here)` that is pretty self
explanatory, and the second one is `FTEXT`, accepting formating
like `printf`. (NOTE: FTEXT is the only libc-dependant feature). 

The only type you should care about is `CTML_Context` as explained
in the [Components](#Components) section.

## Components

Creating components boils down to just calling C functions. The
only requirement is for you to pass the ctml context across components
this way:

```c
void some_button(CTML_Context* ctx) {
	h(button, .class="my-btn") {TEXT("click me");}
}

void my_ui() {
	ctml(.sink=...) {
		h(div) {
			// NOTE HERE THE CTX
			some_button(ctx);
		}
	}
}

```

The `ctx` variable is created by the `ctml` macro. You just need
to pass it around.

## Configuration
This library can be configured using some macros. 

*CTML_PRETTY* will enable pretty print of the HTML. 
*CTML_NOLIBC* will disable all libc dependent stuff (only 
the FTEXT macro)
*CTML_CUSTOM_ATTRIBUTES* as explained in [Custom Attributes](#custom-attributes).

Those macros must be defined BEFORE including `ctml.h` and must be 
repeated each time you inclde it (especially CTML_CUSTOM_ATTRIBUTES).
Creating your own header defining your ctml settings is recommended.

```c
#include "ctml_config.h"
#include "ctml.h"
```

## Custom Attributes

As seen in the example, some attributes are supported by default
like `class` or `id` but one might want to use custom/other
attributes that are not already defined inside the library. 

A special macro `CTML_CUSTOM_ATTRIBUTES` exists to let you add those
to the library. It must be used **before** including `ctml.h` and
shall be repeated whenever you include it. 

It is based on X-macros and let you add attributes this way:

```c
#define CTML_CUSTOM_ATTRIBUTES X(checked);X(color);XL(da, data-attr);
```

This adds attributes `checked` and `color` to the engine.
You can also use `XL` to define shorthands. Like `da` that will
be replaced by `data-attr` inside of the generated HTML. (This
`XL` macro is also useful for attributes names that are not valid
C variable names, like `data-attr` because of the dash.

## Shorthands

Writing `h(tag, attributes)` can also be replaced by `tag(attributes)`
if you also include the file `ctml_short.h`.

This is a file that only declare aliases to make the code more
readable. For instance, here is a small snippet using those:

```c
div(.id="truth") {
	h1(.class="ctml") {
		t("ctml is great");
	}
}
```

NOTE: t() is an alias for the TEXT() macro.
