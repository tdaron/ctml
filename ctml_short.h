/*
 * ctml_short.h
 * * Short aliases for CTML tags.
 * * To disable a specific shortcut (to avoid naming collisions), 
 * define CTML_NO_<TAGNAME> before including this file.
 * * Example:
 * #define CTML_NO_LINK
 * #define CTML_NO_DIV
 * #include "ctml_short.h"
 */

#ifndef CTML_SHORT_H
#define CTML_SHORT_H

#ifndef CTML_NO_HTML
    #define html(...)    h(html, __VA_ARGS__)
#endif

#ifndef CTML_NO_HEAD
    #define head(...)    h(head, __VA_ARGS__)
#endif

#ifndef CTML_NO_BODY
    #define body(...)    h(body, __VA_ARGS__)
#endif

#ifndef CTML_NO_TITLE
    #define title(...)   h(title, __VA_ARGS__)
#endif

#ifndef CTML_NO_META
    #define meta(...)    hh(meta, __VA_ARGS__)
#endif

#ifndef CTML_NO_LINK
    #define link(...)    hh(link, __VA_ARGS__)
#endif

#ifndef CTML_NO_SCRIPT
    #define script(...)  h(script, __VA_ARGS__)
#endif

// --- Containers ---

#ifndef CTML_NO_DIV
    #define div(...)     h(div, __VA_ARGS__)
#endif

#ifndef CTML_NO_SPAN
    #define span(...)    h(span, __VA_ARGS__)
#endif

#ifndef CTML_NO_HEADER
    #define header(...)  h(header, __VA_ARGS__)
#endif

#ifndef CTML_NO_FOOTER
    #define footer(...)  h(footer, __VA_ARGS__)
#endif

#ifndef CTML_NO_SECTION
    #define section(...) h(section, __VA_ARGS__)
#endif

#ifndef CTML_NO_ARTICLE
    #define article(...) h(article, __VA_ARGS__)
#endif

#ifndef CTML_NO_NAV
    #define nav(...)     h(nav, __VA_ARGS__)
#endif

// --- Typography ---

#ifndef CTML_NO_H1
    #define h1(...)      h(h1, __VA_ARGS__)
#endif

#ifndef CTML_NO_H2
    #define h2(...)      h(h2, __VA_ARGS__)
#endif

#ifndef CTML_NO_H3
    #define h3(...)      h(h3, __VA_ARGS__)
#endif

#ifndef CTML_NO_H4
    #define h4(...)      h(h4, __VA_ARGS__)
#endif

#ifndef CTML_NO_H5
    #define h5(...)      h(h5, __VA_ARGS__)
#endif

#ifndef CTML_NO_H6
    #define h6(...)      h(h6, __VA_ARGS__)
#endif

#ifndef CTML_NO_P
    #define p(...)       h(p, __VA_ARGS__)
#endif

#ifndef CTML_NO_A
    #define a(...)       h(a, __VA_ARGS__)
#endif

#ifndef CTML_NO_STRONG
    #define strong(...)  h(strong, __VA_ARGS__)
#endif

#ifndef CTML_NO_EM
    #define em(...)      h(em, __VA_ARGS__)
#endif

#ifndef CTML_NO_SMALL
    #define small(...)   h(small, __VA_ARGS__)
#endif

#ifndef CTML_NO_CODE
    #define code(...)    h(code, __VA_ARGS__)
#endif

#ifndef CTML_NO_PRE
    #define pre(...)     h(pre, __VA_ARGS__)
#endif

#ifndef CTML_NO_BLOCKQUOTE
    #define blockquote(...) h(blockquote, __VA_ARGS__)
#endif

// --- Lists ---

#ifndef CTML_NO_UL
    #define ul(...)      h(ul, __VA_ARGS__)
#endif

#ifndef CTML_NO_OL
    #define ol(...)      h(ol, __VA_ARGS__)
#endif

#ifndef CTML_NO_LI
    #define li(...)      h(li, __VA_ARGS__)
#endif

// --- Media ---

#ifndef CTML_NO_IMG
    #define img(...)     hh(img, __VA_ARGS__)
#endif

#ifndef CTML_NO_VIDEO
    #define video(...)   h(video, __VA_ARGS__)
#endif

#ifndef CTML_NO_AUDIO
    #define audio(...)   h(audio, __VA_ARGS__)
#endif

#ifndef CTML_NO_SOURCE
    #define source(...)  hh(source, __VA_ARGS__)
#endif

// --- Forms ---

#ifndef CTML_NO_FORM
    #define form(...)    h(form, __VA_ARGS__)
#endif

#ifndef CTML_NO_INPUT
    #define input(...)   hh(input, __VA_ARGS__)
#endif

#ifndef CTML_NO_TEXTAREA
    #define textarea(...) h(textarea, __VA_ARGS__)
#endif

#ifndef CTML_NO_BUTTON
    #define button(...)  h(button, __VA_ARGS__)
#endif

#ifndef CTML_NO_LABEL
    #define label(...)   h(label, __VA_ARGS__)
#endif

#ifndef CTML_NO_SELECT
    #define select(...)  h(select, __VA_ARGS__)
#endif

#ifndef CTML_NO_OPTION
    #define option(...)  h(option, __VA_ARGS__)
#endif

// --- Tables ---

#ifndef CTML_NO_TABLE
    #define table(...)   h(table, __VA_ARGS__)
#endif

#ifndef CTML_NO_THEAD
    #define thead(...)   h(thead, __VA_ARGS__)
#endif

#ifndef CTML_NO_TBODY
    #define tbody(...)   h(tbody, __VA_ARGS__)
#endif

#ifndef CTML_NO_TR
    #define tr(...)      h(tr, __VA_ARGS__)
#endif

#ifndef CTML_NO_TH
    #define th(...)      h(th, __VA_ARGS__)
#endif

#ifndef CTML_NO_TD
    #define td(...)      h(td, __VA_ARGS__)
#endif

// --- Breaks ---

#ifndef CTML_NO_BR
    #define br(...)      hh(br, __VA_ARGS__)
#endif

#ifndef CTML_NO_HR
    #define hr(...)      hh(hr, __VA_ARGS__)
#endif

// --- Utilities ---

#ifndef CTML_NO_T
    #define t(c) TEXT(c)
#endif

#endif // CTML_SHORT_H
