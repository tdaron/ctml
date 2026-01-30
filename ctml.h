/* ctml - v1.0.0 - https://github.com/tdaron/ctml

This library is a small (~125 SLoC of C) HTML templating engine.
CTML is macro-based as the objective is to provide a nice to use 
DSL inside of the C programming language. (however it does not hide
anything of the control flow).


# Features
- Strongly typed tag attributes 
- Directly embeded inside C code
- Really lightweight
- Not any dependencies (not even libc)
- C functions as components

As this library is a header-only library, at (only) one place
in your code you need to define `CTML_IMPLEMENTATION` to include
the implementation of the library.


# Quick Example
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

# How to use

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

# Components

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

# Configuration
This library can be configured using some macros. 

*CTML_PRETTY* will enable pretty print of the HTML. 
*CTML_NOLIBC* will disable all libc dependent stuff (only 
the FTEXT macro)
*CTML_CUSTOM_FIELDS* as explained in [Custom Attributes](#Custom Attributes). 

Those macros must be defined BEFORE including `ctml.h` and must be 
repeated each time you inclde it (especially CTML_CUSTOM_FIELDS). 
Creating your own header defining your ctml settings is recommended.

```c
#include "ctml_config.h"
#include "ctml.h"
```

# Custom Attributes

As seen in the example, some attributes are supported by default
like `class` or `id` but one might want to use custom/other
attributes that are not already defined inside the library. 

A special macro `CTML_CUSTOM_FIELDS` exists to let you add those
to the library. It must be used **before** including `ctml.h` and
shall be repeated whenever you include it. 

It is based on X-macros and let you add attributes this way:

```c
#define CTML_CUSTOM_FIELDS X(checked);X(color);XL(da, data-attr);
```

This adds attributes `checked` and `color` to the engine.
You can also use `XL` to define shorthands. Like `da` that will
be replaced by `data-attr` inside of the generated HTML. (This
`XL` macro is also useful for attributes names that are not valid
C variable names, like `data-attr` because of the dash.

# Shorthands

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



 
*/
#ifndef CTML_H
#define CTML_H


#define FIELDS   \
	X(class) \
	X(style) \
	X(id)    \
	X(lang)  \
	X(src)   \

typedef struct {
	char* tag_name;
	int self_close;
	#define X(field)     char* field;
	#define XL(field, _) char* field;
		FIELDS
	#ifdef CTML_CUSTOM_FIELDS
		CTML_CUSTOM_FIELDS
	#endif
	#undef X
	#undef XL
} CTML_Tag;

typedef void (*ctmlSink) (char*);

typedef struct {
	ctmlSink sink;	
	int indent;
} CTML_Context;


#ifdef CTML_PRETTY
	void ctml_indent(CTML_Context* ctx, int indent);
#endif

void ctml_open_tag(CTML_Context* ctx, CTML_Tag* tag);
void ctml_close_tag(CTML_Context* ctx, CTML_Tag* tag);

// TODO: Maybe find a way to clean up those CONCAT ?
#define CONCAT1(a, b) a##b
#define CONCAT(a, b) CONCAT1(a, b)
#define ctml_tag(n, ...)                                               \
	CTML_Tag CONCAT(_tag_, __LINE__) = {.tag_name=#n, __VA_ARGS__};			\
	ctml_open_tag(ctx, &CONCAT(_tag_, __LINE__));                                         \
	for (int _once = 0; _once < 1; _once=1,ctml_close_tag(ctx, &CONCAT(_tag_, __LINE__))) \

#define h(n, ...) ctml_tag(n, __VA_ARGS__)
#define hh(n, ...) ctml_tag(n, .self_close=1, __VA_ARGS__)

#define ctml(...) CTML_Context ctml_context = (CTML_Context) {__VA_ARGS__}; \
		  CTML_Context* ctx = &ctml_context;


#ifndef CTML_NOLIBC

	#ifndef CTML_BUF_SIZE
		#define CTML_BUF_SIZE 1024
	#endif

	char tmpbuf[CTML_BUF_SIZE];
	#ifdef CTML_PRETTY
		#define FTEXT(...)                                     \
			snprintf(tmpbuf, CTML_BUF_SIZE, __VA_ARGS__);  \
			ctx->sink(tmpbuf);                          

	#else
		#define FTEXT(...)                                     \
			snprintf(tmpbuf, CTML_BUF_SIZE, __VA_ARGS__);  \
			ctx->sink(tmpbuf);                          
	#endif // CTML_PRETTY

#endif // CTML_NOLIBC

#ifdef CTML_PRETTY
	#define TEXT(t) ctml_indent(ctx, ctx->indent+1);ctx->sink(t);ctx->sink("\n");
#else
	#define TEXT(t) ctx->sink(t);
#endif // CTML_PRETTY



#ifdef CTML_IMPLEMENTATION

#ifdef CTML_PRETTY
	void ctml_indent(CTML_Context* ctx, int count) {
		for (int i = 0; i < count; i++) {
			ctx->sink(" ");
		}
	}
#endif // CTML_PRETTY


void ctml_open_tag(CTML_Context* ctx, CTML_Tag* tag) {
	#ifdef CTML_PRETTY
		ctml_indent(ctx, ctx->indent);
		ctx->indent++;
	#endif
	ctx->sink("<");
	ctx->sink(tag->tag_name);

	#define X(field)                                  \
		if (tag->field != 0) {                 \
		        ctx->sink(" " #field);          \
		        ctx->sink("=\"");               \
		        ctx->sink(tag->field);          \
		        ctx->sink("\"");                \
		}                                         
	#define XL(field, lname)                          \
		if (tag->field != 0) {                 \
		        ctx->sink(" " #lname);          \
		        ctx->sink("=\"");               \
		        ctx->sink(tag->field);          \
		        ctx->sink("\"");                \
		}                                         
	FIELDS
	#ifdef CTML_CUSTOM_FIELDS
		CTML_CUSTOM_FIELDS
	#endif
	#undef X
	#undef XL

	if (!tag->self_close){
		ctx->sink(">");
	} else {
		ctx->sink("/>");
	}
	#ifdef CTML_PRETTY
		ctx->sink("\n");
	#endif
}

void ctml_close_tag(CTML_Context* ctx, CTML_Tag* tag) {
	if (tag->self_close) return;
	
	ctx->indent--;

	#ifdef CTML_PRETTY
		ctml_indent(ctx, ctx->indent);
	#endif

	ctx->sink("</");
	ctx->sink(tag->tag_name);
	ctx->sink(">");

	#ifdef CTML_PRETTY
		ctx->sink("\n");
	#endif
}

#endif // CTML_IMPLEMENTATION

#endif // CTML_H
