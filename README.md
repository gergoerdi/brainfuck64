Brainfuck compiler and BASIC transpiler for the Commodore 64
============================================================

This compiler runs on the Commodore 64 and emits either C64 machine
code directly, or C64 BASIC. The memory region to use as the Brainfuck
memory and also as the location of the resulting program is
configurable.


Limitations
-----------

* The `,` instruction for input is not supported (it is ignored by the
  compiler)
  
* A maximum of 20 loops can be nested in each other

* Only 256 cells available. Doing a `<` on cell 0 wraps the pointer
  around to cell 255; similarly for `>`.

* There's no check in the compiler to make sure no important bits of
  the RAM is overwritten. For example, using the right memory and
  program base addresses, you can get overlap between the two. You can
  even use a program base address that results in the compiler
  overwriting itself as it emits the results of the compilation.

Screencast
----------
![Brainfuck64 screecast](http://i.imgur.com/FEvHMHb.gif)
