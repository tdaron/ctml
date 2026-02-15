/* ctml - v1.0.3 - https://github.com/tdaron/ctml

# CTML

This library is a small single header file (~150-200 SLoC of C) HTML templating engine
for the C programming language.

CTML is macro-based as the objective is to provide a nice to use 
DSL inside of the C programming language.

**Status**: This library is still in development. It works but is not as
optimized as it could be. This warning will be removed when the library
is considered usable.


## Features

- Directly embedded inside C code
- Really lightweight
- Not any dependencies (not even libc)
- C functions as components

As this library is a header-only library, at (only) one place
in your code you need to define `CTML_IMPLEMENTATION` to include
the implementation of the library.


## Quick Example

### With shorthands
(only supports a subset of html tags defined in `ctml_short.h`)

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
                ctml_text("hello, world");
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
                ctml_text("hello, world");
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

NOTE: The sink function is a `void *(sink) (char*, void* userData)` where ctml
will send the generated HTML.
CTML will send data in multiple batches and not only once with the
full generated HTML. 

The sink function also takes `void* userData` with user data
given to `ctml()` using `.userData = ...`

## Usage

The api of the library is really simple. It only consists of few
macros and 1 type. 

The first macro is `h(tag, attributes*)` that is used to created
an HTML tag. You also have `hh(tag, attributes*)` to create self closed
tags.

You'll also need the `ctml` macro that will create a context 
containing the sink as well as the indentation state.

Two last 4 macros allow you to put text inside of the HTML.
You can use `ctml_text(char* text)` to put some text in the
HTML. This **will** be escaped by default. You can also use
`ctml_textf(char* text, ...)` that accepts formating like
printf and co.


The last ones are `ctml_raw(char* text)` and `ctml_rawf(char* text, ...)`
that works the same way than `ctml_text` except they do not do any escaping.
**IMPORTANT**: ctml_raw(f) does NOT escape anything.

Formatting macros depends on libc to work. (Using snprintf under the hood).

The only type you should care about is `CTML_Context` as explained
in the [Components](#Components) section.

## Components

Creating components boils down to just calling C functions. The
only requirement is for you to pass the ctml context across components
this way:

```c
void some_button(CTML_Context* CTML_CTX_NAME) {
    h(button, .class="my-btn") {ctml_raw("click me");}
}

void my_ui() {
    ctml(.sink=...) {
        h(div) {
            // NOTE HERE THE CTX
            some_button(CTML_CTX_NAME);
        }
    }
}

```

The `ctx` variable is created by the `ctml` macro. You just need
to pass it around.

## Configuration
This library can be configured using some macros. 

`CTML_PRETTY` will enable pretty print of the HTML.
`CTML_NOLIBC` will disable all libc dependent stuff (only
the ctml_rawf macro)
`CTML_CUSTOM_ATTRIBUTES` as explained in [Custom Attributes](#custom-attributes).

To avoid calling the sink loads of times, ctml will bufferize the output.
The size of this buffer can be parametrized using `CTML_SINK_BUFSIZE`.
The default value is 1024 bytes. Setting it to `1` or `0` will disable
buffering.

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
        ctml_text("ctml is great");
    }
}
```
°°
*/
#ifndef CTML_H
#define CTML_H

#ifndef CTML_CTX_NAME
#define CTML_CTX_NAME ctx
#endif

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
	char self_close;
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

#ifndef CTML_SINK_BUFSIZE
	#define CTML_SINK_BUFSIZE 1024
#endif

typedef void (*ctmlSink) (char*, void* userData);
typedef struct {
	ctmlSink sink;	
	void* userData;
	int indent;
	char outputBuf[CTML_SINK_BUFSIZE];
	int bufferedDataLength;
} CTML_Context;


#ifdef CTML_PRETTY
	void ctml_indent(CTML_Context* CTML_CTX_NAME, int indent);
#endif

void ctml_open_tag(CTML_Context* CTML_CTX_NAME, CTML_Tag* tag);
void ctml_close_tag(CTML_Context* CTML_CTX_NAME, CTML_Tag* tag);

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
//      ctml_raw("tata");
//	ctml_raw("toto");
//	
// } <-- END OF FOR LOOP - will call ctml_close_tag for the dib
// 
#define ctml_tag(n, ...)                                               \
	CTML_Tag CONCAT(_tag_, __LINE__) = {.tag_name=#n, __VA_ARGS__};			\
	ctml_open_tag(CTML_CTX_NAME, &CONCAT(_tag_, __LINE__));                                         \
	for (int _once = 0; _once < 1; _once=1,ctml_close_tag(CTML_CTX_NAME, &CONCAT(_tag_, __LINE__))) \


// Those macros are used by the user.
// h is the basic macro to create tags and hh is a macro to create a self closing tag.
#define h(n, ...) ctml_tag(n, __VA_ARGS__)
#define hh(n, ...) ctml_tag(n, .self_close=1, __VA_ARGS__) {}

// simple macro to create the context
// TODO: Maybe find a way not to have this "hidden" CTML_CTX_NAME the user can use ?
#define ctml(...) CTML_Context ctml_context = (CTML_Context) {__VA_ARGS__}; \
		  CTML_Context* CTML_CTX_NAME = &ctml_context; \
		  for (int _once = 0; _once < 1; _once=1,ctml_flush_buffer(CTML_CTX_NAME)) \


// Definition of ctml_raw macro.
#ifdef CTML_PRETTY
	#define ctml_raw(t) ctml_indent(CTML_CTX_NAME, CTML_CTX_NAME->indent);ctml_output(t);ctml_output("\n");
	#define ctml_text(t) ctml_indent(CTML_CTX_NAME, CTML_CTX_NAME->indent);ctml_escape_text(CTML_CTX_NAME, t);ctml_output("\n");
#else
	#define ctml_raw(t) ctml_output(t);
	#define ctml_text(t) ctml_escape_text(CTML_CTX_NAME, t);
#endif // CTML_PRETTY

// Implementation of ctml_rawf that is basically
// just a snprintf inside a temp buffer.
// The size of the buffer can be changed using the
// macro CTML_BUF_SIZE (default: 1024 bytes)
// TODO: What happens if the buffer is too small. A warning might be printed.
#ifndef CTML_NOLIBC

	#ifndef CTML_BUF_SIZE
		#define CTML_BUF_SIZE 1024
	#endif

	char ctml_tmpbuf[CTML_BUF_SIZE];

	#define ctml_rawf(...)                                     \
		snprintf(ctml_tmpbuf, CTML_BUF_SIZE, __VA_ARGS__);  \
		ctml_raw(ctml_tmpbuf);


#endif // CTML_NOLIBC





#ifdef CTML_IMPLEMENTATION

#define ctml_output(c) ctml_buffered_ctml_output(CTML_CTX_NAME, c, -1);

void ctml_buffered_ctml_output(CTML_Context* CTML_CTX_NAME, char* data, int length) {
	#if CTML_SINK_BUFSIZE == 0 || CTML_SINK_BUFSIZE == 1
		CTML_CTX_NAME->sink(data, CTML_CTX_NAME->userData);
	#else
		int available = CTML_SINK_BUFSIZE - CTML_CTX_NAME->bufferedDataLength - 1;
		int i;
		for (i = 0; i < available; i++) {
			if (data[i] == '\0' || (i == length && length != -1) ) {
				break;
			}
			(CTML_CTX_NAME->outputBuf+CTML_CTX_NAME->bufferedDataLength)[i] = data[i];
		}
		CTML_CTX_NAME->bufferedDataLength += i;
		// Check if the buffer is full
		if (i == available) {
			CTML_CTX_NAME->outputBuf[CTML_SINK_BUFSIZE-1] = '\0';
			CTML_CTX_NAME->sink(CTML_CTX_NAME->outputBuf, CTML_CTX_NAME->userData);

			CTML_CTX_NAME->bufferedDataLength = 0;
		}
		// if there is data left
		if (data[i] != '\0') {
			// send the rest of it
			int new_length = length;
			if (length != -1) {
				new_length -= i;
			}
			if (new_length != 0) {
				ctml_buffered_ctml_output(CTML_CTX_NAME, data+i, new_length);
			}
		}
	#endif
}

void ctml_flush_buffer(CTML_Context* CTML_CTX_NAME) {
	if (CTML_CTX_NAME->bufferedDataLength != 0) {
		CTML_CTX_NAME->outputBuf[CTML_CTX_NAME->bufferedDataLength] = '\0';
		CTML_CTX_NAME->sink(CTML_CTX_NAME->outputBuf, CTML_CTX_NAME->userData);
		CTML_CTX_NAME->bufferedDataLength = 0;
	}
}



#ifdef CTML_PRETTY
	void ctml_indent(CTML_Context* CTML_CTX_NAME, int count) {
		for (int i = 0; i < count; i++) {
			ctml_output(" ");
		}
	}
#endif // CTML_PRETTY

void ctml_open_tag(CTML_Context* CTML_CTX_NAME, CTML_Tag* tag) {
	#ifdef CTML_PRETTY
		ctml_indent(CTML_CTX_NAME, CTML_CTX_NAME->indent);
		if (!tag->self_close) {
			CTML_CTX_NAME->indent++;
		}
	#endif
	ctml_output("<");
	ctml_output(tag->tag_name);

	#define X(field)                                  \
		if (tag->field != 0) {                 \
		        ctml_output(" " #field);          \
		        ctml_output("=\"");               \
		        ctml_output(tag->field);          \
		        ctml_output("\"");                \
		}                                         
	#define XL(field, lname)                          \
		if (tag->field != 0) {                 \
		        ctml_output(" " #lname);          \
		        ctml_output("=\"");               \
		        ctml_output(tag->field);          \
		        ctml_output("\"");                \
		}                                         
	ATTRIBUTES
	#ifdef CTML_CUSTOM_ATTRIBUTES
		CTML_CUSTOM_ATTRIBUTES
	#endif
	#undef X
	#undef XL

	if (!tag->self_close){
		ctml_output(">");
	} else {
		ctml_output("/>");
	}
	#ifdef CTML_PRETTY
		ctml_output("\n");
	#endif
}

void ctml_close_tag(CTML_Context* CTML_CTX_NAME, CTML_Tag* tag) {
	if (tag->self_close) return;
	
	CTML_CTX_NAME->indent--;

	#ifdef CTML_PRETTY
		ctml_indent(CTML_CTX_NAME, CTML_CTX_NAME->indent);
	#endif

	ctml_output("</");
	ctml_output(tag->tag_name);
	ctml_output(">");

	#ifdef CTML_PRETTY
		ctml_output("\n");
	#endif
}

void ctml_escape_text(CTML_Context* CTML_CTX_NAME, char* text) {
	// Not to be escaped count the number of unescaped chars
	// that could be send in one call to ctml_buffered_ctml_output
	int not_to_be_escaped = 0;
	int i;
	for (i = 0; text[i] != '\0'; i++) {

		if (0) {}
		#define ESCAPE(char, escaped_version) \
		else if (text[i] == char) {  \
		        /* Sending all the text that should not be escaped */ \
			ctml_buffered_ctml_output(CTML_CTX_NAME, &text[i-not_to_be_escaped], not_to_be_escaped); \
		        /* Sending escaped version of current char */ \
			ctml_output(escaped_version); \
			/* Resetting text that should not be escaped counter */ \
			not_to_be_escaped = 0; \
		} \

		ESCAPE('<', "&lt;")
		ESCAPE('>', "&gt;")
		ESCAPE('&', "&amp;")
		ESCAPE('\'', "&quot;")
		ESCAPE('"', "&#39;") 
		else {
			// This char should not be escaped
			not_to_be_escaped++;
		}
	}
	if (not_to_be_escaped > 0){
		ctml_buffered_ctml_output(CTML_CTX_NAME, &text[i-not_to_be_escaped], not_to_be_escaped);
	}
}

#endif // CTML_IMPLEMENTATION

#endif // CTML_H
