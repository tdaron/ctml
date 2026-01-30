#ifndef CTML_SHORT_H
#define CTML_SHORT_H

#define h1(...) h(h1, __VA_ARGS__)
#define div(...) h(div, __VA_ARGS__)
#define html(...) h(html, __VA_ARGS__)
#define img(...) hh(img, __VA_ARGS__) {}
#define button(...) h(button, __VA_ARGS__)
#define t(c) TEXT(c) 
#endif // CTML_SHORT_H
