# Coding style notes

Jasmin code is sometimes as messy as regular assembly code.  Therefore,
to make some sense of it all, here are some style guides that I try to follow.

## Editor settings

* Use tabs, not spaces
* Tabs are 8 cols wide
* Lines should not exceed the 100th column
* Comments should not exceed the 80th column

## Spacing

* Any casts are seperated to their expressions with a single space:
  `(int) i`, not `(int)i`.

## Choices

* All locals in a functions are ordered in the following fashion:

  ```jazz
  fn do_crypto(...) {
          // Inline ints
          inline int gamma1 = 1 << 17;

          // Stack allocated buffers and their pointer registers
          stack u32[20] buf;
          reg ptr u32[20] buf_rsp;

          // Temporary values with a name
          stack u16 attempt;
          reg u16 attempt_reg;
          reg u32 coeff;
          reg u16 nonce;

          // 'v' temporaries that do not have a name
          reg u32 v32_0 v32_1;
          reg u8 v8;

          // Loop indices
          reg u64 i j;

          // (...)
  }
  ```

* All stack-allocated buffers should have a `_rsp` pointer register
  associated with them that is used to index into the buffer.  Otherwise
  jasminc will add an extra `leaq` instruction.  'RSP' refers to the fact
  that this register contains an address that points to a value somewhere in
  the current stack frame.

* Unnamed temporaries are called `vXX_Y`, where `XX` is the size, e.g., `64`,
  `32`, etc., and `Y` can be an index.  If there is a single of these
  variables in the scope of a function, you can omit this info.

* When using literal instructions, always explicitly add the size, e.g.,
  `#LEA_64()`, not `#LEA()`.  (This is s.t. we do not accidentally emit
  instructions like `lead` which you should never use.)

* Always unpack all values from a function or instruction call
  with `_` or `?{}`.

* When assigning a register to `0`, always use `#set0()`.

* Only use `for` loops when you _deliberately_ want to unroll some code.
  The ICache is not infinitely large!