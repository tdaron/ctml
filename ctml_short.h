#ifndef CTML_SHORT_H
#define CTML_SHORT_H

#define html(...)    h(html, __VA_ARGS__)
#define head(...)    h(head, __VA_ARGS__)
#define body(...)    h(body, __VA_ARGS__)
#define title(...)   h(title, __VA_ARGS__)
#define meta(...)    hh(meta, __VA_ARGS__)
#define link(...)    hh(link, __VA_ARGS__)
#define script(...)  h(script, __VA_ARGS__)

#define div(...)     h(div, __VA_ARGS__)
#define span(...)    h(span, __VA_ARGS__)
#define header(...)  h(header, __VA_ARGS__)
#define footer(...)  h(footer, __VA_ARGS__)
#define section(...) h(section, __VA_ARGS__)
#define article(...) h(article, __VA_ARGS__)
#define nav(...)     h(nav, __VA_ARGS__)

#define h1(...)      h(h1, __VA_ARGS__)
#define h2(...)      h(h2, __VA_ARGS__)
#define h3(...)      h(h3, __VA_ARGS__)
#define h4(...)      h(h4, __VA_ARGS__)
#define h5(...)      h(h5, __VA_ARGS__)
#define h6(...)      h(h6, __VA_ARGS__)
#define p(...)       h(p, __VA_ARGS__)
#define a(...)       h(a, __VA_ARGS__)
#define strong(...)  h(strong, __VA_ARGS__)
#define em(...)      h(em, __VA_ARGS__)
#define small(...)   h(small, __VA_ARGS__)
#define code(...)    h(code, __VA_ARGS__)
#define pre(...)     h(pre, __VA_ARGS__)
#define blockquote(...) h(blockquote, __VA_ARGS__)

#define ul(...)      h(ul, __VA_ARGS__)
#define ol(...)      h(ol, __VA_ARGS__)
#define li(...)      h(li, __VA_ARGS__)

#define img(...)     hh(img, __VA_ARGS__)
#define video(...)   h(video, __VA_ARGS__)
#define audio(...)   h(audio, __VA_ARGS__)
#define source(...)  hh(source, __VA_ARGS__)

#define form(...)    h(form, __VA_ARGS__)
#define input(...)   hh(input, __VA_ARGS__)
#define textarea(...) h(textarea, __VA_ARGS__)
#define button(...)  h(button, __VA_ARGS__)
#define label(...)   h(label, __VA_ARGS__)
#define select(...)  h(select, __VA_ARGS__)
#define option(...)  h(option, __VA_ARGS__)

#define table(...)   h(table, __VA_ARGS__)
#define thead(...)   h(thead, __VA_ARGS__)
#define tbody(...)   h(tbody, __VA_ARGS__)
#define tr(...)      h(tr, __VA_ARGS__)
#define th(...)      h(th, __VA_ARGS__)
#define td(...)      h(td, __VA_ARGS__)

#define br(...)      hh(br, __VA_ARGS__)
#define hr(...)      hh(hr, __VA_ARGS__)

#define t(c) TEXT(c)

#endif // CTML_SHORT_H

