/* ctml - v1.0.0 - https://github.com/tdaron/ctml

This library is a small (~125 SLoC of C) HTML templating engine.
CTML is macro-based as the objective is to provide a nice to use 
DSL inside of the C programming language. (however it does not hide
anything of the control flow).


# Features
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
*CTML_CUSTOM_ATTRIBUTES* as explained in [Custom Attributes](#Custom Attributes). 

Those macros must be defined BEFORE including `ctml.h` and must be 
repeated each time you inclde it (especially CTML_CUSTOM_ATTRIBUTES). 
Creating your own header defining your ctml settings is recommended.

```c
#include "ctml_config.h"
#include "ctml.h"
```

# Custom Attributes

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

// This is the X macro containing attributes.
// Instead of adding attributes here, they can
// also be provided using the CTML_CUSTOM_ATTRIBUTES
// macro. 
#define ATTRIBUTES   \
	X(class) \
	X(style) \
	X(id)    \
	X(type)  \
	X(lang)  \
	X(src)   \


/*CTML_Tag structure. Looks like this:

typedef struct {
	char* tag_name;
	int self_close;

	char* class;
	char* id;
	char* lang;
	... for all attributes and custom attributes.
} CTML_Tag;

Every call to h() or hh() macro ends up creating a tag. 

*/
typedef struct {
	char* tag_name;
	int self_close;
	#define X(field)     char* field;
	#define XL(field, _) char* field;
		ATTRIBUTES
	#ifdef CTML_CUSTOM_ATTRIBUTES
		CTML_CUSTOM_ATTRIBUTES
	#endif
	#undef X
	#undef XL
} CTML_Tag;


/*
 * CTML_Context structure
 * This one contains the sink (where the generated HTML is sent)
 * and the indent state for pretty printing the output if enabled.
*/

// TODO: move tmpbuf here to be thread safe

typedef void (*ctmlSink) (char*, void* userData);
typedef struct {
	ctmlSink sink;	
	void* userData;
	int indent;
} CTML_Context;


#ifdef CTML_PRETTY
	void ctml_indent(CTML_Context* ctx, int indent);
#endif

void ctml_open_tag(CTML_Context* ctx, CTML_Tag* tag);
void ctml_close_tag(CTML_Context* ctx, CTML_Tag* tag);

// Those CONCAT macros are used to generate unique names for created tag
// like this: __tag__32 with 32 the __LINE__ number where h() or hh() macros
// are used. This is the only macro sorcellery of this library.
#define CONCAT1(a, b) a##b
#define CONCAT(a, b) CONCAT1(a, b)

// ctml_tag is THE macro of the library using the for() trick.
// basically it does 3 things
// 1. Create a CTML_Tag
// 2. Call ctml_open_tag() to generate the opening of it (like <div class="toto"> )
// 3. Start a for loop that will be executed once and then call ctml_close_tag to generate
//    the closing tag. (like <div/>)
// NOTE: The for loop does not have brackets, because the inside of the tag will be defined inside the
// user code like this
//
// h(div) { <-- BEGINNING OF THE FOR LOOP (will call ctml_open_tag)
//         
//      t("tata");
//	t("toto");
//	
// } <-- END OF FOR LOOP - will call ctml_close_tag for the dib
// 
#define ctml_tag(n, ...)                                               \
	CTML_Tag CONCAT(_tag_, __LINE__) = {.tag_name=#n, __VA_ARGS__};			\
	ctml_open_tag(ctx, &CONCAT(_tag_, __LINE__));                                         \
	for (int _once = 0; _once < 1; _once=1,ctml_close_tag(ctx, &CONCAT(_tag_, __LINE__))) \


// Those macros are used by the user.
// h is the basic macro to create tags and hh is a macro to create a self closing tag.
#define h(n, ...) ctml_tag(n, __VA_ARGS__)
#define hh(n, ...) ctml_tag(n, .self_close=1, __VA_ARGS__) {}

// simple macro to create the context
// TODO: Maybe find a way not to have this "hidden" ctx the user can use ?
#define ctml(...) CTML_Context ctml_context = (CTML_Context) {__VA_ARGS__}; \
		  CTML_Context* ctx = &ctml_context;

// Implementation of FTEXT that is basically
// just a snprintf inside a temp buffer.
// The size of the buffer can be changed using the
// macro CTML_BUF_SIZE (default: 1024 bytes)
// TODO: What happens if the buffer is too small. A warning might be printed.
#ifndef CTML_NOLIBC

	#ifndef CTML_BUF_SIZE
		#define CTML_BUF_SIZE 1024
	#endif

	char ctml_tmpbuf[CTML_BUF_SIZE];
	#ifdef CTML_PRETTY
		#define FTEXT(...)                                     \
			snprintf(ctml_tmpbuf, CTML_BUF_SIZE, __VA_ARGS__);  \
			ctx->sink(ctml_tmpbuf, ctx->userData);                          

	#else
		#define FTEXT(...)                                     \
			snprintf(ctml_tmpbuf, CTML_BUF_SIZE, __VA_ARGS__);  \
			ctx->sink(ctml_tmpbuf, ctx->userData);                          
	#endif // CTML_PRETTY

#endif // CTML_NOLIBC

//TODO: Add some ESCAPED_TEXT macro for safety

// Definition of TEXT macro.
#ifdef CTML_PRETTY
	#define TEXT(t) ctml_indent(ctx, ctx->indent);ctx->sink(t, ctx->userData);ctx->sink("\n", ctx->userData);
#else
	#define TEXT(t) ctx->sink(t, ctx->userData);
#endif // CTML_PRETTY



#ifdef CTML_IMPLEMENTATION

#ifdef CTML_PRETTY
	void ctml_indent(CTML_Context* ctx, int count) {
		for (int i = 0; i < count; i++) {
			ctx->sink(" ", ctx->userData);
		}
	}
#endif // CTML_PRETTY

//TODO: Maybe passing the str length to the sink might be
// 	a good idea. Most of the time this will be required
// 	to be computed anyway. If done in a smart way it could
// 	be fully optimized by the compiler as every string length
// 	would be compile-time known except for the TEXT that contain
// 	arbitrary string.
//
//	NOTE: idk if it is possible to implement my own strlen to not
//	depend on libc but still get gcc to optimize it when it is
//	compile time known.
#define output(c) ctx->sink(c, ctx->userData)

void ctml_open_tag(CTML_Context* ctx, CTML_Tag* tag) {
	#ifdef CTML_PRETTY
		ctml_indent(ctx, ctx->indent);
		ctx->indent++;
	#endif
	output("<");
	output(tag->tag_name);

	#define X(field)                                  \
		if (tag->field != 0) {                 \
		        output(" " #field);          \
		        output("=\"");               \
		        output(tag->field);          \
		        output("\"");                \
		}                                         
	#define XL(field, lname)                          \
		if (tag->field != 0) {                 \
		        output(" " #lname);          \
		        output("=\"");               \
		        output(tag->field);          \
		        output("\"");                \
		}                                         
	ATTRIBUTES
	#ifdef CTML_CUSTOM_ATTRIBUTES
		CTML_CUSTOM_ATTRIBUTES
	#endif
	#undef X
	#undef XL

	if (!tag->self_close){
		output(">");
	} else {
		output("/>");
	}
	#ifdef CTML_PRETTY
		output("\n");
	#endif
}

void ctml_close_tag(CTML_Context* ctx, CTML_Tag* tag) {
	if (tag->self_close) return;
	
	ctx->indent--;

	#ifdef CTML_PRETTY
		ctml_indent(ctx, ctx->indent);
	#endif

	output("</");
	output(tag->tag_name);
	output(">");

	#ifdef CTML_PRETTY
		output("\n");
	#endif
}

#endif // CTML_IMPLEMENTATION

#endif // CTML_H
