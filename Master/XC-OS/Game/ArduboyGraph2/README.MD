# Graph 2 (for the Arduboy)

A graphing calculator for your Arduboy!

# Features

Here's what's currently here:
* Simple keypad for entering numbers
* An RPN calculator
* Graph output
* Shunting-yard algorithm for converting user-readable functions.
* Formula keyboard

Here's what I hope to accomplish in the near future:
* Direct mode (enter formulas, get response)
* Better error reporting
* Coherent UI
* Table view

# Known Issues

Copied from Arduboy forum thread:
* ~~Division operations are reversed (Function-related issue)~~
* Cursor resets Arduboy when it goes offscreen on main menu (GraphFont-related issue)
* Implicit Multiplication doesn't work (Function-related issue) - `2x`
* Unary Negative doesn't work (Function-related issue) - `-5*x`
* Error reporting doesn't exist - functions fail silently (Function-related issue) - `x+`
* ~~Graph object wastes a lot of memory by storing all Y values (Graph-related issue)~~
* Prepare for functions by converting existing char input system over to a token-based system (Big issue)
  * This means using a more TI-84-like system where there's a dedicated unary minus token and it's completely different from the subtraction token.

# Screenshots

I'm still working on the UI! If you want to see how far I've come, check out [this Twitter thread](https://twitter.com/V360dev/status/1191078748256456706)!

# Credits

* Me - UI, RPN calculator, font
* SmileBASIC Discord - A lot of help with C++
* snail_ - Some help with the Shunting-Yard Algorithm
* yinkou - TinyFont code, used as base for GraphFont
