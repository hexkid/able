# fabled
FAncy BLock EDitor

This is a Block editor for gForth.

Almost got called "bled", but I came up with this better name.

I could make gForth work with blocks my making
the first line in the first screen start with "\\\\" (backslash backslash),
the first line in all other screens start with "\\ " (backslash space).

To load and interpret the block, just start gForth and issue

    use blocks.fb
    1 load

You can try the gForth block editor with

    use blocked.fb
    1 load           \ words now lists "editor" words
    editor
    0 l              \ screen 0 describes the editor

to edit your blocks with gForth editor, continue from above with

    use my.fb
    1 load
    1 t              \ highlight
    i text here      \ insert
    flush            \ write to disk

I don't like gForth block editor, so I wrote my own.
Enjoy :-)
