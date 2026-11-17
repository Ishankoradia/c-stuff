# STATE — where we are

**Single source of truth for the C training.** Claude reads this first in every
session; `/wrap` rewrites it at the end of every session. If this file and my
memory disagree, this file wins.

Companions: `ROADMAP.md` (curriculum + protocol), `LEARNINGS.md` (lab notebook),
`CLAUDE.md` (teaching contract), `TUNING.md` (course corrections).

---

## Position

- **Phase:** 1 — Core language, drilled → **in 1A** (everything that isn't
  a pointer). **Phase 0 is COMPLETE.** 🎉
- **Task in flight:** none — **structs-as-values + padding, return-by-value, and
  the stack-frame model are all done** (`p1a/sizes.c` and `p1a/retpair.c` both
  built and run clean). The **stack frame** was taught head-on (LIFO frames,
  pushed on call / popped on return, locals' bytes die with the frame) — so the
  1A exit test ("draw the stack for a three-deep call chain") is now *close*.
  **Carry-over (not blocking):** the `p0.5/Makefile` still lacks the strict flag
  set, an `asan` target, and `.PHONY` — round those out early in Phase 1.
- **Next action:** **continue Phase 1A with storage duration — automatic vs
  static.** It builds straight off the stack-frame model just taught and lands as
  a Python-collision: they *just* learned "a local's bytes die when its frame
  pops," and a `static` local flatly contradicts that (survives across calls).
  **First question / Socratic hook:** a function has a local counter; you call it
  three times and it prints 1, 2, 3 — not 1, 1, 1. The frame was torn down and
  rebuilt each call, so how did the count survive — what storage is that local in,
  and where does it live if not in the frame? New drill in `p1a/` (a `static`
  counter that persists across calls). After this, the remaining 1A material is:
  `enum` (a state-machine drill), `const`, scope/shadowing, and then formally
  drawing the stack for the **exit test**. Also still outstanding from the 1A
  plan: the unsigned-wraparound / `-Wconversion` drill.
- **Sessions this phase:** 2
- **Phase 1A exit test:** can **draw the stack for a three-deep call chain** and
  say which locals exist at each moment and when each one dies. (ASan becomes a
  real gate in Phase 2, once things allocate.)

## Last session

**2026-11-16 (session 12, same-day continuation after session 11's wrap) —
the stack frame, return-by-value, and the `()`-vs-`(void)` trap.** Same-day, no
quiz. Prompted a teaching correction: Ishan noted that after a big explanation of
a core concept I jump to the assignment instead of checking understanding — I held
to checking-first for the rest of the session, and this should route through
`/tune` into `CLAUDE.md` so it sticks. They also flagged I'd been *using* "stack
frame" without ever teaching it, so we did.

**Covered (core 1A ground):**
- **The call stack / stack frames.** Every function call pushes a **frame**
  (activation record) holding that call's parameters, locals (the *actual bytes*,
  sitting right there), and bookkeeping (return address). Return → frame
  **popped**, memory reclaimed. It's **LIFO** — the running function's frame is
  always on top; a caller's frame persists the whole time anything it called is
  running, and is gone the instant it returns. **Python collision:** Python locals
  are names bound to heap objects the GC can keep alive past the call; in C the
  local's storage *is* the frame — when it pops, the bytes are gone, full stop,
  nothing can rescue them. Ishan answered the 3-deep prediction (main→a→b) cleanly.
- **Return-by-value.** Returning a struct **copies its bytes into the caller's
  storage at the moment of return, before the callee's frame is torn down** — so
  the caller reads its own copy, never the dead frame. Safe *because* it copies.
  Foreshadowed 1B: returning a *pointer* to a local would be the dangling-pointer
  trap (an address escaping into a demolished frame).
- **Struct init syntax** (brisk — mechanical): the **dot operator** (`v.field =`)
  and the **brace initializer** (`= {a, b}`, positional, fills in declaration
  order). Saw the brace form works with *runtime* variables, not just constants.
- **`()` vs `(void)`** (real nuance): empty `()` is "unspecified/unchecked arg
  list," not "no params" — `foo(99)` on `foo()` compiles clean; `foo(void)` makes
  it a hard error. Answered the `int main(void)` open thread (that's what the
  `void` does). Decided to **stay on `-std=c11`** (not upgrade to C23, which would
  hide exactly these learnable sharp edges; all reference material is pre-C23).

**Built — `p1a/retpair.c`:** `struct Pair {int x; int y;}`, a function
`ret(int x, int y)` that builds a local `{x,y}` and returns it **by value**,
`main(void)` catches the copy and prints it. First draft hardcoded `{3,4}` with a
no-arg `ret()`; review flagged (a) it didn't take inputs per the brief and (b) the
`()` gotcha — Ishan fixed both. Final: clean under `-Wall -Wextra`, prints
`x=5 y=10`.

**Banked 1 learning:** `(void)` not `()` for a no-parameter function
(`standard`, `python-collision`). (The return-by-value rule was understood but not
separately banked — it lives as a recall item instead.)

**Where I left off mid-thought:** nothing open; both `p1a/` programs build clean
and run. Next is storage duration (`static` vs automatic locals).

**2026-11-16 (session 11) — Phase 1A opens: a struct is a value, and padding.**
Overnight gap, so the quiz ran: 3 asked, 2 clean, 1 partial.
- **Q1 (arm64 Mac binary won't run on arm64 EC2 — why?):** correct — same CPU ≠
  same executable; Mac emits Mach-O + macOS syscalls, Linux only loads ELF.
- **Q2 (`clang -E` → 599 lines: what's in them, what isn't?):** correct — pasted
  header *declarations* (prototypes, typedefs, struct shapes, macros), comments
  stripped; **no function bodies** (printf's code isn't there).
- **Q3 (is printf's code copied into the binary?):** **partial** — got the
  headline (not copied, arrives at runtime) but muddled *which* linker does what.
  Retaught the two-linker split: the **static** linker (build time) leaves an
  IOU/stub for a dynamically-linked lib; the **dynamic** linker (`dyld`, every
  launch) maps the `.dylib` and resolves it. Mechanism stays a Phase-3 open thread.

**Covered (new — a real C nuance, not mechanical):**
- **A `struct` is a value, like an `int`.** `struct Pt b = a;` *copies all the
  bytes* into a separate block — writing `b.x` cannot touch `a`. The
  Python-reference collision: Python's `b = a` shares, C copies. The rule
  generalises: **assigning, passing to a function, *and returning* a struct all
  copy its bytes.** (Matters later when structs get big or own heap memory.)
- **`sizeof` is members + *padding*, not the sum.** Each member sits at an address
  that's a multiple of its own alignment (int→4, double→8) so the CPU reads it in
  one aligned access; the compiler inserts padding to guarantee that, and rounds
  the whole struct up to its own alignment (so arrays stay aligned). **Member
  declaration order changes the size** — largest-alignment-first minimises padding.
- Clarified: `sizeof(struct S1)` **names a type**, it does not construct/init a
  struct; `sizeof` is compile-time and does not evaluate its operand.

**Built — `p1a/sizes.c`:** defines S1–S4, prints each `sizeof` with `%zu`.
Predictions vs reality: S1 `{char,char,char}` → **3** (predicted 4; all-char =
align 1 = no padding); S2 `{char,double}` → **16** (predicted 12; double wants
8-align → 7 pad bytes, not 3); S3 `{char,int,char}` → **12** ✓; S4
`{int,char,char}` → **8** ✓. Same three members, S3 vs S4 = 12 vs 8 purely from
order — the payoff. Two code bugs on the way, both self-fixed: `sizeof(S1)` missing
the `struct` tag (undeclared-identifier), and a bare `%` instead of `%zu`.

**Banked 1 learning:** declare-largest-alignment-first / sizeof-is-members-plus-
padding (`alignment-aliasing`, `types`, `memory`) — first structural-leaning tag
of Phase 1.

**Where I left off mid-thought:** nothing open; `p1a/sizes.c` compiles clean under
`-Wall -Wextra` and runs. Next is the return-a-struct-by-value drill.

**2026-11-15 (session 10) — the Makefile; Phase 0 closed.** Same-day, no quiz.
Wrote a hand-written `Makefile` from scratch in `p0.5/` — first Makefile I'd ever
seen. Learned: a **rule** = `target : prerequisites` + a **tab-indented recipe**;
`make` rebuilds a target when it's **older than any prerequisite** (timestamps,
never contents); the first target runs on plain `make`; `clean`/`test` are
**phony** (named actions, no file). Got the tabs right first try (dodged the #1
trap). Hit real bugs and fixed them: leftover `#include "other.h"` debris from the
0.4 circular experiment; object rules missing `-c` (so they tried to link); the
header missing as a prerequisite; and `clean` using `rm` (fails on missing files)
instead of `rm -f`. Saw `make` print "up to date" on a no-op second run — the
whole point of the tool, witnessed. **Got overwhelmed** near the end when I piled
on the full flag set + sanitizers + variables at once — pulled back; the strict
flags/`asan`/`.PHONY` are deferred (see carry-over above). Banked 1 learning
(make = timestamps).

**2026-11-15 (session 9) — multi-file builds and headers (0.4 done).** Same-day
continuation, so no quiz. Split `maxarr` into `main.c` + `util.c` + `util.h`.

**Covered:**
- **Translation units:** the compiler compiles each `.c` *alone*, blind to the
  others. Cross-file calls need the **declaration** (via a header); the **linker**
  joins them. Three-file roles: `util.h` = declaration (shared, repeatable),
  `util.c` = definition (exactly one), `main.c` = user.
- **Two-step build:** `clang -c x.c -o x.o` (compile a TU → object) then
  `clang *.o -o prog` (link). Saw `.o` as Mach-O *object*, not executable.
- **One Definition Rule:** 0 defs → "undefined symbol"; 1 → links; 2+ → "duplicate
  symbol." Declarations repeat; **definitions** (function bodies, `struct`/typedef
  *with* braces) don't. A bare `struct Point;` is a declaration (repeatable); with
  members it's a definition (once only).
- **Headers:** must be **self-contained** (include own deps — `util.h` needs
  `<stddef.h>` for `size_t`) and **guarded** (`#ifndef UTIL_H …`, the name is a
  plain convention from the filename, not magic). Guard = idempotent: stops diamond
  duplicate-defs *and* circular-include recursion.
- **The linker matches by NAME, not type** — a mismatched decl/def links clean and
  becomes runtime UB; that's why a `.c` includes its own header.

**Built — `p0.4/main.c`, `util.c`, `util.h`:** `max_of_arr` split cleanly; header
ended textbook (guard + `<stddef.h>` + declaration only); `Max = 15`, clean under
`-Wall -Wextra`. Ran all four break-experiments and predicted each stage right.

**Banked 4 learnings:** translation-unit blindness; linker-matches-by-name-not-type;
include-guard idempotency; self-contained headers.

**Where I left off mid-thought:** nothing open. `p0.4/` builds clean. Strict flag
set still not adopted — that's literally the 0.5 task now. Next step (0.5, the
Makefile) **closes Phase 0**.

**2026-11-15 (session 8) — reading errors, and a deep detour into memory.**
1-day gap, so the quiz ran. Q1 was *"you deref `NULL` — why does it crash, and
what does the checking?"* **Missed:** said the *compiler* throws the error (the
compiler is gone at runtime). That miss opened into the session's real material —
**virtual memory**, which was entirely new ground (I'd thought addresses were
RAM). Quiz stopped after Q1 (no badgering).

**Covered (new, load-bearing for Phase 2):**
- **Virtual memory:** a pointer is a *virtual* address; the **MMU** translates
  virtual→physical every access via page tables; "unmapped" = no translation.
  `NULL` deref → MMU can't translate addr 0 → **page fault** (hardware trap) →
  kernel judges it illegal → **SIGSEGV**. A page fault isn't always fatal (lazy
  alloc / swap use the same mechanism). Processes are isolated because the page
  table is per-process.
- **0.3 proper:** broke code four ways; **compiler error** = undeclared, file:line;
  **linker error** = undefined symbol, no line. The dividing line is
  **declared (compiler) vs defined (linker)** — `pintf` (undeclared) is a compiler
  error, renamed `main` (valid but `_main` undefined) is a linker error.
- **`-Wall -Wextra`** caught `-Wsign-compare` on `i < sizeof(...)` → led into
  **signed/unsigned** conversion and **two's complement** (`-1` and `UINT_MAX`
  are the same bits; signedness is interpretation; the CPU uses one add/sub for
  both).

**Did — `p0.3/break.c`:** a working program broken on purpose (missing `;`,
undeclared var, `pintf`, renamed `main`), each diagnostic read and predicted;
then compiled clean with `-Wall -Wextra` vs bare to compare.

**Banked 4 learnings:** virtual-address/MMU; signed-vs-unsigned conversion;
two's-complement (same bits); declared-vs-defined.

**Where I left off mid-thought:** nothing open in the code. 0.3's strict flags
(`-Werror -Wshadow -Wconversion`, `-std=c11 -g`) not yet adopted — deferred to the
0.5 Makefile. Next real step is **0.4 (multi-file + headers)**.

**2026-11-08 → 2026-11-14 (session 7) — arrays, strings, and a deep dive that
finished 0.2.** Long session spanning a 6-day gap: taught on 11-08, the vowel
drill built and the string/`sizeof` deep-dives done on 11-14 (same-day gate meant
no recall quiz at the start; a soft brush-up on resume).

**Covered — the first genuinely un-Python topic:**
- **Arrays** = a contiguous block of N fixed slots; indexing is `start + i×size`
  arithmetic with **no bounds check**. The array **does not store its length** —
  out-of-bounds is UB (silent garbage, or a segfault, or a buffer-overflow write
  that clobbers a neighbour). `sizeof(arr)/sizeof(arr[0])` recovers N *only* where
  the real array is in scope.
- **`char` is a 1-byte integer** — `'a'` *is* `97`, `'\0'` *is* `0` (≠ `'0'` = 48);
  comparing chars is integer `==`. `sizeof(char) == 1` is guaranteed (though a
  "byte" is `CHAR_BIT`≥8 bits; 8 on this Mac).
- **C strings** = `char` array + a `'\0'` sentinel the compiler plants for you;
  loops walk until they hit it. The idiomatic stop is `sent[i] != '\0'`, *not*
  `i < sizeof` (which includes the terminator and breaks for oversized buffers or
  after decay).
- Deep dives (all banked): **`sizeof` is compile-time** (reads the static type,
  doesn't evaluate its operand); **a C string can't hold a `0` byte as data**
  (→ real data needs `(ptr, len)`); **`len` is O(1) / `strlen` is O(n)** because
  one stores the length and the other doesn't.

**Built — `p0.2/maxarr.c` and `p0.2/vowels.c`:**
- `maxarr.c`: computed `n` via `sizeof`, seeded `max = arr[0]`, `i < n` bound — all
  correct. Had a logic bug (compared `arr[i] > arr[i-1]`, the *neighbour*, not the
  running `max`); fixed to `arr[i] > max` after a failing test case `{3,20,2,5,1}`.
- `vowels.c`: counts correctly (5 for "The Quick Brown Fox", handling both cases
  via position arithmetic). **Left with known, unfixed hygiene issues** (noted, not
  correctness-critical): loops `i < sizeof` so it scans the `'\0'` instead of
  stopping at it; off-by-one upper bounds (`<= 91`/`<= 123` include `'['`/`'{'`);
  a leftover "Not a vowel" debug print; missing `\n`.

**Where I left off mid-thought:** 0.2 is done; `vowels.c` hygiene fixes are
optional and still pending if I want them. `p0.1/hello.c` still missing `return 0`
(harmless). Several `p0.2/` throwaway experiment files (`sigint.c`, `segv.c`,
`collision.c`) are bin-able. **All the `git mv` + new `p0.2/` files are still
staged/uncommitted** — a commit is overdue.

**2026-11-08 (session 6) — nested loops, finally: the triangle built.** Quick
2-question quiz (only the pipeline "which stage understands C" one got asked —
answered correctly, "compiler only"; I over-drilled it, which prompted the pacing
feedback below). Then taught **nested loops** via the clock model (outer = hours,
inner = minutes runs a full cycle per outer step), with the triangle's twist: the
inner loop's bound is **not constant** — it's the *current outer counter*. Built
`p0.2/triangle.c` — `for i`, inner `for j` with `j <= i`, `\n` after each row —
correct 4-row output, unaided. Review finding (not fixed): `int n = 4` is declared
but unused; the loop hard-codes `i <= 4` (magic number) instead of `n`, so `n` is
dead and the triangle won't respond if you change it.

**Two course-corrections tuned in today (both live):**
- `CLAUDE.md` item 1 — **spend depth where C is hard** (memory/pointers/internals/
  Python-divergence), move briskly through trivial Python-like syntax; name which
  kind of topic we're on.
- `/teach` step 3 — **recall is gap-gated**: same-day resumes skip the quiz; the
  cold 2-3-question quiz only fires after an overnight-or-longer gap. (Today was
  4 same-day sessions each opening with a quiz — that's what prompted it.)

**Where I left off mid-thought:** nothing open. Two real drills done (`sum.c`,
`triangle.c`); throwaway experiment files (`sigint.c`, `segv.c`, `collision.c`)
still in `p0.2/`, bin-able. `p0.1/hello.c` still missing `return 0;` (harmless).
The `git mv` into `p0.1/`/`p0.2/` is **still staged, not committed** (now also
several new `p0.2/` files + the triangle). Next up is **arrays**.

**2026-11-08 (session 5) — recall reteach (exit status / signals), then a real
surprise (divide-by-zero on arm64).** Cold quiz Q1 was *"`return 3` but `echo $?`
says 130 — decompose it."* Missed: said `127 + 3` and "above 127 isn't an error
code" — wrong boundary and vague meaning. Reteach landed the model: `$?` is one
8-bit number saying *how* a process died — **path 1** it returned its own code
(`0` success), **path 2** it was killed by signal `N`, which the shell reports as
**`128 + N`**. So `130 = 128 + 2` = SIGINT (Ctrl-C); predicted `139 = 128 + 11`
for SIGSEGV correctly. Then the planned signal experiments turned up something
better: my divide-by-zero experiment (meant to SIGFPE → `136`) **didn't crash on
arm64** — it printed `0` and exited `0`. That's the banked surprise: integer
÷0 is UB, and arm64 defines it as "return 0" where x86 traps. Also had to
re-explain what **UB** even is from scratch — the acronym hadn't stuck despite
three entries using it. Nested loops deferred again.

**Built — throwaway experiments in `p0.2/` (safe to delete):**
- `sigint.c` (infinite loop, Ctrl-C → `130`), `segv.c` (actually divide-by-zero,
  printed `0`, exited `0` on arm64 — no trap), `collision.c` (the global-`FILE`
  redefinition from last session). None are drills.
- **Review note on `segv.c`:** first version computed `int c = a/b;` but never
  *used* `c`, so the division was dead code the compiler could delete; had to add
  a `printf` of `c` to make it observable — and even then, no crash (the arm64
  point).

**One banked learning:** *integer divide-by-zero is UB — may or may not crash,
depending on the machine* (`ub`, `integer`, `python-collision`).

**Where I left off mid-thought:** nothing open; the real drill (`p0.2/sum.c`) is
still the only one done. `p0.1/hello.c` still missing its `return 0;` (harmless).
The `git mv` into `p0.1/`/`p0.2/` is **still staged, not committed.** Throwaway
experiment files are sitting in `p0.2/` and can be binned.

**2026-11-08 (session 4) — a recall reteach: `#include` vs `import`.** Short
session. Opened the cold quiz with *"give three ways `#include` differs from
`import`"* and drew a blank — **couldn't retrieve it at all**, though it was
banked back on 2026-10-18. Per protocol that miss became the whole session; the
planned new material (nested loops) was pushed to next time. Fully recovered by
rebuilding it from the `clang -E` fact (4 lines → 599):

- **Difference 1 — caching.** `import` runs once and caches; `#include` text-pastes
  the header *every time*, no cache (→ why include guards exist; `stdio.h` has
  them internally). I got this one by reasoning, not recall.
- **Difference 2 — no namespace** (the one I kept missing). `#include` dumps every
  name flat into my file's global scope — no `stdio.` prefix. I first mis-blamed
  guards for a name collision; needed a "one sheet of paper, two things named
  `FILE`" picture before it landed. This is the same *one-name-one-scope* rule
  that bit `sum` last session, now at file scope.
- **Difference 3 — shape vs code.** `import` brings runnable code; `#include`
  brings only *declarations* (shapes). The body arrives later from the linker.

**Did — no new binary, a confirming experiment instead:** declared a global
`FILE` after `#include <stdio.h>` and compiled it; got the expected **compile-time**
redefinition error (identified the stage correctly from the `file:line`, applying
last session's prefix rule), with clang's "previous definition is here" pointing
into `stdio.h`. Prediction matched — nothing surprising, no `LEARNINGS.md` entry.

**Where I left off mid-thought:** nothing open. No new code written this session;
`p0.2/sum.c` still the only drill done. `p0.1/hello.c` still has no `return 0;`
(harmless warning). The `git mv` into `p0.1/`/`p0.2/` is **still staged, not
committed**. Nested loops + the triangle are the very next thing.

## Open threads

*Things I half-understand, deferred questions, "come back to this". Claude
should offer to close one of these when a session has slack at the end.*

- **Dynamic linking.** The linker may not copy `printf`'s body in at all — on
  macOS it records a reference that a *dynamic linker* resolves at launch, every
  run. I have the sentence, not the mechanism. Properly met in Phase 3 with
  `otool -L`, and it partly explains the 33KB thread below.
- **Why is a 4-line program's binary ~33KB?** Theory formed, never checked.
  Phase 3, with `nm`.
- ~~**`int main(void)` — what is the `void` doing?**~~ **CLOSED 2026-11-16:** the
  `void` explicitly declares zero parameters; empty `()` would mean "unspecified/
  unchecked," not "none." (The `argv` form of `main` is still deferred, but the
  `void` question itself is answered.)
- **UB and the optimiser.** I know "the compiler may assume UB never happens" as
  a sentence, not as something I've watched happen. Revisit at `-O2` in Phase 2.
- **Not banked:** the second rule from the linker output — *the prefix on a
  diagnostic names the stage that produced it, and a warning and an error in one
  output can come from different stages.* Offered twice, not written up. It's in
  the recall queue anyway.
- **Array → pointer "decay", and `char *p = "..."`.** Keep hearing that passing an
  array to a function turns it into a pointer (so `sizeof` starts lying), and that
  a string literal assigned to `char *` is read-only and writing to it is UB — but
  I've only got the sentences, not the mechanism or the memory picture. This is
  *the* Phase 1B topic; met properly there.
- **Makefile polish (carry-over from 0.5).** The `p0.5/Makefile` works but is
  minimal — no strict flag set, no `asan` target, no `.PHONY`. Round it out early
  in Phase 1 (and once `-Werror -Wconversion` go in, the `int n = sizeof(...)` in
  `main.c` may turn from a warning into a hard error — a real fix, not a surprise).

## Recall queue

*Spaced repetition. **After a real gap (overnight or longer — not a same-day
session),** `/teach` opens by asking me 2-3 of these cold, oldest-asked first; a
same-day resume skips the quiz or asks at most one older item. If I can't answer,
that's today's intake, not new material. Seeded from `LEARNINGS.md`. Claude
retires one after I've answered it correctly twice on different days.*

| Question | Added | Last asked | Correct streak |
| --- | --- | --- | --- |
| Name the four stages between `hello.c` and `hello`. What does each one consume and produce? | 2026-10-18 | 2026-11-08 | 1 |
| `clang -E hello.c` turns 4 lines into 599. What is in those lines — and what is definitely *not*? | 2026-10-18 | 2026-11-16 | 1 |
| Your binary is arm64 and your EC2 box is arm64. Why won't it run there? | 2026-10-18 | 2026-11-16 | 1 |
| Give three ways `#include` differs from `import`. | 2026-10-18 | 2026-11-08 | 0 — **missed cold, retaught** (no-namespace one was the sticky part) |
| Why does dereferencing `NULL` usually crash, and what is doing the checking? | 2026-10-18 | 2026-11-15 | 0 — **missed cold, retaught** (said "compiler"; it's the MMU → page fault → kernel → SIGSEGV) |
| `printf` is handed a value. How does it work out what type it is — and what happens when you're wrong? | 2026-10-18 | — | 0 |
| A program returns `3`, but `echo $?` says `130`. What happened, and what does `130` decompose into? | 2026-10-18 | 2026-11-08 | 0 — **missed cold, retaught** (said `127+3`; it's `128+2` = killed by SIGINT) |
| A build prints a warning with a `file:line` and an error prefixed `ld:`. Which one stopped the build, and what does each prefix tell you? | 2026-10-18 | 2026-11-08 | 1 |
| You rename `main` to `run` and rebuild. Which stage fails — and will the message name a file and line, or a symbol? | 2026-10-18 | — | 0 |
| Is `printf`'s machine code copied into your binary when you link? If not, what is, and when does the real thing arrive? | 2026-10-18 | 2026-11-16 | 0 — **partial**: got "not copied, arrives at runtime" but muddled the two linkers (static leaves a stub; `dyld` resolves the `.dylib` at launch) |
| Which of the four stages is the only one that understands C? What does that imply about where a syntax error can come from? | 2026-10-18 | 2026-11-08 | 1 |
| A C program ran, printed what you expected, and exited `0`. What does that prove? | 2026-10-18 | 2026-10-18 | 1 |
| A function returning `int` reaches its closing brace with no `return`. When is that defined, and when is it not? | 2026-10-18 | 2026-10-18 | 1 |
| `double r = 5 / 2;` — what is `r`, and why isn't it `2.5`? | 2026-11-08 | — | 0 |
| You declare a local variable with the same name as the function it lives in. What can you no longer do below it, and what error names the problem? | 2026-11-08 | — | 0 |
| `sum(100000)` returns a positive number far below the true sum and exits `0`. Where did the missing value go, and why did nothing complain? | 2026-11-08 | — | 0 |
| You divide an `int` by a variable holding `0` and run it on your arm64 Mac. Does it crash? What would an x86 box do, and what one concept explains why both are "correct"? | 2026-11-08 | — | 0 |
| What does *undefined behaviour* mean in C — and why is it a different claim from "you'll get an unpredictable result"? | 2026-11-08 | — | 0 |
| At what point is `sizeof(x)` computed, and does it evaluate `x`? What does that mean for `sizeof(x++)`? | 2026-11-14 | — | 0 |
| Why can't a C null-terminated string hold a `0` byte in the middle, and what must you use for data (like an HTTP body) that might contain one? | 2026-11-14 | — | 0 |
| Python `len(s)` and C `strlen(s)` — which is O(1), which is O(n), and what single fact about each explains the difference? | 2026-11-14 | — | 0 |
| You deref `NULL` and get SIGSEGV. Walk the chain: what spots the bad access, what does it raise, and who actually sends the signal? Is a page fault always fatal? | 2026-11-15 | — | 0 |
| `for (size_t i = len-1; i >= 0; i--)` — what goes wrong, and why? | 2026-11-15 | — | 0 |
| `-1` as a signed `int` and `UINT_MAX` as unsigned — how do their bit patterns compare, and what does that tell you about where "signedness" lives? | 2026-11-15 | — | 0 |
| A misspelled function *call* vs a renamed `main`: which fails at the compiler, which at the linker, and what one-word distinction decides it? | 2026-11-15 | — | 0 |
| When the compiler compiles `main.c`, what can it see of `util.c`? So how can `main.c` legally call a function whose body is in `util.c`? | 2026-11-15 | — | 0 |
| A header declares `int add(int,int)` but the `.c` defines it as `double add(int,int)` and the `.c` doesn't include the header. Does it link? What happens at runtime, and why didn't anything catch it? | 2026-11-15 | — | 0 |
| An include guard prevents *two* different failures — name both, and say which header contents actually need protecting (and which don't). | 2026-11-15 | — | 0 |
| You can write `struct Point;` twice but not `struct Point { int x; };` twice. Why — what's the difference, in one word? | 2026-11-15 | — | 0 |
| `make` runs, does its thing; you run `make` again immediately and it says "up to date." What did it compare to decide there was no work — and what did it *not* look at? | 2026-11-15 | — | 0 |
| In Python `b = a` makes two objects share; in C `struct Pt b = a;` — share or copy? What happens to `a`'s bytes when you then write `b.x`, and what else (besides `=`) silently copies a struct? | 2026-11-16 | — | 0 |
| The same three members (`int`, `char`, `char`) declared in two different orders give `sizeof` 12 and 8. Why the difference — and which member should come first, and why? | 2026-11-16 | — | 0 |
| `main` calls `a`, `a` calls `b`, and `b` is running. Whose frames exist right now? Is `a`'s local alive? And what happens to it the instant `a` returns — why? | 2026-11-16 | — | 0 |
| A function builds a local `struct` and returns it *by value*; that local dies when the function returns. How does the caller still get a valid struct — where does the copy go, and *when* relative to the frame being torn down? Why would returning a *pointer* to that local be a disaster instead? | 2026-11-16 | — | 0 |
| In C, what's the difference between declaring `foo()` and `foo(void)`? Which one lets `foo(99)` compile silently, and which makes it an error — and why does the trap exist? | 2026-11-16 | — | 0 |

## Phase progress

- [x] **P0** Toolchain and the mental model — *complete (0.1–0.5). Exit test passed: four-stage pipeline explained cold.*
- [ ] **P1** Core language, drilled
- [ ] **P2** Memory ★
- [ ] **P3** Data structures (`libcore`)
- [ ] **P4** The OS boundary
- [ ] **P5** Protocols hand-rolled (`libhttp`, `libjson`)
- [ ] **P6** Persistence + outside world
- [ ] **P7** Strangler rewrite
- [ ] **P8** Production hardening

## Tag tally to date

*Updated by `/wrap` from `LEARNINGS.md`. The shape of this table over time is the
real measure of progress — mechanical memory tags should give way to structural
ones by Phase 5.*

31 entries.

| Tag | Count |
| --- | --- |
| `python-collision` | 15 |
| `types` | 10 |
| `toolchain` | 10 |
| `ub` | 6 |
| `standard` | 5 |
| `integer` | 4 |
| `process` | 3 |
| `memory` | 3 |
| `alignment-aliasing` | 2 |
