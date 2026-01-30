#include <unistd.h>
#include <string.h>
#define CTML_NOLIBC
#define CTML_PRETTY
#define CTML_CUSTOM_ATTRIBUTES X(toto);XL(da, data-attr);
#define CTML_IMPLEMENTATION
#include "ctml.h"
#include "ctml_short.h"

void sink(char* src, void*_) {
	write(1, src, strlen(src));
	
}

void cbutton(CTML_Context* ctx) {
	button(.class="btn-primary") {
		t("click");
	}
}

void ui() {
	ctml(
		.sink=sink
	) {
		t("<!DOCTYPE html>");
		html(.lang="en") {
			img(.src="https://google.com/toto.png") 
			div(.class="toto", .da="hey", .toto="hey") {
				h1() {
					t("hello, world ! ");
					t("My name is ");
					t("john");
				}
			}
			cbutton(ctx);
		}
	}
}
int main() {
	ui();
	return 0;
}
