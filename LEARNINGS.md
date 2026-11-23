# Learnings

Generalized rules, banked as I go. Written by `/learnings`. This is the lab
notebook and the real deliverable of the whole project — the C backend is the
vehicle.

**The unit is the rule, not the incident.** Each heading is a rule stated so it
applies to code I haven't written yet. A reader scanning only the headings
should get a usable summary of everything I know. If a heading can't carry its
rule alone, it isn't general enough yet.

Everything lands here: Python collisions, toolchain behaviour, what the standard
does and doesn't promise, experiments that came out the other way, and the
lesson from every bug I debug.

Every entry carries **When** — the date and the phase it came from. The date
makes the recurrence gap measurable (a rule arriving twice, six months apart, is
different from twice in a week); the phase makes it possible to ask what each
phase actually taught me when it closes.

## Bugs

A segfault, leak or sanitizer hit gets an entry **at the moment of surprise,
before the fix** — the fix lives in git, this file holds the misunderstanding
that caused it. Same three fields; the memory category goes in the tags.

**Memory tags:** `leak` · `error-path-leak` · `double-free` · `use-after-free` ·
`dangling-stack` · `pointer-invalidation` · `buffer-overrun` · `off-by-one` ·
`uninitialised-read` · `integer` · `alignment-aliasing` · `ownership-ambiguity` ·
`lifetime-design` · `concurrency` · `fd-leak`

**Other tags:** `ub` · `toolchain` · `types` · `memory` · `process` ·
`python-collision` · `standard`

## Review

Monthly, with `/review`. Two questions:

1. **Is the memory-tag mix shifting?** Phases 1-3 should be dominated by the
   mechanical tags (`leak`, `off-by-one`, `buffer-overrun`). If
   `ownership-ambiguity` and `lifetime-design` aren't the majority by Phase 5,
   the drills didn't take. That shift is the actual measure of progress.
2. **Are rules recurring?** The same rule arriving twice from a different
   direction means it hadn't landed. Repeats get their own entry and are
   expected — the lengthening gap between them is the evidence it's sticking.

Claude never reads entries back to me mid-struggle — not in `/teach`, not in
`/stuck`, not as a hint. Rediscovering a rule by hand is where it gets
internalised. This file is read by me at review, and by `/wrap` when seeding the
recall queue. It is also the source for `/teach`'s quiz.

---

# Log

<!-- newest at the bottom -->

## A C program running without complaining proves nothing

- **I used to assume:** that if a program ran and didn't error, it was legal —
  the way Python guarantees me an `IndexError` or a traceback when I break a
  rule. In C the rules live in a document, and nothing at runtime enforces them
  on my behalf. "It worked" and "it is correct" are unrelated claims.
- **Found out by:** `printf` with `%s` and an integer `0` printed `(null)` and
  exited `0`. Undefined behaviour that a polite libc happened to absorb.
- **When:** 2026-10-18 · P0.1
- **Tags:** `ub`, `python-collision`

## A crash is the good outcome; silent undefined behaviour is the dangerous one

- **I used to assume:** that a segfault was the worst thing that could happen.
  It's the best — loud, immediate, found in seconds. UB that silently works is
  what ships, runs for months, and breaks on an OS upgrade or at `-O2`.
- **Found out by:** `%s` with `3` segfaulted instantly; `%s` with `0` printed
  `(null)` and looked fine. Same category of mistake, opposite visibility.
- **When:** 2026-10-18 · P0.1
- **Tags:** `ub`

## Undefined behaviour means the standard imposes no requirements at all

- **I used to assume:** that "undefined" meant "unspecified result" — that I'd
  get *some* answer, just not a guaranteed one. It means the implementation may
  do anything: crash, print, work, or have the compiler assume the code is
  unreachable and delete it.
- **Found out by:** C11 §5.1.2.2.3 explicitly guarantees that `main` falling off
  the end returns `0`, while nothing guarantees anything about `%s` with a null
  pointer. Both "worked". Only one was promised.
- **When:** 2026-10-18 · P0.1
- **Tags:** `ub`, `standard`

## At runtime there are only bytes — types exist for the compiler and are gone

- **I used to assume:** values carry their type with them, as Python objects do.
  They don't. The compiler uses types to decide which instructions to emit, then
  erases them. Nothing downstream can inspect what it was handed.
- **Found out by:** one `printf` call rendered the same `0` as `0` and as
  `(null)` in the same line of output, with no way to notice the contradiction.
- **When:** 2026-10-18 · P0.1
- **Tags:** `types`, `python-collision`

## C does no implicit conversion at a call — a format string is a promise, not a description

- **I used to assume:** that `printf` would convert whatever I gave it to text,
  the way Python's `print` asks the object to render itself. The format string
  is the *only* source of truth about the arguments; `printf` acts on it blindly
  and cannot verify it. A wrong specifier is me lying to a function that has no
  way to catch me.
- **Found out by:** `%s` with an `int` — segfault, because `printf` took the
  integer as an address and tried to read from it.
- **When:** 2026-10-18 · P0.1
- **Tags:** `types`, `python-collision`, `alignment-aliasing`

## `main`'s return value is the process exit status, and the outside world reads it

- **I used to assume:** a program's return value was an internal detail, the way
  `main()`'s is in Python. It's the one number a process can say to the world as
  it dies: `0` is success, non-zero is failure, and the shell, `&&`, `make` and
  CI all branch on it. `$?` is the shell's `subprocess.returncode`.
- **Found out by:** `return 3` from `main`, then `echo $?` printing `3`.
- **When:** 2026-10-18 · P0.1
- **Tags:** `process`, `python-collision`

## An exit status above 128 means the process was killed, not that it returned

- **I used to assume:** `$?` always held a value my program chose. `128+N` means
  it was killed by signal N and never returned at all — `130` is `128+2`,
  SIGINT, Ctrl-C. `$?` is also clobbered by *every* command, so it has to be
  read immediately after the one I care about.
- **Found out by:** `echo $?` printing `130` after a program that returned `3`.
- **When:** 2026-10-18 · P0.1
- **Tags:** `process`

## Falling off the end of a function is a trap everywhere except `main`

- **I used to assume:** a missing `return` would be a compile error, or a
  guaranteed zero. C11 §5.1.2.2.3 special-cases `main` alone to return `0`. Any
  other `int` function that reaches its closing brace hands the caller garbage,
  and using that value is undefined.
- **Found out by:** deleting `return 0;` from `main`; it compiled, ran, and
  `$?` was `0`.
- **When:** 2026-10-18 · P0.1
- **Tags:** `standard`, `ub`

## A header carries the shape of a function, never its body

- **I used to assume:** `#include` fetched the code, the way `import` does.
  `stdio.h` is ~595 lines of declarations, typedefs and macros and contains not
  one line of `printf`'s implementation. The compiler needs only the shape to
  emit a correct call; the linker supplies the body later from libc. That split
  is why a C build fails in two unrelated ways.
- **Found out by:** `clang -E hello.c | wc -l` → 599 lines from a 4-line file.
- **When:** 2026-10-18 · P0.1
- **Tags:** `toolchain`

## `#include` is text substitution, not `import`

- **I used to assume:** include and import were the same idea. `import` runs a
  module once, caches it, and gives me a namespace. `#include` pastes the file's
  text in, every time, into the global scope, with no cache and no namespace —
  which is why include guards have to exist.
- **Found out by:** scrolling `clang -E hello.c` and finding my four lines at
  the bottom of 595 pasted ones.
- **When:** 2026-10-18 · P0.1
- **Tags:** `toolchain`, `python-collision`

## A binary is tied to its OS's executable format, not just to the CPU

- **I used to assume:** matching architecture meant a binary would run. macOS
  loads Mach-O, Linux loads ELF, and the kernel won't recognise the other as a
  program at all — same arm64 CPU or not.
- **Found out by:** `file hello` → `Mach-O 64-bit executable arm64`, then
  working out what would happen if I `scp`'d it to the arm64 EC2 box.
- **When:** 2026-10-18 · P0.1
- **Tags:** `toolchain`

## Address 0 is left unmapped on purpose, so null dereferences fail loudly

- **I used to assume:** `NULL` was a value like `None`, checked by the runtime.
  It's address `0`, and the OS deliberately leaves the bottom of the address
  space unmapped so that touching it traps instead of silently corrupting
  something. The hardware is the check — there is no runtime doing it.
- **Found out by:** `%s` with `3` segfaulting: the read was refused by the
  memory unit, not by `printf`.
- **When:** 2026-10-18 · P0.1
- **Tags:** `memory`

## `main` is the contract with the OS, not a convention

- **I used to assume:** that main could be named anything else, just like in
  Python, and it would still be compiled and invoked.
- **Found out by:** renaming `main` to `main123`. Renaming it breaks at the
  **linker** stage, because the linker adds the startup code and that is what
  calls `main` — `ld: Undefined symbols: _main, referenced from:
  <initial-undefines>`. The compiler had no objection at all.
- **When:** 2026-10-18 · P0.1 (pipeline rebuild)
- **Tags:** `toolchain`, `python-collision`, `process`

## The type of an arithmetic result comes from the operands, not the variable on the left

- **I used to assume:** the output type is based on the variable type we define
  on the left side. It isn't — the operation and its result type are chosen from
  the *operands'* types, decided at compile time, before the result is ever
  assigned to anything.
- **Found out by:** `5 / 2` gave `2` (both `int` → integer division), but
  `double r = 5 / 2;` gave `2.0`, not `2.5` — the right side was computed as two
  ints first, *then* the `2` was widened to `double`. The `double` on the left
  had no say in how the division was done.
- **When:** 2026-11-08 · P0.2
- **Tags:** `types`, `python-collision`

## In one scope a name binds to one thing — a local shadows a function of the same name

- **I used to assume:** the compiler could tell a `sum` variable and a `sum`
  function apart by context and understand both together. It can't — within a
  scope (e.g. inside a function) a name can mean only one thing.
- **Found out by:** declaring `int sum = 0;` inside my function `sum`, then
  trying to call `sum(1)` below it → `called object type 'int' is not a function
  or function pointer`. After the local declaration, the name `sum` *is* the int;
  the function is unreachable by that name in its own body. My working program
  only survived because it never called `sum` after the shadow — the bug was
  latent, not active.
- **When:** 2026-11-08 · P0.2
- **Tags:** `standard`

## A fixed-width integer overflows silently and hands back a wrong answer, not an error

- **I used to assume:** if the result went past what an `int` can hold, it would
  fail — I guessed at the compiler step. An integer has a fixed limit; when the
  value goes beyond it and we store it in an `int`, it silently gives us the
  wrong answer with no crash and no warning.
- **Found out by:** `sum(100000)` should be `5,000,050,000` but printed
  `705082704` and exited `0`. That's exactly the true sum minus `2³²` — the low
  32 bits; the top bits fell off and nobody checked. The compiler never knew the
  value, because `n` is only known at run time. (Signed overflow is also *UB*,
  not a guaranteed wrap — revisit at `-O2`.)
- **When:** 2026-11-08 · P0.2
- **Tags:** `integer`, `ub`, `python-collision`

## Integer divide-by-zero is undefined behaviour in C — depending on the machine it may or may not crash

- **I used to assume:** it would raise a divide-by-zero error or crash — the way
  Python guarantees a `ZeroDivisionError` every time.
- **Found out by:** `7 / b` with `b = 0` on my arm64 Mac printed `0` and exited
  `0` — no crash at all. On an x86 machine the same code would trap (SIGFPE,
  `$? = 136`). Same UB, opposite outcome by architecture.
- **When:** 2026-11-08 · P0.2
- **Tags:** `ub`, `integer`, `python-collision`

## `sizeof` is a compile-time operator, not a runtime measurement

- **I used to assume:** it measured an object's size while the program ran, like
  Python's `len` / `sys.getsizeof`. Really the compiler reads the operand's
  **static type**, works out the size *during compilation*, and bakes in a
  constant — there's no runtime work at all. It doesn't even evaluate its operand
  (`sizeof(x++)` leaves `x` unchanged; `sizeof(f())` never calls `f`).
- **Found out by:** asking what `sizeof` does underneath — `sizeof(sent)` for
  `char sent[20]` is replaced by the literal `20` in the generated code. Also why
  it "lies" after array→pointer decay: it's reading the static type, which has
  become `char *`.
- **When:** 2026-11-14 · P0.2
- **Tags:** `types`, `python-collision`

## A null-terminated C string cannot hold a `0` byte as data

- **I used to assume:** a string could contain any character, the way a Python
  `str`/`bytes` can hold `\x00` anywhere. But C marks a string's end with the
  `'\0'` sentinel (value 0), so the convention *steals* the value 0 — a zero byte
  in the middle makes every `'\0'`-based loop and library function stop dead
  there, and everything after it is invisible though still in memory.
- **Found out by:** asking "what if the string actually has a 0?" — a `0` at
  index 3 of `"abc\0ef"` means `strlen`/`printf("%s")`/my own loop all see just
  `"abc"`. Anything that can contain zero bytes (HTTP bodies, binary data) needs
  an explicit `(pointer, length)`, not a `char *`.
- **When:** 2026-11-14 · P0.2
- **Tags:** `types`, `python-collision`, `standard`

## Python `len()` is O(1), C `strlen` is O(n) — "runtime" doesn't mean "linear"

- **I used to assume:** that because `len` runs at runtime (unlike `sizeof`), it
  must scan the object, so it'd be O(n). It isn't — Python stores the element
  count *in the object* (`ob_size`) and `len()` just reads it (O(1)); mutating
  methods keep that counter in sync. C strings store no length, so `strlen` has
  to walk byte-by-byte to the `'\0'` (O(n)). The cost gap is a direct consequence
  of whether the length is stored.
- **Found out by:** asking whether Python's `len` is O(n) — it isn't, and the
  contrast with `strlen` pinned down why. It's the same "C arrays don't store
  their length" rule, now showing up as a complexity difference.
- **When:** 2026-11-14 · P0.2
- **Tags:** `types`, `python-collision`

## A pointer is a virtual address, not a RAM location — the MMU translates every access

- **I used to assume:** an address in C was a direct spot in physical RAM. Each
  process has its own **virtual** address space; the **MMU** (hardware) translates
  virtual→physical on every access via page tables set up by the OS. "Unmapped"
  means no translation exists.
- **Found out by:** working out why dereferencing `NULL` crashes. Address 0 is
  deliberately left **unmapped**, so the MMU can't translate it → raises a
  **page fault** (hardware trap into the kernel) → the kernel judges it illegal →
  sends **SIGSEGV** → process dies (`$? = 139`). Note: a page fault isn't always
  fatal — it's the same mechanism behind lazy allocation and swap; the kernel
  often just maps a page in and resumes.
- **When:** 2026-11-15 · P0.3
- **Tags:** `memory`, `python-collision`

## Comparing signed with unsigned converts the signed to unsigned — a negative becomes huge

- **I used to assume:** mixed-integer comparisons just did the obvious thing.
  `size_t` (what `sizeof` returns) is **unsigned** (a size can't be negative), and
  comparing `int i < sizeof(...)` converts `i` to unsigned first — so a *negative*
  `i` wraps to a giant number and the comparison flips.
- **Found out by:** `-Wextra` flagging `i < sizeof(sent)` in `vowels.c`
  (`-Wsign-compare`). Harmless there (`i` only grows), but the canonical
  `for (size_t i = len-1; i >= 0; i--)` is an **infinite loop** — unsigned can
  never be `< 0`. A bare compile and a correct-looking run both hid it; the flag
  caught it.
- **When:** 2026-11-15 · P0.3
- **Tags:** `types`, `integer`

## A signed negative and a large unsigned are the *same bits* — signedness is only an interpretation

- **I used to assume:** the hardware had a separate representation for negatives,
  and converting negative→huge-unsigned was the hardware "not understanding"
  negatives. Really it's **two's complement**: `-1` is stored as all-1s bits,
  which read as unsigned *is* the maximum value. The CPU uses one set of add/sub
  instructions for both; only comparison and overflow care about sign.
- **Found out by:** realizing the signed→unsigned conversion is *free* — the same
  bits, just reinterpreted. `-1` and `UINT_MAX` are identical bits. Same idea as
  "at runtime there are only bytes; types exist for the compiler," now at the
  level of integer representation.
- **When:** 2026-11-15 · P0.3
- **Tags:** `types`, `integer`

## Compiler needs a *declaration*, linker needs a *definition* — that's the dividing line

- **I used to assume:** an "unknown function/symbol" was always one kind of error.
  The **compiler** needs a *declaration* (the shape) for anything you use, and
  errors (file:line) if it's missing from the one file it can see. The **linker**
  needs a *definition* and errors (a symbol, prefixed `ld:`, no line) if it's
  missing across all the combined object files.
- **Found out by:** breaking code on purpose in 0.3. A misspelled *call* (`pintf`)
  is **undeclared** → compiler error with a line. A renamed `main` is valid,
  fully-defined code, but the startup code's `_main` is **undefined** → linker
  error, a symbol, no line. (Historical note: pre-C99 allowed implicit
  declarations, so `pintf` *would* have been a linker error then — the boundary
  moved earlier, the principle didn't.)
- **When:** 2026-11-15 · P0.3
- **Tags:** `toolchain`

## The compiler compiles one `.c` at a time, in isolation — blind to every other `.c`

- **I used to assume:** the compiler saw the whole program at once, the way
  Python imports pull in whole modules. Each `.c` compiled alone is a **translation
  unit**, and the compiler can't see any other `.c`. To call a function whose body
  lives in another `.c`, this unit needs only its **declaration** (from a header);
  the **linker** connects the call to the real definition afterwards.
- **Found out by:** splitting a program into `main.c` + `util.c` + `util.h` in
  0.4. `main.c` compiles fine knowing only `max_of_arr`'s declaration; the body in
  `util.c` is invisible to it until link time.
- **When:** 2026-11-15 · P0.4
- **Tags:** `toolchain`, `python-collision`

## The linker matches symbols by NAME only — it never checks types

- **I used to assume:** the toolchain verified a function's types end to end. The
  **compiler** checks types *within one file*; the **linker** just wires "needs
  `add`" to "has `add`" by name and never looks at types. So if a header's
  declaration and the `.c`'s definition disagree on types and nothing catches it,
  it **links cleanly and becomes runtime UB**.
- **Found out by:** working out why `util.c` should `#include` its own header —
  it's the one place the compiler is forced to see the declaration and definition
  together and check they match, since the linker never will. (Same root as "at
  runtime there are only bytes" — the linker wires addresses, nobody re-checks
  types.)
- **When:** 2026-11-15 · P0.4
- **Tags:** `toolchain`, `types`

## An include guard makes a header idempotent — include it any number of times, it takes effect once

- **I used to assume:** guards were just "avoid duplicate code." They stop two
  distinct failures: **duplicate definitions** (a *diamond* include — two paths
  reach the same header) and **infinite recursion** (a *circular* include).
  Declarations may repeat freely; **definitions** (structs, typedefs) may not — so
  the guard is really protecting the definitions.
- **Found out by:** the circular-include experiment in 0.4 — *with* the guard it
  compiles (the second visit hits `#ifndef` and is skipped); *without* it the
  preprocessor recurses until `#include nested too deeply`.
- **When:** 2026-11-15 · P0.4
- **Tags:** `toolchain`

## A header must include its own dependencies — it has to compile no matter who includes it, or in what order

- **I used to assume:** it was fine for a header to rely on the `.c` having
  included things first. A header that names `size_t` must itself
  `#include <stddef.h>`; otherwise it silently depends on include order and breaks
  for the next file that includes it first or alone. A header stands on its own
  feet (self-contained) and is guarded.
- **Found out by:** `util.h` named `size_t` but included nothing — it only built
  because both `.c` files included `<stdio.h>` *before* it. A file including only
  `util.h` failed with `unknown type name 'size_t'`.
- **When:** 2026-11-15 · P0.4
- **Tags:** `toolchain`

## `make` rebuilds by comparing file *timestamps*, not contents — stale = target older than a prerequisite

- **I used to assume:** `make` would just rebuild everything, or somehow diff the
  files. It never reads contents: a target is rebuilt when it is **older than any
  of its prerequisites** (by mtime). If `util.o` is older than `util.c` or
  `util.h` → rebuild; otherwise skip (`"up to date"`).
- **Found out by:** running `make` twice — the second run did nothing. Also why a
  header must be *listed* as a prerequisite (or a change to it is missed), and why
  `touch`ing a file forces a rebuild even though nothing in it changed.
- **When:** 2026-11-15 · P0.5
- **Tags:** `toolchain`

## Declare struct members largest-alignment-first — `sizeof` is members-plus-padding, not the sum, and member *order* changes it

- **I used to assume:** there wouldn't be any padding — a struct's `sizeof` is
  just the sum of its members' sizes, and the order you declare them in doesn't
  matter.
- **Found out by:** `struct {char a; int b; char c;}` printed `12` but the same
  three members reordered as `{int b; char a; char c;}` printed `8` — same data,
  33% smaller. Each member must sit at an address that's a multiple of its own
  alignment (an `int` at a multiple of 4, a `double` at a multiple of 8) so the
  CPU can read it in one aligned access; declaring the largest-alignment members
  first minimises the padding bytes inserted to satisfy that. Also: a `char`-only
  struct has alignment 1 and zero padding (`sizeof` 3 for three chars), and the
  total is rounded up to the struct's own alignment so arrays stay aligned.
- **When:** 2026-11-16 · P1A
- **Tags:** `alignment-aliasing`, `types`, `memory`

## Declare a no-parameter function as `(void)`, not `()` — in C they mean different things

- **I used to assume:** empty parens `()` meant "this function takes no
  arguments" — like `def f():` in Python.
- **Found out by:** calling `foo(99)` on a function declared `foo()` compiled
  clean under `-Wall -Wextra` — `()` means "unspecified / unchecked argument
  list," so the compiler does *no* argument checking. Declaring it `foo(void)`
  made the same call a hard compile error. On `-std=c11` (this whole project) `()`
  is still unchecked; `(void)` is the only way to say "no parameters" *and* the
  only one that lets the compiler catch a wrong call. (C23 redefines `()` to mean
  no-params, but every pre-C23 source I'll read — King, man pages, libpq, OpenSSL
  — carries the old meaning.)
- **When:** 2026-11-16 · P1A
- **Tags:** `standard`, `python-collision`

## `printf` does no type checking or detection — it trusts the format string completely

- **I used to assume:** `printf` figured out each argument's type itself (and
  that it happened "at compile time").
- **Found out by:** at runtime a C value is just untyped bytes — `printf` is
  *variadic* (`printf(const char *fmt, ...)`) and reads the format string to
  decide how many bytes to grab and how to interpret them. `printf("%s\n", n)`
  with `n` an `int` took the int's bits as an **address**, followed it, and
  segfaulted; `%d` on a `double` just printed a garbage number. The only
  type-checking is a *compile-time courtesy* — clang `-Wformat` (in `-Wall`) reads
  the **literal** format string and warns (it even printed the right specifier),
  but that's the compiler, not `printf`, and it can't help if the format string is
  a variable. Python's `print` can't be lied to this way — it receives a tagged
  object and asks it its type; C hands over naked bytes with nothing to ask.
- **When:** 2026-11-17 · P1A
- **Tags:** `types`, `python-collision`, `ub`

## A `static` local lives in the program's data segment, not the stack frame — its value survives after the call frame is torn down

- **I used to assume:** every local dies when its function's frame is popped, so
  to keep a value across calls you'd need a global variable.
- **Found out by:** a function with `static int cnt = 0; cnt++;` printed 1,2,3,4
  across four calls; deleting just the `static` made it print 1,1,1,1. The static
  version keeps its value because it lives in a fixed data-segment slot (created
  once, before `main`, `= 0` run only once) that frame teardown never touches —
  while its *scope* stays local (invisible outside the function). So scope (where
  a name is visible) and storage duration (how long its bytes live) are two
  independent axes; `static` changes only the second. (Python/JS fake this with
  closures / mutable default args — a long-lived object *referencing* the value on
  the heap; C's static is a permanent reserved address with no reference needed.)
- **When:** 2026-11-17 · P1A
- **Tags:** `memory`, `python-collision`

## A C `enum` is just named integer constants — no runtime guard, no type check, unlike Python's `Enum`

- **I used to assume:** a C `enum` was a distinct, safe type like Python's
  `Enum` — members that aren't plain ints, and that you can't put an arbitrary
  integer into.
- **Found out by:** `enum State { LOCKED, UNLOCKED };` just makes named int
  constants (`LOCKED`==0, `UNLOCKED`==1, auto-numbered from 0); an `enum State`
  variable is a single int. So `int x = UNLOCKED;` is fine and `enum State s = 99;`
  compiles with no complaint and stores 99 — a state the enum never named. There's
  no array/object stored for the "set," and no runtime guarding at all; the enum is
  pure compile-time *naming* over plain ints. (Payoff isn't safety — it's
  readability + a `-Wswitch` warning when a `switch` misses a case.)
- **When:** 2026-11-18 · P1A
- **Tags:** `types`, `python-collision`

## A `NULL` pointer fails loud (deterministic SIGSEGV); an uninitialized pointer fails silent (garbage address) — so initialize pointers to `NULL` and null-check before dereferencing

- **I used to assume:** dereferencing a bad pointer — `NULL` included — just hands you a garbage value. (This was literally my guess at the start of the session.)
- **Found out by:** `p1b/null.c` — `int *p = NULL; *p` is a *deterministic* SIGSEGV at address `0x0` (the deliberately-unmapped virtual page), **not** garbage. It's the *uninitialized* pointer that holds a leftover garbage address and may deref into valid-looking memory → silent corruption that passes every test. `NULL` is C's `None`, but its deref is an **uncatchable** crash, not a catchable `AttributeError` — so guard every maybe-null deref with a null-check.
- **When:** 2026-11-22 · P1B
- **Tags:** `memory`, `ub`, `python-collision`

## A runtime crash can still name a source line — because compile-time tooling (`-g`'s DWARF map, and the sanitizer's inserted checks) writes file:line *into the binary* for a runtime tool to read back

- **I used to assume:** once the program is running the compiler is long gone, so a runtime crash has no way to know *which source line* it happened on.
- **Found out by:** the `NULL` deref under `-fsanitize -g` reported `null.c:<line>` — because at **compile time** UBSan records every dereference site *with its file and line* and bakes that into the binary, and `-g` writes a DWARF address→line map (the `.dSYM`). The runtime handler just prints back what was stored. Strip `-g` *and* the sanitizers and the same deref gives a bare `segmentation fault` with **no line** — nobody wrote the map down.
- **When:** 2026-11-22 · P1B
- **Tags:** `toolchain`, `process`

## The same cast-away-`const` write has two faces depending on where the bytes live — it silently sticks on the writable stack, but crashes in read-only memory

- **I used to assume:** casting `const` away and writing would behave one consistent way — and that if the compiler could "see" the target was `const`, it would stop me.
- **Found out by:** the write always *compiles* once you cast (the compiler only checks the type in front of it, and the cast strips `const` off that type — it doesn't trace where the pointer came from). What differs is runtime, and it's UB: `p1b/constcheck.c` — a `const int` local lives on the **writable stack**, so `*p = 99` silently *stuck* (no crash). `p1b/strlit.c` — writing through a pointer to a `"double-quoted string"` *crashed* (ASan: WRITE access, SIGBUS at strlit.c:5), because string literals are stored in **read-only memory** that the hardware refuses to let you write. Same mistake, opposite symptom — decided entirely by where the compiler placed the bytes.
- **When:** 2026-11-23 · P1B
- **Tags:** `ub`, `memory`

## Casting `const` away lets the write compile, but reading the name can still show the OLD value — the compiler cached the `const` as a literal at compile time, so `const` is a compile-time promise, not a runtime lock on the bytes

- **I used to assume:** if the write went through (no crash), then reading the variable afterwards would obviously show the new value — same address, same answer.
- **Found out by:** `p1b/constcheck.c` — after `*p = 99` (where `p` was `(int *)&x`), `*p` printed `99` but plain `x` printed `5`, *same address `0x16b4fe680`, same run*. Because `x` was declared `const int x = 5`, the compiler trusted the promise and baked the literal `5` straight into the `printf` — it never emitted a read from memory for the name. `*p` reads memory (99); `x` was replaced by the cached literal (5). `const` bought zero runtime protection of the storage — it's a promise the compiler *builds on*, and breaking it silently desyncs memory from code (UB).
- **When:** 2026-11-23 · P1B
- **Tags:** `ub`, `types`, `toolchain`
