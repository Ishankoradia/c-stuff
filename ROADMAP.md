# C Roadmap — learning C by rewriting the bookmarks backend

**Outcome being optimised for: my fluency in C and in manual memory management.**
A working C backend is the *vehicle*, not the goal. If a choice trades speed-of-
delivery for depth-of-understanding, take depth.

Companion files: `LEARNINGS.md` (the lab notebook — the real deliverable of all this),
`TUNING.md` (how this roadmap has been corrected by contact with reality)
and `CLAUDE.md` (the teaching contract Claude is held to).

## Status

| | |
| --- | --- |
| **Current phase** | Phase 0 — First contact with C and the toolchain |
| **Started** | 2026-10-17 |
| **Target: first C-served production endpoint** | ~May 2027 (Phase 6 exit) |
| **Budget** | 15-20 hrs/week |

Realistic end-to-end: **~7 months** to the first production endpoint served by
C, **~10-13 months** to a full replacement. Shorter is a sign I skipped the drills.

## Starting point (calibration — read before teaching me anything)

**I am an experienced programmer and a total beginner in C.** Those are two
separate facts and both matter:

- **I know how to program.** Python, TypeScript, SQL, Docker, git, the terminal.
  I ship a full-stack product. Don't explain what a function, a loop, a return
  value, a compiler, a library or a test is in the abstract.
- **I know no C, and no systems-level anything.** I have never written a line of
  C. I don't know pointers, stack vs heap, what `malloc` is, what a header file
  is for, what the compiler does with my code, what an object file is, what
  `argv` is, or how a program's memory is laid out. Every one of these is new.
- **My instincts are from garbage-collected, memory-safe languages** and most of
  them are actively wrong here. Expect me to assume values get copied when they
  don't, that strings know their own length, that things clean themselves up,
  and that out-of-bounds access raises an error.
  **Lead with those collisions — don't just name them when they come up.** When
  introducing anything that also exists in Python with different semantics, open
  with "in Python this would X; here it does Y" *before* stating the C rule. The
  comparison is what makes it land; it is the explanation, not a garnish on it.
  Reach for the analogy by default, including for things I haven't flagged as
  confusing. It is *not* a licence to write C for me.

**Teaching consequences:**

- One concept per session, taught to the bottom. Not three.
- **Every session ends with code I compiled and ran myself.** A session of pure
  theory is a failed session in Phase 0-2, however good the theory was.
- Introduce tools and compiler flags **one at a time, each explained**. Never
  hand me a nine-flag command line and move on.
- Socratic still applies, but the ladder starts lower: ask me to predict
  outcomes of things I've already seen, not to derive things I've never met.
- When I can't answer a prediction question, that is normal and expected at this
  stage, not a gap to be alarmed about. Teach it and move on.

## Standing decisions

- **Permitted external deps: libpq, OpenSSL, pthreads, libc. Nothing else.**
  HTTP server and parser, JSON, hash maps, strings, allocators, router and
  scheduler are all hand-written. If this starts feeling unbearable, the answer
  is to cut features from the rewrite, not to relax this.
- **Exercise-drilled.** Concepts get drilled in isolation before they get used
  in anger. When I fall behind, cut scope from the *rewrite*, never the *drills*.
  Shipping less C backend is fine. Learning less C is not.
- **Production stays on Python** until a C endpoint has run 24h under synthetic
  load with flat RSS. Strangler pattern behind the existing nginx, one route at
  a time. There is no cutover day.
- **Migrations stay in Alembic.** Not being rewritten, not negotiable.
- **macOS for dev, Linux/EC2 for prod.** kqueue locally, epoll in prod — so the
  event loop gets an abstraction layer, and I build and test on Linux (container)
  from Phase 4 onward rather than discovering the gap in Phase 7.
- **Phase 7 step 6 (scraper, RSS, OpenAI client) is an off-ramp, not a finish
  line.** Reaching it with the judgement to leave those in Python counts as
  completing this project.

## How we work

**Seven commands.** I never have to remember where we were — `STATE.md` holds it.

| | |
| --- | --- |
| `/recap` | What did I do last time? Read-only, past tense only, ~12 lines. No commitment to a session. |
| `/teach` | Start or resume. Recaps last session, quizzes me cold on old material, then teaches the next concept. |
| `/learnings` | Something surprised me, or clicked. Banks it as a *generalized rule* in `LEARNINGS.md`. 60 seconds. |
| `/review` | I've written something. Bugs reported by category and location, never the fix. |
| `/stuck` | 30+ min of banging on it. The hint ladder, one rung at a time. |
| `/wrap` | End of session. Saves state, updates the recall queue and bug tally, logs the line. |
| `/tune` | Feedback on *how this is being taught*, not on C. Folds it into `CLAUDE.md` / `ROADMAP.md` / the skills, so it survives past today. |

These documents are a first guess, written before I'd been taught any C —
`/tune` is how they get corrected, and `TUNING.md` is the record of it. The
exception is the spine: no-code rule, sanitizer gates, no phase skipped, the
30-minute rule, hand-rolled deps. Those can't be tuned in the moment; `/tune`
parks the request until I reaffirm it on a different day, because the evening I
most want them gone is the evening they're doing their job.

`STATE.md` is the single source of truth for position, last-session recap, open
threads and the recall queue. `/wrap` is the only thing that writes it — so if I
skip `/wrap`, the next session starts blind. It's the one ritual not to drop.

**The 30-minute rule.** No asking for help until genuinely stuck for 30 minutes
*and* I've written down what I think is happening. Articulating the wrong theory
is where most of the learning is; being handed the right one skips it.

**Rhythm beats volume.** Five 3-hour evenings beats one 15-hour Saturday.
Pointer bugs are found by a rested brain. Roughly: 2 hrs reading + Socratic
intake, 10 hrs building alone, 2 hrs review, 2 hrs consolidation (LEARNINGS.md
entries, ownership comments in headers, tests topped up).

**Claude never writes my code.** Not snippets, not "just the tricky bit", not a
skeleton to fill in — not even when I ask in a moment of weakness. Full contract
and the escalation ladder in `CLAUDE.md`.

## Gates

A phase closes only when its exit test passes **clean under
`-fsanitize=address,undefined`, zero leaks.** No partial credit, no "I'll come
back to it". Claude refuses to open the next phase otherwise.

(Exception, and only this one: Phase 0 and Phase 1A allocate nothing, so there
is no heap for ASan to have an opinion about. Their gate is the exit test alone,
built with the sanitizer flags on. From Phase 1B onward the rule is absolute.)

At each phase close, 5 minutes in the session log: what I can do now that I
couldn't before; my `LEARNINGS.md` tag tally for the phase; one thing I still
don't really understand, carried forward.

**Monthly** (first of the month, 30 min): reread `LEARNINGS.md`, tally tags,
check the mix is shifting from mechanical (leak, off-by-one) toward structural
(ownership-ambiguity, lifetime-design) — that shift is the actual measure of
progress. Then re-derive one thing cold from memory: the process address space,
or exactly what `realloc` guarantees. If I can't, that's next month's intake.

---

# Phase 0 — First contact with C and the toolchain (weeks 1-2)

The point of this phase is that C stops being alien. No pointers yet, no
`malloc` yet, no multi-file anything until step 4. Five steps, in order; each
one ends with a binary I built and ran.

### 0.1 — One file, one command

Write `hello.c`. Compile it with `clang hello.c -o hello` and run `./hello`.
**Learn:** what `main` is and why it returns an `int` (check it with `echo $?`);
what `#include <stdio.h>` is doing; `printf` and its format specifiers; why
there is a build step at all, when `python hello.py` needs none; what the file
`hello` actually *is* (`file hello`).

### 0.2 — The language's small vocabulary, one file at a time

Still one file, still compiled by hand every time. Variables and the basic
types (`int`, `char`, `double`, `size_t`), `if`/`while`/`for`, writing and
calling my own functions, declaration-before-use, arrays as a block of N things,
`sizeof`. **Deliberately collide with my Python instincts:** integer division,
`char` is a number, an array that does not know its own length, no strings as a
first-class type, no booleans until `<stdbool.h>`.

**Drill:** three or four tiny programs — sum 1..n, print a triangle, find the
max in an array, count vowels in a fixed `char` array.

### 0.3 — Reading errors, and the first flags

Break 0.2's working program on purpose, one way at a time, and read what clang
says: missing semicolon, undeclared variable, typo'd function name, wrong
`printf` specifier for the type, calling a function with the wrong argument
count. **Then add warning flags one at a time**, understanding each before
adding the next: `-Wall`, then `-Wextra`, then `-std=c11`, then `-g`.
`-Werror`, `-Wshadow` and `-Wconversion` come at the *end* of this step, once
the output is quiet. Goal: a clang diagnostic is something I read, not something
I panic at.

### 0.4 — Two files, and why headers exist

Split a working single-file program into `main.c` + `util.c` + `util.h`.
**Learn:** translation units (the compiler sees one `.c` file at a time and is
blind to the others); what the preprocessor does (`clang -E` and look at it);
declaration vs definition; compiling to object files with `-c`; linking them;
include guards; the difference between a *compiler* error (names a file and
line) and a *linker* error (names a symbol). Break it four ways and predict
which kind of error each produces first: misspelled header, object file left
out of the link, circular include, function declared but never defined.

### 0.5 — The Makefile

Only now, with files that exist and a build I've been running by hand and am
tired of retyping. Hand-written, not CMake. **Learn first:** what a make rule
is (target, prerequisites, recipe), how make decides a target is stale, and the
tab-not-spaces rule — GNU make manual §2. Targets: `build`, `test`, `asan`,
`clean`. The full flag set, now that I know what each flag is for:

```
-std=c11 -Wall -Wextra -Werror -Wshadow -Wconversion -g -O0
-fsanitize=address,undefined -fno-omit-frame-pointer
```

**Tools introduced in this phase:** `clang`, `file`, `make`. Deferred to the
phase where they're needed rather than front-loaded: `lldb` (Phase 1), ASan
report format (Phase 2, where there are finally heap bugs to find), `nm` and
`otool -L` (Phase 3, with a real static library), `leaks` (Phase 2).
(Valgrind is unreliable on Apple Silicon — ASan is the daily driver; keep
Valgrind for the Linux/EC2 side later.)

**Exit test:** from an empty directory, I can write a two-file C program with a
header, build it with a Makefile I typed myself, and explain out loud what
happened between `make` and the binary existing — naming the preprocessor,
compiler, assembler and linker and what each one consumed and produced.
(ASan-clean is not yet a meaningful gate: nothing here allocates.)

---

# Phase 1 — Core language, drilled (weeks 3-7)

Split in two, because pointers are the wall and everything before them is
cheap. **1A has no pointers in it at all.**

## 1A — Everything that isn't a pointer (weeks 3-4)

**Learn:** the integer types and why there are so many; `size_t`; integer
promotion and implicit conversion; signed overflow is UB while unsigned wraps;
`sizeof`, alignment and struct padding; `enum`; `struct` as a value (and the
shock that assigning one copies it); `const`; storage duration — automatic vs
static vs allocated — and what `static` means at file scope vs block scope;
scope and shadowing; the stack frame as a mental model (what happens to a local
when the function returns).

**Drills:** a struct whose padding I predict *before* printing `sizeof`; a
program that demonstrates unsigned wraparound and one that trips `-Wconversion`;
an `enum`-based state machine; a function returning a struct by value.

**1A exit test:** I can draw the stack for a three-deep call chain and say which
locals exist at each moment and when each one dies.

## 1B — Pointers and memory you don't own yet (weeks 5-7)

**Learn:** what a pointer *is* (an address, which is just a number, with a type
attached saying how to read what's there); `&` and `*`; `NULL`; pointers to
locals and the dangling-pointer-to-dead-stack-frame trap; pass-by-value vs
passing a pointer and why C has no pass-by-reference; arrays vs pointers and
array decay; pointer arithmetic and why `p + 1` moves by `sizeof(*p)`;
NUL-terminated strings and the fact that `strlen` is O(n); `char *` vs
`char[]` and which one you may write to; `void *`; function pointers; `const`
on each side of the `*`.

**Drills (write every one from scratch, no libc):** `strlen`, `strcpy`,
`strncpy` (and explain why its semantics are a trap), `strcmp`, `memcpy`,
`memmove` (overlap!), in-place string reverse, word tokeniser without `strtok`,
`atoi`, a bitset, generic `swap`, generic bubble-sort over `void*` with a
comparator. (`strdup` and the 2D-matrix drill move to Phase 2 — they allocate.)

**Learn `lldb` here:** breakpoints, `bt`, `frame variable`, printing a pointer
and what it points at, watchpoints. Pointers become real when you can look at
the addresses.

**Exit test:** given any of my drills, I can state for each pointer: what it
points at, who owns that memory, and when it dies.

---

# Phase 2 — Memory, the heart of it (weeks 8-12) ★ the most important phase

**Learn:** the process address space (text/rodata/data/bss/heap/stack — draw it
from memory); `malloc`/`calloc`/`realloc`/`free` exact semantics; `realloc`
invalidating every pointer into the block; allocation failure handling; the
error-path leak (the single most common real-world C leak); ownership discipline
and naming conventions (`_create`/`_destroy`, borrowed vs owned parameters,
documented in every header); dangling pointers to dead stack frames; fragmentation;
alignment; strict aliasing; `restrict`.

**Build, in order:**
1. `vec` — a growable array. Learn amortised growth and pointer invalidation.
2. `arena` — a bump allocator over a big `malloc`/`mmap` block, free-all-at-once.
   This will change how I think about memory for the rest of the project.
3. `str` — a length-prefixed string + string builder on top of the arena.
4. A ref-counted buffer; then write down why refcounting is usually the wrong
   answer for a request-scoped server.
5. **My own `malloc`** over `mmap`: free list, splitting, coalescing, alignment.
   Then benchmark it against system malloc and be humbled.

**Exit test:** a 500-line program using all five, running a randomised stress
workload for 60s, ASan+UBSan clean, zero leaks, and an `ownership` comment on
every public function in every header.

---

# Phase 3 — Data structures I'll actually need (weeks 13-15)

**Build `libcore`,** a static library, each with unit tests:
hash map (open addressing, FNV-1a, tombstones, resize), dynamic string map,
singly/doubly linked list (and a written note on when it beats `vec`: rarely),
ring buffer, binary min-heap (this becomes the job scheduler's timer queue),
tagged union (`core_value`) that will later back JSON.

**Memory themes:** who owns keys vs values in a map; interning strings into an
arena; the cost of pointer-chasing; cache lines and why `vec` usually wins.

**Exit test:** `libcore` has >80% line coverage from my own hand-written test
runner (I write the runner too), and the whole suite is sanitizer-clean.

---

# Phase 4 — The OS boundary (weeks 16-18)

**Learn:** `errno` discipline, syscalls vs libc wrappers, partial `read`/`write`
loops, `EINTR`, file descriptors and their lifetimes (an fd is memory too),
`fork`/`exec`, signals and async-signal-safety, **`SIGPIPE`** (every C server
author gets killed by this once), sockets, blocking vs non-blocking, `EAGAIN`,
`select` → `poll` → `kqueue` (macOS dev) / `epoll` (Linux prod), pthreads,
mutex/condvar, atomics, a thread pool, and the basics of the memory model.

**Build:** TCP echo server, three times —
(a) blocking, one connection at a time;
(b) thread-per-connection with a bounded pool;
(c) single-threaded event loop on kqueue.
Then write down which one this product should use, and why.

**Exit test:** (c) survives 10k connections from a load generator without leaking
fds or memory.

---

# Phase 5 — Protocols, hand-rolled (weeks 19-23)

**`libhttp`:** HTTP/1.1 request parser — request line, headers, `Content-Length`,
chunked transfer-encoding, keep-alive, pipelining. Design decision to make
deliberately: **zero-copy slices into the read buffer vs. copying strings out.**
Argue it in writing, then implement the one I chose. Response writer. Router with
path params (`/bookmarks/:id`) over my hash map.

**`libjson`:** parser + serialiser, arena-allocated, built on `core_value`.
Must not leak on *any* malformed input — that's the whole exercise.

**Fuzzing starts here:** libFuzzer harnesses for both parsers, corpus committed
to the repo. Fuzzing is where C stops being theoretical. Expect a bad week.

**Exit test:** 1M fuzz iterations on each parser, zero crashes, zero leaks.
Correctness checked against a small suite of real captured requests.

---

# Phase 6 — Persistence and the outside world (weeks 24-27)

**Learn:** `libpq` — connection lifecycle, `PQexecParams` (never string
concatenation), **result ownership and `PQclear`**, binary vs text params,
`pgvector` values over the wire. Then a connection pool I write myself on the
Phase 4 primitives. OpenSSL as a TLS *client* for outbound calls: contexts,
cert verification (do not skip it), BIO lifetimes.

Also: config/env loading, a structured logger, and an error type that carries
context without leaking on the way up.

**Exit test:** `GET /bookmarks` (list, paginated, auth'd) served from C against
the real database, benchmarked against the FastAPI version, running 24h under a
synthetic load with flat RSS.

---

# Phase 7 — Strangler rewrite, endpoint by endpoint (months 7+)

Run C and FastAPI side by side behind the existing nginx; move one route at a
time. Alembic stays Python — migrations are not being rewritten.

Port order (easiest/highest-value first, each with load + leak verification):
1. Read-only bookmark/category endpoints.
2. Auth — JWT verification (HMAC-SHA256 via OpenSSL; constant-time compare).
3. Bookmark writes + the follow graph.
4. Feed read endpoints.
5. Background jobs — scheduler on the Phase 3 heap, worker pool from Phase 4.
6. **Last, and only if still worth it:** RSS/HN ingestion, HTML scraping, and
   the OpenAI embedding client. These are the weakest candidates for C and the
   most work. Reaching here and deciding to leave them in Python is a *success*,
   not a failure.

---

# Phase 8 — Production hardening (ongoing from Phase 6)

ASan/UBSan builds and the fuzz corpus in CI; `clang --analyze` and `cppcheck`;
`-D_FORTIFY_SOURCE=2 -fstack-protector-strong` in release builds; graceful
shutdown; timeouts everywhere; backpressure and bounded queues; rlimits;
a crash handler that dumps a backtrace.

---

# Reading list

- **C Programming: A Modern Approach, 2nd ed.** — K. N. King. **The Phase 0-1
  primary text.** Written for people who don't know C yet, and unlike K&R it
  doesn't assume you already think like a 1978 systems programmer. Chapters
  1-12 cover Phase 0 and Phase 1 almost exactly; its exercises are the drills.
- **Modern C** — Jens Gustedt (free PDF). Becomes the primary text from Phase 2
  on; it's the one that's actually current about C11/C17. Its "Level 0" is
  readable now, the rest is better once pointers have landed.
- **K&R, 2nd ed.** — for the exercises, not the idioms. Its style is dated and
  it is a *terrible* first book despite its reputation. Phase 1B at the earliest.
- **C Interfaces and Implementations** — Hanson. The best book on *ownership
  discipline* and designing C APIs. Read during Phase 3.
- **Expert C Programming: Deep C Secrets** — van der Linden. Arrays-vs-pointers
  and declarator syntax, finally clear.
- **Computer Systems: A Programmer's Perspective**, ch. 9-12 — memory, linking,
  I/O, concurrency. Read alongside Phases 2 and 4.
- **Beej's Guide to Network Programming** — Phase 4.
- `man` pages are primary sources: `malloc`, `realloc`, `read`, `recv`, `poll`,
  `kqueue`, `pthread_mutex_lock`, `signal`.
- RFC 9112 (HTTP/1.1 syntax) — Phase 5. Read it, don't guess it.

---

# How I'll know it's working

- `LEARNINGS.md` memory tags shift over time from *leak/double-free* to *design/ownership
  ambiguity*. That shift is the whole curriculum.
- I stop reaching for `malloc` reflexively and start asking "what's this
  allocation's **lifetime**, and whose arena does it belong to?"
- I can read someone else's C and immediately ask the right question: who frees this?

---

# Session log

One line per session. Date — phase — what I built — learnings logged.
Phase-close entries get the three-question retro above.
Newest at the bottom.

2026-10-18 — P0 — step 0.1 complete: hello.c built, run, and broken five ways (exit status, `%s` UB, Mach-O, `clang -E`, falling off the end of `main`) — 12 learnings (4 ub, 5 python-collision, 3 toolchain)
2026-10-18 — P0 — session 2: first cold recall quiz (2/3). Pipeline miss became the session — rebuilt hello through all four stages by hand (`-E`/`-S`/`-c`/link), read `hello.s`, `file hello.o` → object not executable, renamed `main` and read the compiler-warning-vs-linker-error output properly — 1 learning (toolchain/python-collision/process)
2026-11-08 — P0 — session 3: recall quiz 3/3 (four-stage pipeline now clean; Mach-O-vs-ELF recovered w/ nudge). Started 0.2 — built `sum 1..n` (`p0.2/sum.c`) with a `for` loop and own `sum()` function; proved name-shadowing (`called object type 'int' is not a function`) and silent int overflow (`sum(100000)` → `705082704`) by hand. Org: work moved into `p0.1/`/`p0.2/`; tuned `/teach` to pre-make the step folder — 3 learnings (types, standard, integer/ub/python-collision)
2026-11-08 — P0 — session 4: short recall reteach. Blanked on "#include vs import" cold (banked 3wk ago); rebuilt all three differences (cache / no-namespace / shape-not-code), the no-namespace one being the sticky part — needed the "one sheet, two FILEs" picture. Confirmed by hand: global `FILE` after `#include <stdio.h>` → compile-time redefinition error. Nested loops deferred. No new code, no new learnings (consolidation)
2026-11-08 — P0 — session 5: recall reteach (exit status / signals) — missed `130` decode cold (`127+3`→really `128+2`=SIGINT); rebuilt the `$?` model (returned code vs `128+N` killed-by-signal), predicted `139` for SIGSEGV. Signal experiment surfaced a real surprise: integer ÷0 on arm64 returns `0`, no SIGFPE (where x86 would `136`). Re-explained UB from scratch. Nested loops deferred again — 1 learning (ub/integer/python-collision)
2026-11-08 — P0 — session 6: nested loops taught (clock model; inner bound = current outer counter) and the triangle drill built (`p0.2/triangle.c`, correct, unaided). Review: `n` declared-but-unused, loop hard-codes the bound. Two tunes landed: depth-allocation (spend time on C's hard/divergent bits, move fast through trivial) → `CLAUDE.md`; recall gap-gated (skip same-day quizzes) → `/teach`. No new learnings. Next: arrays
2026-11-08→11-14 — P0 — session 7 (spanned a 6-day gap): arrays + char + C strings; **0.2 complete**. Built `maxarr.c` (logic bug: compared neighbour not running max, fixed via failing test `{3,20,2,5,1}`) and `vowels.c` (correct count 5; hygiene issues left unfixed: scans `'\0'`, off-by-one bounds, stray print). Deep dives on string internals → 3 learnings (sizeof compile-time; C strings can't hold a 0 byte; len O(1) vs strlen O(n) — all types/python-collision). Next: 0.3 (reading errors + `-Wall`)
2026-11-15 — P0 — session 8: **0.3 core done** (`p0.3/break.c`) — broke code 4 ways, read compiler-vs-linker diagnostics, nailed declared-vs-defined; adopted `-Wall -Wextra` (caught a real `-Wsign-compare`). Quiz Q1 (NULL deref) missed → opened a big detour into **virtual memory / MMU / page faults**. 4 learnings (virtual-address/MMU; signed-vs-unsigned; two's-complement; declared-vs-defined). Strict flags deferred to 0.5. Next: 0.4 (multi-file + headers)
2026-11-15 — P0 — session 9: **0.4 done** (`p0.4/` main.c+util.c+util.h). Translation units (compiler blind to other `.c`s), two-step compile+link, One Definition Rule, self-contained + guarded headers, linker-matches-by-name-not-type. Built `max_of_arr` split across files (clean, `Max=15`); ran all four break-experiments, predicted each stage right. 4 learnings (TU-blindness; name-not-type linking; guard idempotency; self-contained headers). Next: 0.5 (the Makefile) — closes Phase 0
2026-11-15 — P0 — session 10: **0.5 done, PHASE 0 COMPLETE.** Hand-wrote first-ever Makefile (`p0.5/`): rules = target:prereqs + tab recipe; make rebuilds by timestamp (older-than-prereq = stale); phony targets; got tabs right first try. Fixed real bugs: `other.h` debris, missing `-c`, missing header prereq, `rm` → `rm -f`. Overwhelmed when I piled on strict flags+asan+variables at once — pulled back, deferred that polish. 1 learning (make=timestamps).

2026-11-16 — P1A — session 11, **Phase 1A opens**: a `struct` is a value (assignment/passing/returning all copy the bytes — the Python-reference collision), and `sizeof` is members+padding, not the sum (alignment drives padding; member order changes the size). Built `p1a/sizes.c` — predicted four `sizeof`s (2 hits: S3=12, S4=8; 2 misses: S1=3 all-char no-pad, S2=16 double wants 8-align), drew the layouts; 2 self-fixed code bugs (missing `struct` tag, bare `%`). Quiz 2 clean + 1 partial (printf/two-linker). 1 learning (`alignment-aliasing`/`types`/`memory`) — first structural tag of P1. Next: return-a-struct-by-value drill.

2026-11-16 — P1A — session 12 (same-day continuation): **stack frames taught head-on** (LIFO, pushed on call/popped on return, locals' bytes die with the frame — Python-collision: no GC rescue), **return-by-value** (copy out before the frame dies → safe; a returned *pointer* to a local would dangle), struct init syntax (dot + brace), and the **`()` vs `(void)`** trap (empty `()` = unchecked args; answered the `int main(void)` open thread). Built `p1a/retpair.c` (`ret(int,int)` returns a struct by value; clean, `x=5 y=10`). Decided to stay on `-std=c11` (not C23 — keeps the learnable sharp edges). 1 learning (`standard`/`python-collision`). Teaching tune to route via `/tune`: check understanding before handing off on core concepts. Next: storage duration (`static` vs automatic).

2026-11-17 — P1A — session 13: recall-quiz miss on `printf` (said "compile time") became the session. Retaught: **`printf` is variadic and does no type detection — it trusts the format string**; at runtime values are untyped bytes; mismatch = UB whose shape decides the damage (`%d`-on-`double` → garbage; `%s`-on-`int` → segfault via address deref). `-Wformat` is a compile-time courtesy on *literal* format strings (warns, even suggests the fix), separate from `printf` itself; warnings-not-errors so UB was reachable. Built `p1a/fmt.c` (baseline + two deliberate lies; predicted compile warnings + runtime split correctly); met `%f`. 1 learning (`types`/`python-collision`/`ub`). Storage duration deferred to next. Teaching tune still parked for `/tune`.

2026-11-17 — P1A — session 14 (same-day continuation): **storage duration — `static` vs automatic locals.** Taught the two independent axes (scope vs lifetime); a `static` local lives in the **data segment** for the whole program (init-once, before `main`), survives frame teardown, but stays local in scope. Deep Python/JS collision (mutable-default-arg mechanism, closures, module namespace keeping refs alive; reference-keeps-alive vs permanent-address). Built `p1a/tick.c` (static → 1,2,3,4; remove `static` → 1,1,1,1; used `(void)` unprompted). 1 learning (`memory`/`python-collision`). Next: `enum` (+ state machine), then `const`.

2026-11-18 — P1A — session 15: recall miss on `#include` vs `import` (got 1/3) became the session. Retaught the two sticky differences — **no caching** (dumb text re-paste → why guards exist, guard = manual cache) and **no namespace** (names land flat in global scope → the `FILE` collision); the 2-vs-3 sorting test (one-thing-twice → guards vs two-things-one-name → rename). Deep dive: a duplicate struct via double-include is a **compile-time** error (same TU, a type produces no linker symbol), not a linker one. **No new binary** — skipped the confirming experiment as old ground (built in sessions 4 & `p0.4`); logged as reteach/consolidation. Ran `/tune` → `CLAUDE.md` item 2 (drop pulse-checks; check via prediction at handoff). 0 learnings. Next: `enum` (overdue, deferred twice) — ends in a binary.

2026-11-21 — P1A — session 17: recall quiz (3-day gap, 3 cold) — **Q1 four-stage pipeline pass → retired** (tightened compiler→`.s` vs assembler→`.o`); **Q2 `$?`=130 clean recovery** (`128+2`=SIGINT); **Q3 `file:line` vs `ld:` partial** — stage mapping right, *stage-vs-severity* shaky, retaught (prefix=stage; `warning:`/`error:`=whether it stopped). Taught **`const` on plain values**: write through a const name is a **compile-time** error; `const` is a **promise to the compiler, not a lock on the storage** (they nailed this unprompted); casting it away + writing = UB (parked for 1B). Built `p1a/constness.c` (error landed as predicted). Cut the cast-away demo mid-session (needs pointer syntax = 1B) — kept one-concept discipline. 0 learnings. Next: scope/shadowing, then the 1A exit test.

2026-11-22 — P1A — session 18 (same-day continuation, rolled past midnight): **scope/shadowing + the 1A exit test → 1A COMPLETE ✅.** Scope/shadowing (brisk, mechanical): C has **block scope** (name dies at `}`), Python has none (function-level LEGB) → `for (int i…){}` then `i` = compile error; shadowing hides the *name* not the *storage*; `-Wshadow` is opt-in, **not** in `-Wall`/`-Wextra`. They predicted every outcome cold and opted out of the hands-on shadow drill ("too easy"). **Exit test — built `p1a/chain.c`** (`main→a→b`, three deep). Passed: frames/teardown correct cold (three coexist, `a` alive while `b` runs, LIFO `b`→`a`→`main`). **One reteach (load-bearing):** at `b`'s moment, `a`'s not-yet-declared `x` — they picked "doesn't exist," retaught to "**slot exists (frame reserved on entry), holds garbage**" → separated storage vs scope/name-visibility vs initialization; re-probe (read-before-init = UB, garbage number) landed. 0 learnings (storage-vs-init flagged as a `/learnings` candidate for them). Next: **open 1B — pointers** (fresh session, not same-day).

2026-11-22 — P1B — session 19 (third of the day; they chose to open the wall "super fresh" over my rested-brain flag): **PHASE 1B OPENS — what a pointer *is*.** A pointer = a **typed address** (starting-byte number + compile-time type = "decoder ring" for how many bytes `*p` reads); `&`/`*` are inverses; `*p = 99` mutates the real variable (the handle); `sizeof` any pointer = **8** (address-sized, pointee-independent — they predicted "same" correctly, avoided the int*=4/char*=1 trap); **`void *`** = type stripped off → **can't be dereferenced** (decoder-ring rule run backwards), cast back to use, and `%p` takes `void *`. Built `p1b/ptr.c` (`*p=99`→`x=99`, both ptr sizes 8); updated bare-`int*`-under-`%p` to `(void *)` casts. 0 learnings (decoder-ring/`void*` flagged as a candidate). Next: pointers as function params (`swap`) + owed `NULL`.

2026-11-22 — P1B — session 21 (fifth of the day; deliberately light/consolidating): **`NULL`.** Spent depth on purpose + danger (crash mechanism was already cracked in session 20's mull): `NULL` = "no object" sentinel (C's `None`), address `0`, used for not-found / `malloc`-fail / end-of-list; **null-check before deref**. Key distinction nailed: `NULL` deref = *deterministic loud* SIGSEGV at `0x0`, vs an **uninitialized/wild pointer** = garbage address → possible *silent* corruption that passes tests → so **init to `NULL`**. Their unprompted question ("runtime crash, how do we get a line no?") → toolchain insight: compile-time (UBSan instrumentation + `-g`/DWARF/`.dSYM`) serializes file:line into the binary for runtime tools to read back. Built `p1b/null.c` (watched the named SIGSEGV under sanitizers). **2 learnings banked** → 36 total (`memory`/`ub`/`python-collision`; `toolchain`/`process`). Next: parked payoffs (cast-away-`const`, `&outer_x` shadow-proof), then pointer arithmetic / array decay.

2026-11-22 — P1B — session 20 (fourth of the day; I offered a lighter consolidation path, they chose the new concept, "quite fresh"): **pointers as function parameters.** C is **pass-by-value** (callee gets a copy → can't change caller's var); fix = **pass the address**, deref `*p` to reach back into the caller's frame; the **pointer itself is still passed by value** (copy of the address, same storage) → C has **no true pass-by-reference**. Python parallel: `f(n){n=n+1}` (unchanged) vs `g(lst){lst.append}` (changed/shared). Both predictions cold-correct (value-swap no-ops; must use `*a`/`*b` not `a`/`b`). Built `p1b/swap.c` — values exchanged, **first program run under `-fsanitize=address,undefined -g -O0`**, clean. **Tooling taught** (the carve-out): `-g` (DWARF→readable), `-O0` (accurate line-mapping), `-fsanitize=address,undefined` (ASan mem-bugs + UBSan UB); explained the `swap.dSYM` bundle (`dsymutil` under `-g`; build output, gitignore+clean). 0 learnings (pass-by-value model + tooling flagged as candidates). Next: owed `NULL` + parked cast-away-`const` & `&outer_x` shadow-proof, then pointer arithmetic / array decay.

2026-11-23 — P1B — session 22 (began same-day after the 2026-11-22 run, rolled past midnight; kept light/consolidating): **cast-away-`const` — both faces of the UB** (the parked session-17 payoff, finally cashed in). Core correction: the cast strips `const` off the *type*, so the write **always compiles** — the compiler checks only the type at the write site, never traces provenance (they'd predicted it would "detect the const and refuse"; restated it back correctly). The outcome is **runtime UB decided by placement**: (1) `const` *local* (writable stack) → write **silently stuck**, `*p`=99 but plain `x`=5 at the *same address* because the compiler **baked the literal `5`** into the read; (2) pointer to a **string literal** (read-only rodata) → **SIGBUS** (ASan `WRITE`, `strlit.c:5`) — distinguished from `NULL` SIGSEGV (no page vs page-no-write-permission). Built `p1b/constcheck.c` + `p1b/strlit.c`. Review caught 3 fixes (cast-on-the-left declaration tangle; missing cast; pushed 3× to print plain `x` after the write — the whole experiment). **2 learnings banked** → 38 total (`ub`/`memory`; `ub`/`types`/`toolchain`). Got overwhelmed mid-session → pulled back to 3-line, plain-language answers. Held the no-code line on the second demo. Next: `&outer_x` shadow-proof (last parked payoff), then pointer arithmetic / array→pointer decay.

2026-11-23 — P1B — session 23 (same-day continuation, short/consolidating): **the `&outer_x` shadow-proof — last parked payoff, both now closed.** Saved `p = &x` *before* a bare inner block that shadowed `x` with its own `x = 99`; inside: `x`→99 (nearest name), `*p`→5 (outer storage untouched), `&x`(inner) ≠ `p`(outer). Proved **shadowing hides the name, not the storage**, and a **pointer captures a storage address independent of names** (reaches past the shadow) — same storage-vs-name axis as `chain.c`. All four predicted cold-correct. Built `p1b/shadowptr.c`. One mechanical slip (over-cast `*p` to `void *` under `%d`) → untangled into "address+`%p`→cast; value+`%d`→no cast" (recurrence of the `ptr.c` `%p`-wants-`void *` point). 0 learnings (all predicted, nothing surprised). Next: **pointer arithmetic + array→pointer decay** (the big 1B topic, its own session).

2026-11-23 — P1B — session 24 (same-day, 7th in the stretch; offered the rested-brain fork, they chose to push into the big topic, went well): **pointer arithmetic + array→pointer decay — the heart of 1B.** `p + 1` advances by `sizeof(*p)` bytes (one *element*, not a byte — `int *`+1→+4, `char *`+1→+1; same decoder-ring as session 19); arithmetic is on the *address*, never the value, and `p+1` as an expression doesn't move `p`. **`a[i]` ≡ `*(a + i)`** (brackets are sugar; `2[a]` compiles). **Decay:** passing an array copies only the address (pass-by-value) + strips the type → `sizeof(arr)` inside = 8 (pointer), `int arr[]`≡`int *arr` (size ignored); length is lost at the boundary → must pass it separately, computed caller-side pre-decay (ties to `argc`/`argv` + session 7). Corrected two misconceptions (p+1 "loses" x; `char *`+1 "fails"). Built `p1b/arith.c`: `sizeof(a)`=16 in main, walked with `*(a+i)`, decay seen live (sizeof inside=8), and the crux — `print_ele1` printed only **2 of 4** (8/4) then `print_ele2(int *arr, size_t s)` fixed it. **1 learning** (array decay → pass length separately; `memory`/`types`/`python-collision`) → 39 total. Minor: hex-arith slip (`0x1000`+12 → wrote `0x1012`, should be `0x100C`). Next: a consolidating pointer-walking drill (string→`'\0'`, or a `(ptr,len)` reduce) before the 1B string drills.

2026-11-26 — P1B — session 25 (3-day gap, first non-same-day since the 11-22→23 stretch; quiz ran): **hand-rolled `strlen` + `%s` recurrence + const-correctness review.** Recall: NULL-deref-chain / "is a page fault always fatal?" → **partial, recovered after reteach** — had endpoints (addr 0 unmapped; SIGSEGV uncatchable) but collapsed the middle; rebuilt the **MMU detects (raises a page fault, not a signal) → kernel judges benign (demand-paging → map & resume, invisible) vs illegal (addr 0 → SIGSEGV)** split, so a page fault is *not* always fatal and the MMU can't send the signal (only the kernel tells benign from fatal). Built `p1b/strlen.c` (`size_t str_len(const char *)`, walk to `'\0'`, sanitizer-clean): the `'\0'` is the compiler-appended terminator (not counted), length lives only there → **O(n)** (vs Python `len` O(1)), and **no length param** needed (in-band sentinel vs `print_ele2`'s separate `len`). **`%s`-on-`*p` recurrence** (fmt.c): passed `char` 72 to `%s` → UB that *still appeared to print* → retaught `%s` wants an **address** (`p`), walks it itself; UB is allowed to look like it works, the `-Wformat` warning is the real signal. `/review`: two const-correctness fixes (param + `p` should be `const char *` — only-reads + read-only-literal/rodata trap), both applied → *the type should tell the truth about what you may do through the pointer*. 0 learnings (the `%s` UB is a flagged recurrence candidate; else understood). They predicted next concept for free (`const char *` locks pointee not pointer). Next: **`const` on each side of the `*`**, then the 1B string drills.

## Phase 1A sub-close (2026-11-22, 8 sessions, 2026-11-16 → 2026-11-22)

**Can now do that they couldn't:** predict `sizeof` for a struct from alignment/padding and reorder members to shrink it; reason that assigning/passing/returning a struct copies its bytes (vs Python's shared reference); explain `printf` as variadic + trusting the format string (mismatch = UB); distinguish `static` vs automatic storage duration (data segment vs stack frame, init-once); use `enum` as named int constants with no runtime guard; use `const` as a compiler-checked promise, not a storage lock; reason about block scope + shadowing + `-Wshadow`; and **draw the stack for a three-deep call chain**, separating a local's storage / name-visibility / initialization and knowing when each frame and local dies. Built `sizes.c`, `retpair.c`, `fmt.c`, `tick.c`, `turnstile.c`, `constness.c`, `chain.c`.
**Learnings banked in 1A:** ~5 (`alignment-aliasing`, `standard`, `types`, `memory`, `python-collision` — first structural-leaning tag `alignment-aliasing` appeared; mix still mechanical as expected pre-pointers).
**Bugs (non-sanitizer; 1A allocates nothing, no ASan gate by design):** fell-off-the-end of a non-void function (recurrence, `turnstile.c`); two self-fixed code-shape bugs in `sizes.c`. No heap, so no leak/overrun class yet.
**Exit test:** ✅ three-deep stack drawn + lifetimes narrated (`chain.c`, 2026-11-22) — passed with a reteach on storage-vs-initialization.
**Still shaky / carried forward:** the **storage-vs-initialization** split (storage exists from frame entry; init runs at the declaration line) — needed a reteach, now in the recall queue for cold re-confirmation. This is load-bearing for 1B (uninitialized locals, dangling pointers).
**Next:** Phase 1B — pointers (*the wall*). ASan/UBSan + zero-leak gate becomes absolute from here.

2026-11-18 — P1A — session 16 (same-day continuation): **`enum` + state-machine drill.** Enums = named int constants (auto-numbered from 0), an enum variable is just an int, no stored "set"; Python collision = **no runtime type safety** (`enum State s = 99;` compiles; opposite of Python `Enum`). Built `p1a/turnstile.c` (LOCKED/UNLOCKED × COIN/PUSH, `enum`-typed `StateCalc`, prints 1,0,1,0). 1 bug (recurrence): fell off the end of a non-void function (no final `else`) → `-Wreturn-type` → UB; fixed, and reconnected to "fall-off-end defined only for `main`." 1 learning (`types`/`python-collision`). Next: `const`.

## Phase 0 retro (closed 2026-11-15, 10 sessions, ~2026-10-18 → 2026-11-15)

**Can now do that they couldn't:** explain the four-stage build pipeline cold; read a compiler error vs a linker error by its *shape* (file:line vs `ld:`/symbol) and know *why* (declared-vs-defined); write and compile a multi-file C program by hand; write a hand-rolled Makefile with correct dependencies; reason about exit status/signals, virtual memory/MMU, two's complement, and UB. Wrote small working programs (sum, triangle, max-in-array, vowel count) and a two-file `max_of_arr`.
**Learnings banked this phase:** 29 total (tags still mechanical/`toolchain`/`python-collision`-heavy, as expected pre-pointers; `memory` just appearing).
**Bugs (non-sanitizer, nothing allocates yet):** neighbour-vs-running-max logic bug (maxarr); a pile of deliberate break-experiments. No ASan gate this phase by design.
**Exit test:** ✅ four-stage pipeline explained cold (recall quiz, 2026-11-08).
**Still shaky / carried forward:** array→pointer decay and `char *` literals (only sentences, not mechanism — Phase 1B); the Makefile's strict-flags/`asan`/`.PHONY` polish (deferred). Also: tendency to get overwhelmed when too much is introduced at once — keep the one-concept-per-session discipline tight in Phase 1.
**Next:** Phase 1A — everything that isn't a pointer; exit test is drawing the stack for a 3-deep call chain.
