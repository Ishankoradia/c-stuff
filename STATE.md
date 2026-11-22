# STATE — where we are

**Single source of truth for the C training.** Claude reads this first in every
session; `/wrap` rewrites it at the end of every session. If this file and my
memory disagree, this file wins.

Companions: `ROADMAP.md` (curriculum + protocol), `LEARNINGS.md` (lab notebook),
`CLAUDE.md` (teaching contract), `TUNING.md` (course corrections).

---

## Position

- **Phase:** 1 — Core language, drilled → **1A COMPLETE ✅, in 1B** (pointers —
  *the wall*). **Phase 0 is COMPLETE.** 🎉
- **Task in flight:** none — **1B pointer basics + pointers-as-parameters are
  done**. Basics (session 19): a pointer is a **typed address** (starting-byte
  number + compile-time type = the "decoder ring" for how many bytes `*p` reads);
  `&`/`*` are inverses; `*p = v` mutates the real variable; `sizeof` any pointer =
  **8 bytes** (address-sized, pointee-independent); `void *` = type **stripped**
  (can't be dereferenced; cast back to use; `%p` takes `void *`). **Parameters
  (session 20):** C is **pass-by-value** (callee gets a *copy*, can't touch the
  caller's variable), so to let a function modify a caller's variable you pass its
  **address** and dereference it to reach back into the caller's frame; **the
  pointer itself is still passed by value** (a *copy of the address*, which still
  points at the same storage) — that's why C has **no true pass-by-reference**.
  Built `p1b/ptr.c`, `p1b/swap.c`, `p1b/null.c`; all ran clean/as-intended under
  `-fsanitize=address,undefined`. **`NULL` is done (session 21):** a "points at
  nothing" sentinel (C's `None`), literally address `0`; deref → *deterministic*
  SIGSEGV at `0x0` (unmapped page) — **not** garbage; the *uninitialized/wild*
  pointer is the dangerous one (garbage address → possible silent corruption that
  passes tests); so **init pointers to `NULL` and null-check before deref**. Also
  cracked *how a runtime crash prints a source line* (UBSan bakes each deref site's
  file:line into the binary at compile time; `-g` writes the DWARF/`.dSYM` map).
  **Tooling met:** `-g` (DWARF debug info → readable reports), `-O0` (no
  optimization → *accurate* line-mapping), `-fsanitize=address,undefined` (ASan
  runtime mem-bug catcher + UBSan runtime UB catcher); macOS auto-runs `dsymutil`
  under `-g` → a **`.dSYM` bundle** (separate debug symbols; build output →
  `.gitignore` + `clean` should `rm -rf *.dSYM`).
  **Carry-over (not blocking):** the `p0.5/Makefile` still lacks the strict flag
  set, an `asan` target, `.PHONY`, and a `*.dSYM` clean. **From here on, build with
  the sanitizers on as a reflex** (nothing allocates until Phase 2, but the habit
  is the point; it becomes an absolute gate the moment there's heap).
- **Next action:** cash in the **two parked payoffs** — both small, both reinforce
  pointers *and* revisit earlier topics, good for one session: (1) **cast-away-
  `const`** (parked since session 17): take `&x` on a `const int`, cast the `const`
  off, write through the pointer — observe whether the write "sticks," segfaults, or
  seems ignored (UB; the object may be in read-only memory, or the value was baked
  in at reads). (2) **`&outer_x` shadow-proof** (parked session 18): grab `&outer_x`
  *before* an inner shadowing block, then reach the outer `5` through that pointer
  from *inside* the block — proving the shadowed variable's storage never moved
  (storage ≠ name-visibility, made concrete). **After these:** pointer arithmetic +
  array→pointer decay (the big 1B topic — its own session; the long-standing open
  thread). Primary source: **K. N. King §11.1–11.5, §12** (arrays/pointers). Still
  outstanding from 1A: the unsigned-wraparound / `-Wconversion` drill + Makefile
  strict-flag polish (fold into one tooling session).
- **Sessions this phase:** 11 (1B: 3)
- **1B exit test:** given any pointer drill, state for each pointer — what it
  points at, who owns that memory, and when it dies. (From 1B on: sanitizer-clean,
  zero leaks, no partial credit.)

## Last session

**2026-11-22 (session 21, FIFTH session of the day — deliberately light,
consolidating material, which suits it; didn't re-run the rested-brain fork, just
noted it) — `NULL`.** Same-day, no quiz.

**Covered (spent the depth on purpose/danger, since they'd already cracked the
crash mechanism in the session-20 mull):**
- **What `NULL` is *for*:** the "no object here" **sentinel** — C's `None`. Used
  as a return for not-found, **`malloc` failure** (Phase 2 foreshadow), end-of-list
  (Phase 3), optional/absent. `NULL` is a macro (`<stddef.h>`) ≈ address `0`. The
  discipline: **null-check before dereferencing** any maybe-null pointer.
- **`NULL` vs an uninitialized ("wild") pointer** — the load-bearing distinction
  they nailed: `NULL` deref = *deterministic, loud* SIGSEGV at `0x0`; an
  uninitialized pointer holds a *garbage address* → may crash, or **worse**, hit
  valid-looking memory → *silent* corruption that passes tests. So `NULL` is the
  *safer* bad value (fails fast at the scene); defensive C **initializes pointers
  to `NULL`**. Python parallel: `None` deref is a *catchable* `AttributeError`; C's
  is an *uncatchable* SIGSEGV.
- **Their unprompted question → a real toolchain insight:** "it's a *runtime*
  crash, how did we get a line number?" Taught: the line is **compile-time
  knowledge serialized into the binary** — UBSan's `-fsanitize=null` instrumentation
  records each deref site's file:line as embedded data at compile time; `-g` writes
  the DWARF address→line map (the `.dSYM`). Runtime tools print back what was
  stored. Strip `-g` + sanitizers → bare `segmentation fault`, no line. This is the
  deliberate exception to their "the compiler is gone at runtime" model.

**Built — `p1b/null.c`:** `int *p = NULL;` printed as `0x0`; deliberate deref →
watched the **deterministic SIGSEGV at `0x0`** under `-fsanitize=address,undefined
-g -O0`, and *read the named sanitizer report* (the exercise was reading the
report, not debugging the crash).

**Banked 2 learnings** (via `/learnings`, their words): (1) `NULL` fails loud vs an
uninitialized pointer failing silent → init to `NULL` + null-check (`memory`, `ub`,
`python-collision`); (2) a runtime crash can name a source line only because
`-g`/UBSan wrote the compiler's notes into the binary (`toolchain`, `process`).
**Now 36 entries.**

**Where I left off mid-thought:** nothing open; `null.c` done, both learnings
banked. Next: the two parked payoffs (cast-away-`const`, `&outer_x` shadow-proof).

**2026-11-22 (session 20, FOURTH session of the day — they said "quite fresh" and
chose to push into new material; I offered a lighter consolidation path (the two
parked payoffs) vs the new concept and recommended consolidation, they picked the
new concept; went fine) — pointers as function parameters.** Same-day, no quiz.

**Covered (the core 1B payoff):**
- **C is pass-by-value**, always: a function receives a *copy* of each argument in
  its own frame, so reassigning a parameter is purely local and dies on return —
  it cannot change the caller's variable. (Built straight on their own session-19
  mull answer: `inc(x)` leaves `x` at 5.)
- **To modify a caller's variable, pass its address** (a pointer). The parameter
  type is a pointer; inside, `*p = …` **dereferences back into the caller's frame**
  and writes the real variable.
- **The subtlety (stated explicitly):** the pointer *itself* is still passed **by
  value** — the callee gets a *copy of the address*, but a copy of an address still
  points at the same storage, so you reach the original through it. Hence **C has
  no true pass-by-reference**; you simulate it by passing a pointer by value.
- **Python parallel used:** `def f(n): n=n+1` (caller's int unchanged — local
  rebind = pass-by-value) vs `def g(lst): lst.append(1)` (caller's list changed —
  reached the shared object). C's pointer-parameter makes `g`'s "reach the real
  thing" explicit via the address.
- Drew the two-frames diagram (caller's `x`@A0/`y`@B0; swap's `a`/`b` holding
  *copies* of those addresses; `*a`/`*b` reaching across).

**Predictions (both correct, cold):** (1) `swap(int,int)` leaves the caller
unchanged — works on copies; (2) the pointer swap must move values via `*a`/`*b`,
**not** `a`/`b` — swapping the local pointers themselves does nothing the caller
can see (they die on return). Their phrasing on (2) was spot-on ("swapping a & b is
all scoped in that function").

**Built — `p1b/swap.c`:** `swap(int *, int *)` with an `int` temp, exchanging
`*a`/`*b`; `main` declares two ints, prints, calls `swap(&x,&y)`, prints — values
actually exchanged. **First program built + run under
`-fsanitize=address,undefined -g -O0`** — ran clean.

**Tooling taught (build/flags — the carve-out, I explained these):** `-g` (DWARF
debug info, makes reports readable), `-O0` (optimizations off → line-mapping stays
accurate for debugging/sanitizers; foreshadowed the `-O2` + UB-optimizer open
thread), `-fsanitize=address,undefined` (ASan runtime memory-bug catcher + UBSan
runtime UB catcher — the "sanitizers always" rule; `swap` reports nothing, the
point is the *reflex*). Explained the **`swap.dSYM`** bundle they noticed: macOS
auto-runs `dsymutil` under `-g` to package debug symbols separately (Mach-O keeps
them out of the lean executable); build output, belongs in `.gitignore` + `clean`.

**Banked 0 learnings** — nothing was reported as a surprise; the pass-by-value /
pointer-parameter model is a strong `/learnings` candidate (their words), as is the
`-g`/`-O0`/sanitizer tooling, left to them.

**Where I left off mid-thought:** nothing open; `swap.c` runs clean. Next session:
the owed `NULL` + the two parked payoffs (cast-away-`const`, `&outer_x`
shadow-proof), then pointer arithmetic / array decay.

**2026-11-22 (session 19, third session of the day — they confirmed they were
"super fresh" and chose to open the pointer wall rather than do lighter carry-over
work; I flagged the rested-brain concern per my own STATE note, they overrode it,
and it went fine) — PHASE 1B OPENS: what a pointer *is*.** Same-day, no quiz.

**Covered (the core 1B model — slowed right down, this is *the wall*):**
- **A pointer is a typed address.** Led with the Python collision as the frame:
  in Python *every* variable is already a hidden reference (`b = a` shares one
  object, address untouchable); in C values are direct bytes (they knew this from
  struct-copy), so a pointer is the **opt-in**, explicit, manipulable reference —
  "I want the *location*, not the value." `&x` = address-of, `*p` = dereference
  (follow the address), and they're **inverses** (`*(&x) == x`).
- **The handle:** `*p = 99` walks to `x`'s address and overwrites `x`'s storage —
  so plain `x` reads 99 afterward. One variable reached through another. (Flagged
  this as the seed of pointer-as-function-parameter, next session.)
- **The type is a "decoder ring."** The big clarifying beat: an **address names
  only the *starting* byte** — it carries no length. `*p` needs two things: *where
  to start* (the address `p` holds) and *how many bytes to read + how to interpret*
  (the **type**, compile-time, not stored anywhere). `int *` → read 4 bytes as int;
  `char *` → read 1 byte as char. They half-stated this themselves ("we only know
  the starting address, so the type helps?") — I confirmed with the stadium-seat
  analogy (a seat number points at one seat; the booking *type* says how many
  seats the group spans).
- **`sizeof(int *) == sizeof(char *)` — prediction.** They correctly reasoned
  "same," because a pointer just stores *one starting address*; the pointee type
  changes how `*p` reads, never the pointer's own size. Confirmed = **8** on their
  arm64 (64-bit) Mac. (Named the classic trap they *avoided*: thinking `int *`=4,
  `char *`=1 — confusing pointer with pointee.)
- **`void *`** (came up from the `%p` cast; taught as a natural corollary): a
  pointer with the **type stripped off** — address, *no* decoder ring. The
  consequence that makes it click: **you cannot dereference a `void *`** (`*vp` is
  a compile error — the compiler has no "how many bytes"), which is just the
  decoder-ring rule run backwards. To use it, cast back to a typed pointer. It's
  for **generic** code (foreshadowed `malloc`/`memcpy`/the bubble-sort comparator).
  And `%p` is specified to take `void *` (a bare address, no type) — which is why
  the "correct" print is `printf("%p", (void *)&x)`.

**Built — `p1b/ptr.c`** (clean under `-Wall -Wextra -std=c11`): declared
`int x = 5; int *p = &x;`, printed `&x` and `p` (same address, `0x16f43eb9c`),
`*p` (5), then `*p = 99` → plain `x` printed 99, and `sizeof(int *)`/`sizeof(char *)`
both 8. First draft printed `&x`/`p` with bare `int *` under `%p` (built clean, but
I flagged it as the *same class* of format mismatch as `%d`-on-`double`, just
benign); they **updated it to cast both to `(void *)`** — rebuilt clean.

**Banked 0 learnings** — but the **decoder-ring / `void *`-can't-deref** insight
is a strong `/learnings` candidate if it felt like it clicked; left to them. The
`sizeof(ptr)` = pointee-independent point is another. Nothing was reported as a
surprise.

**Where I left off mid-thought:** they didn't give the final one-sentence
verbal check ("why `*p` works on `int *` but `*vp` is rejected on `void *`"), but
the cast they wrote proves they got it — not an open gap. `ptr.c` runs. Next
session: **pointers as function parameters** (`swap`), plus the owed `NULL`.

**2026-11-22 (session 18, same-day continuation started after session 17's wrap,
rolled past midnight) — scope/shadowing, then the 1A exit test → 1A COMPLETE.**
Same-day, so no quiz.

**Covered (scope/shadowing — mechanically light, moved briskly):**
- C has **block scope**: a name declared inside `{}` (an `if` body, a `for`) dies
  at the closing `}`. **Python collision:** Python has **no block scope** — a name
  bound inside an `if`/`for` leaks to the whole enclosing function (LEGB = Local is
  *function*-level). So `for (int i…){}` then reading `i` after the loop is a
  **compile-time** error in C (`use of undeclared identifier 'i'`), while Python's
  `print(i)` after the loop is fine. They predicted this cold, correctly.
- **Shadowing:** an inner block may redeclare an outer name; inside, the name
  resolves to the *nearest* (inner) one; the outer is **unreachable by that name
  but not destroyed** — it's still alive, two distinct objects/addresses coexist.
  They got all of this unprompted.
- **`-Wshadow` is opt-in and NOT in `-Wall`/`-Wextra`** — the classic silent
  footgun (declare a fresh inner `total`, write to it, outer `total` stays 0, no
  diagnostic) compiles clean under their usual flags. The flag is in the deferred
  Makefile strict set. **They chose to skip the hands-on shadow drill** ("too easy,
  I understand scope and lifetime") — allowed, since they'd predicted every outcome
  correctly; the binary-every-session rule is satisfied by `chain.c` below.

**The 1A exit test — `p1a/chain.c` (built + run):** `main → a → b`, three deep,
each frame with a local; `b` prints at the bottom. Notable: they wrote `a` so that
`b()` is called **before** `int x = 20;` is declared — which made the exit test
sharper than the generic case.

**Exit-test gate (passed, with one reteach):**
- Frames + teardown: correct cold — all three frames (`main`+`a`+`b`) coexist at
  `b`'s `printf`; `a`'s frame is alive the whole time `b` runs (it's waiting on
  `b`); LIFO teardown `b`→`a`→`main`. Also self-corrected a slip (they said "main's
  print" — `main` has no `printf`, it just falls off the end → returns 0).
- **The reteach (load-bearing):** asked whether, at `b`'s `printf` moment, `a`'s
  `x` **(a)** doesn't exist yet or **(b)** exists but holds garbage. They first
  picked **(a)** ("x doesn't exist, we called b before it"). Retaught using their
  own words — they'd already said *"a's frame is created"* on entry to `a`; the
  frame **is** the storage for all of `a`'s locals (size fixed by the compiler, SP
  dropped once on entry), so `x`'s **slot exists from frame creation**, holding
  **garbage**, until the `= 20` line runs (after `b` returns). The answer is **(b)**.
  Separated three axes: **storage** (bytes exist — from block entry), **scope /
  name-visibility** (textual — begins at the declaration line, so you can't *name*
  `x` at the `b()` call site), **initialization** (runtime — when control reaches
  the line). Re-probe landed: "if `a` printed `x` before the init line → UB, an
  unpredictable number." Gate closed.

**Banked 0 learnings so far** — but the **storage-vs-initialization** correction
is a genuine belief-replacement (they held "declared-after = doesn't exist yet")
and is a strong `/learnings` candidate if it felt like a surprise; left to them to
write in their own words. Nothing else surprised them.

**Where I left off mid-thought:** nothing open; 1A is closed, `chain.c` runs, the
exit test passed. Next session **opens 1B (pointers)** — a fresh rested-brain
session, explicitly not a same-day continuation.

**2026-11-21 (session 17) — `const` on plain values.** 3-day gap, so the quiz ran
properly (3 cold, oldest-asked first).

**Recall quiz:**
- **Q1 (name the four stages, consume/produce each):** **pass** — all four in
  order with the right consume/produce. One imprecision tightened: the **compiler
  produces assembly text (`.s`)**, not an object file; the **assembler** produces
  the `.o`. They'd written "assembly object file," blurring the 2→3 handoff.
  Streak → 2, **retired** (twice-correct on different days; the `.s`/`.o` blur is a
  tightening, not a miss — the pipeline is solid).
- **Q2 (`return 3` but `$?` is 130 — decompose):** **clean recovery** from the
  earlier miss — `130 = 128 + 2`, SIGINT, Ctrl-C killed it before the return ran.
  Streak 0 → 1.
- **Q3 (`file:line` diagnostic vs `ld:` — which stopped the build, which stage):**
  **partial.** Got the **stage mapping right** (`file:line` → compiler, `ld:` →
  linker) but was shaky on **stage vs severity** — I under-asked it (dropped the
  "warning") so this is partly on me. Retaught the nuance: **the prefix names the
  stage, not whether it stopped the build** — that's `warning:` vs `error:`; one
  build's output can carry a compiler *warning* AND a linker *error* at once
  because the warning didn't halt the compile. Streak stays 0 (partial).

**Covered (new — `const`, mechanically light, one real nuance):**
- A write through a `const`-qualified name is a **compile-time error** (clang:
  `cannot assign to variable 'x' with const-qualified type`) — enforced by
  *refusing to build*, nothing at runtime. **Python collision:** Python has no
  real `const`, only the ALL_CAPS *convention* nothing enforces (`MAX = 6` just
  rebinds).
- **The nuance they nailed unprompted:** `const` is a **promise made to the
  compiler, not a lock on the storage**. It does not protect those bytes from
  another path reaching them. Consequence (foreshadowed, not built): you can
  **cast the `const` away** and write through the resulting pointer — the compiler
  lets it compile, but if the object was genuinely declared `const`, the write is
  **UB** (may sit in read-only memory → segfault; or the compiler baked in the old
  value → your write "vanishes" on every read). Named the recurring C shape: *the
  compiler trusts a promise it can't verify at runtime, and breaking it is UB, not
  a caught error* (same shape as `printf`'s format string).

**Built — `p1a/constness.c`:** declared a `const int`, assigned to it on the next
line, read the exact compile error, confirmed the stage (`file:line`, compiler),
then commented the bad line out so it builds and prints. Error landed exactly as
predicted (compile-time, build refused).

**Scope-cut mid-session (good call):** the planned drill had a part 2 —
*defeat `const` by casting it away* — which needs pointer syntax (`&`, `*`, casts)
they haven't drilled. That's **1B** material; I'd reached past where they are. Cut
it to the pure-1A part (declare, assign, read the error) and **parked the
cast-away-`const` demo as 1B's opening treat** (the honest way to meet pointers,
with K&R-King §11.1–11.2 / §7.4 as the source, not from me). Kept the
one-concept-per-session discipline tight — their own retro flags the
overwhelm-when-piled-on risk.

**Banked 0 learnings** — nothing surprised them; `const` matched predictions. No
forced entry.

**Where I left off mid-thought:** nothing open; `constness.c` builds and runs,
error confirmed. Next is **scope/shadowing**, then the 1A exit test.

**2026-11-18 (session 16, same-day continuation after session 15's wrap) —
`enum` + a state-machine drill.** Same-day, no quiz.

**Covered (mechanically light + one real Python collision):**
- An `enum` makes **named integer constants** — `enum State { LOCKED, UNLOCKED };`
  gives `LOCKED`==0, `UNLOCKED`==1 (auto-numbered from 0, overridable like
  `{ A=5, B, C }` → 6,7). An `enum`-typed variable is just a single `int`; there's
  **no array/object stored** for the "set" — the names vanish into int literals at
  compile time (like scoped, auto-numbered `#define`s).
- **Python collision (the slow-down):** a C `enum` has **no runtime type safety** —
  `int x = UNLOCKED;` is fine, and `enum State s = 99;` compiles and stores 99, a
  state the enum never named. Opposite of Python's `Enum` (distinct guarded member
  objects). Payoff of C enums = readability + a `-Wswitch` warning on a missed
  `switch` case, **not** safety.

**Built — `p1a/turnstile.c`:** two states (LOCKED/UNLOCKED) + two inputs
(COIN/PUSH); `StateCalc(enum State, enum Input)` returns the next state; `main`
drives a fixed sequence, printing `1,0,1,0` (correct).

**Bug hit + fixed (a recurrence):** first version of `StateCalc` was an
`if/else-if` chain with **no final `else`**, so it could fall off the end of a
non-`void` function returning nothing → `-Wreturn-type` warning; using that result
is **UB**. Reconnected to the known rule: falling off the end is defined *only* for
`main` (returns 0), UB for every other function. Fixed with a final
`else { return current_state; }`. Also upgraded the signature from `int` params to
**`enum`-typed** params/return — readable, and free because enums are ints anyway.

**Banked 1 learning:** a C `enum` is named int constants with no runtime guard,
unlike Python's `Enum` (`types`, `python-collision`).

**Where I left off mid-thought:** nothing open; `turnstile.c` clean under
`-Wall -Wextra` and correct. Next is `const`.

**2026-11-18 (session 15) — a recall miss on `#include` vs `import` became the
session (reteach + consolidation); no new binary.** Overnight gap, so the quiz
ran. **Q1** (*three ways `#include` differs from `import`*) → got **1 of 3** (the
shape-vs-code one: `#include` brings declarations, `import` brings runnable code);
**missed the other two cold** (caching, namespace), which are the ones historically
sticky. Became the session; `enum` pushed again. Stopped the quiz after Q1.

**Covered (reteach of the two missing differences, + a deep compile-vs-link dive):**
- **No caching:** `#include` is a dumb **text paste** with no memory — the same
  header's text is re-pasted every time it appears (directly or indirectly). That's
  *why include guards exist*: a guard is a **manual cache** (second paste → no-op),
  recreating what `import` gives for free.
- **No namespace:** `#include` dumps every name **flat into global scope** (no
  `stdio.printf` prefix) — which is why a home-grown global `FILE` collides with
  `stdio.h`'s `FILE` (the session-4 experiment).
- **The 2-vs-3 sorting test** (both throw "redefinition"-style errors): *"one thing
  pasted twice"* → caching → **guards fix it**; *"two different things fighting over
  one name"* → namespace → **renaming fixes it**. Ishan initially mislabeled the
  double-struct case as namespace; his own "guard = caching" answer untangled it.
- **Deep dive (his question):** including the same struct-defining header twice in
  one `.c` is a **compile-time** error (`redefinition of 'struct Point'`, file:line),
  **not** a linker error — because both pastes are in the *same* translation unit,
  and a `struct` **type** is a compile-time-only concept that produces **no linker
  symbol** (shape, not code). The linker's duplicate-symbol error is a *different*
  scenario: a function/global defined in *two separate* `.c` files. Also reaffirmed
  declaration (repeatable, `struct P;`) vs definition (once per TU, `struct P {…};`).

**Built — nothing.** Skipped the confirming guard/no-cache experiment (he's built
it twice before — sessions 4 and `p0.4`). Logged as a reteach/consolidation session;
the "binary every session" rule is waived here *only* because it was old ground.

**Also — ran `/tune` mid-session:** his feedback *"after a really big paragraph of
explanation or correcting me, you ask me if its done rather than going to the next
one."* Sharpened `CLAUDE.md` item 2: drop the passive "make sense?/clear?/is it
done?" pulse-checks; the only understanding-check is a real **prediction** at the
build handoff on a core concept. (Also folded in the parked 11-16 note; that open
thread is now removed.)

**Where I left off mid-thought:** nothing open. Next is `enum` (+ state-machine
drill) — new material, ends in a binary.

**2026-11-17 (session 14, same-day continuation after session 13's wrap) —
storage duration: `static` vs automatic locals.** Same-day, no quiz.

**Covered (a real C nuance + strong Python/JS collision):**
- **Two independent axes** they'd been conflating: **scope** (where a name is
  visible) vs **storage duration / lifetime** (how long its bytes exist). For a
  normal (automatic) local they coincide, which is why it's easy to merge them.
- **Automatic** locals live on the **stack frame** — born on entry, die on
  return (what they already knew). A **`static`** local lives in the **data
  segment** for the **whole program**: created once *before `main`*, its
  initializer (`= 0`) runs **once, ever** (not per call), and frame teardown never
  touches it — so the value persists across calls — while its **scope stays
  local** (invisible outside the function). `static` changes only the lifetime
  axis. Footnote parked: `static` at *file* scope means a different thing
  (internal linkage — invisible to other `.c` files); met later.
- **Python/JS collision (explored in depth):** a plain local in Python/JS doesn't
  persist either; persistence there is faked with **closures**, **function
  attributes**, or the **mutable-default-arg gotcha** (`def f(_box=[0])` — default
  evaluated once at def-time, stored on the function object's `__defaults__`,
  mutated in place). Traced *why* it survives: the module namespace (a name→object
  table) holds `f` → `f` holds the list → the list stays alive. Key divergence:
  Python/JS persist by **keeping a reference alive on the heap** (GC-managed); C's
  `static` is a **permanent reserved data-segment address**, no object, no
  reference. Also: `static` has **three** unrelated meanings across C (block-scope
  lifetime; file-scope linkage) and C++/JS/TS (class member).

**Built — `p1a/tick.c`:** `counter(void)` with a `static int cnt = 0; cnt++`
printed **1,2,3,4** across four calls; removing just the `static` collapsed it to
**1,1,1,1**. Clean under `-Wall -Wextra`. Notably used `(void)` on both functions
unprompted — yesterday's `()`-vs-`(void)` rule sticking.

**Banked 1 learning:** a `static` local lives in the data segment and survives
frame teardown (`memory`, `python-collision`).

**Where I left off mid-thought:** nothing open; `tick.c` demonstrates both
behaviours. Next is `enum` (+ a state-machine drill), then `const`.

**2026-11-17 (session 13) — a recall miss on `printf` became the session: the
format string is trusted, not checked.** Overnight gap, so the quiz ran. **Q1**
(*how does `printf` work out an argument's type, and what if you're wrong?*) was
**missed cold** — answered *"figured out at compile time."* Per protocol that
became the whole session and the planned new topic (storage duration) was pushed.
Stopped the quiz after Q1 (no badgering); Q2/Q3 not asked.

**Covered (the reteach):**
- **`printf` is variadic** (`printf(const char *fmt, ...)`) and does **no** type
  detection. At runtime a C value is just **untyped bytes** — the type lived only
  in the compiler and the compiler is gone. `printf` reads the **format string**
  and, per each conversion specifier, grabs N bytes and interprets them however
  the specifier says. It **trusts the format string completely** and cannot be
  told it's wrong. **Python collision:** `print(x)` gets a tagged object and asks
  it its type; C hands over naked bytes with nothing to ask.
- **Mismatch = UB, and the *shape* of the lie decides the damage:** `%d` on a
  `double` → reads wrong bytes → **garbage number**, no crash (nothing
  dereferenced); `%s` on an `int` → takes the int's bits as an **address**,
  follows it into unmapped memory → **SIGSEGV** (same MMU/page-fault chain from
  session 8).
- **The only checking is a compile-time *courtesy*:** clang `-Wformat` (in
  `-Wall`) reads the **literal** format string, warns on a mismatch, and even
  prints the correct specifier (`^ %d` / `%f`). It's the *compiler*, not `printf`
  — and it can't help if the format string is a variable. The warnings are
  **warnings, not errors**, so the binary still built and the UB was reachable;
  `-Werror` (the deferred Makefile carry-over) would make them fatal.
- New specifier met: **`%f` for a `double`** (alongside `%d` int, `%zu` size_t).

**Built — `p1a/fmt.c`:** baseline prints an `int`/`double`/`size_t` with the
right specifiers (clean under `-Wall -Wextra`), then two deliberate lies (`%d` on
the `double`, `%s` on the `int`). Saw `-Wformat` warn both at compile, then ran it
and observed exactly the predicted split: garbage negative number, then a
segfault. Predicted both the compiler reaction and the runtime behaviour correctly.

**Banked 1 learning:** `printf` does no type checking — it trusts the format
string completely (`types`, `python-collision`, `ub`).

**Where I left off mid-thought:** nothing open; `p1a/fmt.c` builds (with the two
intended `-Wformat` warnings) and demonstrates both failure modes. Next is storage
duration (`static` vs automatic) — deferred from today.

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
  diagnostic names the **stage** that produced it (not whether it stopped the
  build — that's `warning:` vs `error:`), and a warning and an error in one output
  can come from different stages.* Offered **three** times now (retaught again in
  session 17's Q3), still not written up. It's in the recall queue anyway — worth
  pushing them to bank it next time it surfaces.
- **Cast-away-`const` demo — parked for 1B.** Defeating `const` (take the address,
  cast off the `const`, write through the pointer → UB if the object was truly
  `const`) needs pointer syntax they haven't drilled. Cut from session 17; it's
  the natural *first* payoff when pointers land in 1B (sources: King §11.1–11.2
  for `&`/`*`, §7.4 for casts). They already understand *why* it's UB — they just
  need the mechanics.
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
| `clang -E hello.c` turns 4 lines into 599. What is in those lines — and what is definitely *not*? | 2026-10-18 | 2026-11-16 | 1 |
| Your binary is arm64 and your EC2 box is arm64. Why won't it run there? | 2026-10-18 | 2026-11-16 | 1 |
| Give three ways `#include` differs from `import`. | 2026-10-18 | 2026-11-18 | 0 — **got 1/3, retaught** (had shape-vs-code; missed caching + namespace again — the two sticky ones) |
| Why does dereferencing `NULL` usually crash, and what is doing the checking? | 2026-10-18 | 2026-11-15 | 0 — **missed cold, retaught** (said "compiler"; it's the MMU → page fault → kernel → SIGSEGV) |
| `printf` is handed a value. How does it work out what type it is — and what happens when you're wrong? | 2026-10-18 | 2026-11-17 | 0 — **missed cold, retaught** (said "compile time"; really: variadic, trusts the format string, no runtime type detection, mismatch is UB) |
| A program returns `3`, but `echo $?` says `130`. What happened, and what does `130` decompose into? | 2026-10-18 | 2026-11-21 | 1 — recovered (`128+2` = SIGINT) |
| A build prints a warning with a `file:line` and an error prefixed `ld:`. Which one stopped the build, and what does each prefix tell you? | 2026-10-18 | 2026-11-21 | 0 — **partial**: got the stage mapping (file:line→compiler, ld:→linker) but shaky on *stage vs severity* (prefix = stage; `warning:`/`error:` = whether it stopped the build); retaught |
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
| You pass a `double` to `printf("%d\n", x)` and build with `-Wall`. Does it compile? Does it run? Who catches the mismatch and who stays blind to it — and why `%d`-on-`double` prints garbage but `%s`-on-`int` segfaults? | 2026-11-17 | — | 0 |
| A function's `static int cnt = 0; cnt++;` prints 1,2,3 across three calls, not 1,1,1. Where does `cnt` physically live, on which call does the `= 0` actually run, and how do its *scope* and its *lifetime* differ? | 2026-11-17 | — | 0 |
| You `#include` the same struct-defining header twice in one `.c`. Is the duplicate-definition error from the compiler or the linker — and why can the *other* one not catch it? | 2026-11-18 | — | 0 |
| Two failure modes both say "redefinition": same thing pasted twice vs two different things with one name. Which is which, which does an include guard fix, and what fixes the other? | 2026-11-18 | — | 0 |
| `enum State { LOCKED, UNLOCKED };` — what value and type do the two names get? Is an `enum`-typed variable a safe distinct type, or can you store `99` into it and assign `LOCKED` straight to an `int`? How does that differ from Python's `Enum`? | 2026-11-18 | — | 0 |
| `const int x = 5; x = 6;` — does it fail, and at *which stage* (compiler or runtime)? Is `const` a hardware lock on those bytes or just a promise to the compiler? If you cast the `const` away and write through a pointer, what happens — and why is that UB, not a caught error? | 2026-11-21 | — | 0 |
| In C, `for (int i = 0; …){}` then reading `i` *after* the loop — compile or error, and why? How does that differ from Python, and what *one* word names the thing C has that Python lacks here? When you shadow an outer name in an inner block, is the outer variable destroyed or just unreachable — and is `-Wshadow` on under `-Wall -Wextra`? | 2026-11-22 | — | 0 |
| A function calls another *before* declaring one of its own locals. At the moment the inner call runs, does that not-yet-declared local **(a)** not exist, or **(b)** exist holding garbage? Defend it in terms of *storage* (when the bytes exist) vs *initialization* (when a value lands) vs *scope* (when you can name it). What do you get if you read it before the init line? | 2026-11-22 | — | 0 — **needs cold re-confirm** (picked (a) in session 18, retaught to (b); the storage-vs-init split is load-bearing for all of 1B/2) |
| `int x = 5; int *p = &x;` — what does `p` actually store? What are `&` and `*`, and what's their relationship? After `*p = 99`, what does plain `x` print, and *why*? And `sizeof(int *)` vs `sizeof(char *)` — same or different, and what's the trap in thinking they'd differ? | 2026-11-22 | — | 0 |
| A pointer's *type* is a "decoder ring." Explain: an address names only the *starting* byte, so what two things does `*p` need, and which does the type supply? Then — why can you dereference an `int *` but the compiler *rejects* `*vp` on a `void *`? What is a `void *` for, and why does `%p` want one? | 2026-11-22 | — | 0 |
| C is pass-by-value, so `inc(int n){ n=n+1; }` can't change the caller's `x`. How do you write a function that *does* change a caller's variable, and what exactly does it receive? In `swap(int *a, int *b)`, do you move values through `a`/`b` or `*a`/`*b` — and what would swapping `a`/`b` themselves accomplish? Given all that, why is "C passes pointers by value" not a contradiction with "pointers let you modify the caller"? | 2026-11-22 | — | 0 |
| What do `-g`, `-O0`, and `-fsanitize=address,undefined` each do in a dev build, and why do you want `-O0` *specifically* when debugging? What is a `.dSYM` bundle, why does macOS create one, and is it source or build output? | 2026-11-22 | — | 0 |
| You deref two bad pointers by mistake — one is `NULL`, one was never initialized. Which fails loud and *deterministic*, and which can *silently* corrupt memory and still pass every test? Why does that make `NULL` the *safer* bad value — and what two habits follow? How does deref-ing `NULL` differ from touching `None` in Python? | 2026-11-22 | — | 0 |
| Dereferencing `NULL` is a *runtime* crash — yet the report says `null.c:7`. How can a runtime crash know a source line? What did the compiler/sanitizer do at *compile* time, and what happens to that line number if you strip `-g` *and* the sanitizers? | 2026-11-22 | — | 0 |

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

36 entries.

| Tag | Count |
| --- | --- |
| `python-collision` | 19 |
| `types` | 12 |
| `toolchain` | 11 |
| `ub` | 8 |
| `memory` | 5 |
| `standard` | 5 |
| `integer` | 4 |
| `process` | 4 |
| `alignment-aliasing` | 2 |
