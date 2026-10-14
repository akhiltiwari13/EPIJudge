# Debugging EPI problems with nvim-dap

Tutorial for stepping through a judge problem in Neovim, with **codelldb/LLDB** (for the clang
toolchains) and **gdb** (for the gcc toolchains). It describes the setup as it exists today — nothing
here requires installing a plugin or editing a config file.

`README.md` in this directory remains the authoritative build document. This file only covers running
a built binary under a debugger.

## 1. What is already in place

| Piece | State on this machine |
|---|---|
| nvim | LazyVim 16.0.1 with the `dap.core`, `lang.clangd` and `lang.python` extras |
| Plugins | `nvim-dap`, `nvim-dap-ui`, `nvim-dap-virtual-text`, `nvim-dap-python`, `mason-nvim-dap` |
| C++ adapter | mason `codelldb` 1.12.3, which **bundles LLDB 22.1.8** — the same version as the system clang/libc++ |
| gdb | system `gdb` 17.2, with its DAP interpreter available (`gdb -i dap`) |
| Alt gdb adapter | mason `cpptools` (`OpenDebugAD7`) |
| Python adapter | mason `debugpy`, wired by `nvim-dap-python` |

The system `lldb` package is *not* installed and is not needed: codelldb ships its own LLDB, at
`~/.local/share/nvim/mason/packages/codelldb/extension/lldb/bin/lldb`.

`lang.clangd` already registers the `codelldb` adapter and two `cpp` configurations — *Launch file*
and *Attach to process*. So a C++ session works out of the box; *Launch file* asks for the path to the
executable, because this repo has one binary per problem.

## 2. First session, start to finish

Debug `dutch_national_flag`. Any solved problem works; avoid `parity`, the only one linking Tracy.

```sh
cd ~/Work/learn/eopicode/epi_judge_cpp
./build.sh build clang-debug dutch_national_flag
cd ..          # IMPORTANT: start nvim from the judge root -- see section 3
nvim epi_judge_cpp/dutch_national_flag.cc
```

In nvim:

1. Put the cursor on a line inside your solution (e.g. the partition loop in `DutchFlagPartition`)
   and press `<leader>db`. A `B` appears in the sign column.
2. `<leader>dc` → pick **Launch file**.
3. At `Path to executable:` type the binary path. The prompt has **file completion**, so
   `epi_j<Tab>/build/clang-debug/dut<Tab>` is enough:

   ```text
   /home/quomptrade/Work/learn/eopicode/epi_judge_cpp/build/clang-debug/dutch_national_flag
   ```
4. The session starts, dap-ui opens, and execution stops at your breakpoint. Scopes shows
   `pivot_index = 2` and `A_ptr = size=4` with `Color` values by name — libc++ formatters working.

Then: `<leader>dO` steps over, `<leader>dc` resumes to the next hit, `<leader>dt` terminates.

## 3. The two traps, and what they mean for a debug session

Both are documented in `CLAUDE.md`; a DAP session hits them exactly like a shell run.

### Working directory

`GetDefaultTestDataDirPath()` (`test_framework/test_utils.h:90`) probes `test_data`, `../test_data`,
… four levels up, and does so **before** `--test-data-dir` is parsed. LazyVim's config uses
`cwd = "${workspaceFolder}"`, which nvim-dap expands to `vim.fn.getcwd()` (`dap.lua:384`) — so the
debuggee inherits **nvim's** working directory.

Start nvim from the judge root, or `:cd ~/Work/learn/eopicode` before launching. From the judge root
or from `epi_judge_cpp/` the probe succeeds; from a deeper directory the binary aborts with
`Can't find test data directory`.

### Every run rewrites `problem_mapping.js`

`update_js` defaults to true (`test_framework/test_config.h:55`). LazyVim's *Launch file* config
passes **no arguments**, so a debug session dirties your git tree exactly like a bare `./binary` run.
Three ways to deal with it:

- Use `<leader>da` (*Run with Args*) instead of `<leader>dc` and enter:
  `--no-update-js --no-tty --no-color`
- Pin the args for the whole nvim session (see section 4).
- Or just `git checkout problem_mapping.js` afterwards.

`git status --short` from the judge root is the check: `problem_mapping.js` must not show up as
modified.

## 4. Making the loop fast (session-local, no files changed)

Both stock configs re-evaluate their `program` prompt on **every** start, including `<leader>dl`
(*Run Last*) — the path function is re-run each time. To get a prompt-free loop for the problem you
are working on, paste this once per nvim session (`:` then the whole line):

```vim
:lua local r=vim.fn.getcwd() local p=vim.fn.expand('%:t:r') require('dap').configurations.cpp={{type='codelldb',request='launch',name='EPI '..p,program=r..'/epi_judge_cpp/build/clang-debug/'..p,cwd=r,args={'--no-update-js','--no-tty','--no-color'},terminal='integrated'}}
```

Run it with the problem's `.cc` focused and nvim's cwd at the judge root. After that `<leader>dc`
starts with no prompts at all, and `<leader>dl` replays it. Swap `clang-debug` for `gcc-debug` or
`gcc-asan-debug` as needed; re-run the line after switching problems.

Rebuild between runs the usual way — `./build.sh build clang-debug <problem>` from `epi_judge_cpp/`,
which is a ~40 ms no-op when nothing changed.

## 5. Keymaps (from the `dap.core` extra)

| Key | Action | Why it matters here |
|---|---|---|
| `<leader>db` | toggle breakpoint | |
| `<leader>dB` | breakpoint with condition | `i == 3` to land on one loop iteration |
| `<leader>dc` | run / continue | starts a session, or resumes a stopped one |
| `<leader>da` | run with args | how you pass `--no-update-js` without editing config |
| `<leader>dC` | run to cursor | skips the rest of a loop without 40 step-overs |
| `<leader>dg` | move the PC to a line, no execution | re-try a block without restarting |
| `<leader>di` / `<leader>dO` / `<leader>do` | step into / over / out | |
| `<leader>dj` / `<leader>dk` | down / up the call stack | the harness stack is deep; see section 8 |
| `<leader>dl` | run last | |
| `<leader>dP` / `<leader>dt` | pause / terminate | |
| `<leader>ds` | show session info | |
| `<leader>dr` | toggle REPL | this is an LLDB console; see section 7 |
| `<leader>dw` | hover widget (scopes/frames/threads float) | |
| `<leader>du` | toggle dap-ui | |
| `<leader>de` | eval expression (normal **and visual** mode) | visually select a subexpression and evaluate it |

## 6. Driving dap-ui

Default layout: a 40-column sidebar with **scopes / breakpoints / stacks / watches**, and a 10-row
bottom split with **repl** and **console**. Inside any of those buffers:

| Key | Effect |
|---|---|
| `<CR>` | expand / collapse a variable, or jump to a frame or breakpoint |
| `e` | **edit** — in Scopes this writes a new value into the live variable; in Breakpoints it edits the condition |
| `d` | remove (a watch, a breakpoint) |
| `o` | open the related source location |
| `r` | jump to the REPL |
| `t` | toggle |
| `q` / `<Esc>` | close a floating element |

Watches: focus the Watches pane, `<CR>`, type an expression (`vec.size()`, `*A_ptr`). Inline values
come from `nvim-dap-virtual-text`; `:DapVirtualTextToggle` turns them off when lines get noisy.

## 7. The REPL is a real LLDB console

codelldb's console mode defaults to `commands`. The adapter itself says so:

```text
Console is in 'commands' mode, prefix expressions with '?'.
```

So in `<leader>dr`:

| You type | What happens |
|---|---|
| `bt`, `frame variable`, `br list` | executed as LLDB commands |
| `?vec.size()` | evaluated as a C++ expression |
| `/nat vec` | evaluate with the native LLDB evaluator (`/py` for Python, `/se` for the simple one) |
| `.c` `.n` `.into` `.out` `.up` `.down` `.scopes` `.frames` `.threads` `.capabilities` `.clear` `.help` | nvim-dap's own commands (`dap/repl.lua:137-157`) |

LLDB commands that earn their keep on these problems:

```text
v                                 all locals in this frame
p vec            / ?vec           print one expression
expr vec.at(0) = kBlue            mutate state mid-run and continue
br s -n DutchFlagPartition        break on a symbol instead of a line
br mod -c 'i == 3' 1              add a condition to breakpoint 1 after the fact
br mod -i 5 1                     ignore the first 5 hits -- i.e. stop on the 6th test case
wa s v -w write less              watchpoint: stop when `less` is written
fin                               finish the current frame (climb out of a harness template layer)
fr s 4                            select frame 4 directly
thread until 42                   run to line 42 in this frame
settings set target.max-children-count 200    stop long vectors truncating at 24
type summary list                 which formatters are active
log enable lldb dap               adapter-level logging when something misbehaves
```

In the Variables view codelldb also understands the pseudo-scopes `[locs]`, `[stat]`, `[glob]`,
`[regs]` and `[raw]`. `[raw]` disables formatters — handy to see a `std::vector`'s actual
`__begin_/__end_/__cap_` pointers.

## 8. Fitting the debugger to the judge harness

**The stack is deep.** Between `main` and your function sit about ten template frames:

```text
main
  GenericTestMain                       generic_test.h:59
    RunTests                            generic_test.h:141
      GenericTestHandler::RunTest       generic_test_handler.h:123
        ParseExpectedAndInvoke(Impl)    generic_test_handler.h:172,203
          GenericTestHandler::Invoke    generic_test_handler.h:216
            InvokeWithExecutor          timed_executor.h:66
              TimedExecutor::Run        timed_executor.h:28
                TimedExecutor::TimedCall  timed_executor.h:45
                  DutchFlagPartitionWrapper   <- your wrapper
                    DutchFlagPartition        <- your solution
```

Use `<leader>dk`/`<leader>dj` or `fr s N` to move, and `fin` to climb out one layer. Never step
*into* the harness: `<leader>dC` (run to cursor) and conditional breakpoints are faster.

**One thread.** `config.json` sets `timeoutSeconds: 0`, and with a zero timeout `TimedExecutor::Run`
calls your function directly (`test_framework/timed_executor.h:26-28`) rather than through
`std::async`. So there is a single thread and the stack above is the whole story. If you raise
`timeoutSeconds`, your code moves onto an async worker thread and the per-case timer will also fire
while you sit on a breakpoint — keep it at `0` while debugging.

**Breaking where the test fails.** A wrong answer throws `TestFailure` from the wrapper. codelldb
exposes two exception filters, and `cpp_throw` is marked default-on, which nvim-dap honours
(`dap.defaults.fallback.exception_breakpoints = 'default'`, `dap.lua:162`):

| Filter | Label | Default |
|---|---|---|
| `cpp_throw` | C++: on throw | **on** |
| `cpp_catch` | C++: on catch | off |

So out of the box a failing case stops you **at the `throw`**, with the failing input still live on
the stack — walk up with `<leader>dk` into `…Wrapper` and inspect the arguments. To change the
selection mid-session: `:lua require('dap').set_exception_breakpoints()` prompts with the adapter's
filters. To silence it: `:lua require('dap').defaults.codelldb.exception_breakpoints = {}`.

**Log points** print without stopping — the right tool inside a partition loop:

```vim
:lua require('dap').set_breakpoint(nil, nil, "i={i} less={less} more={more}")
```

**Several failures in one run.** `config.json` has `numFailedTestsBeforeStop: 1`; raise it to survey
a few failing cases in a single session.

## 9. Debugging with gdb instead

Three routes, none of which needs a config file change.

### 9a. gdb's own DAP interpreter (`gdb -i dap`)

gdb 17.2 here ships the DAP interpreter (`/usr/share/gdb/python/gdb/dap`). Register it for the
session and launch in one go:

```vim
:lua require('dap').adapters.gdb = { type = 'executable', command = 'gdb', args = { '--quiet', '--interpreter=dap' } }
:lua local r=vim.fn.getcwd() local p=vim.fn.expand('%:t:r') require('dap').run({type='gdb',request='launch',name='EPI gdb '..p,program=r..'/epi_judge_cpp/build/gcc-debug/'..p,cwd=r,args={'--no-update-js','--no-tty','--no-color'}})
```

Breakpoints, stepping, scopes and the dap-ui all behave the same. Differences from codelldb:

- The REPL takes **gdb** commands; there is no `?expr` / `/nat` console split.
- Exception filters are named differently, and **none is on by default** — select one explicitly with
  `:lua require('dap').set_exception_breakpoints()`:

  | Filter | Label |
  |---|---|
  | `throw` | C++ exceptions, when thrown |
  | `rethrow` | C++ exceptions, when re-thrown |
  | `catch` | C++ exceptions, when caught |

  (`assert` and `exception` are also offered; they are Ada-only.)
- No `terminal = "integrated"`; the debuggee's stdout arrives in the dap-ui console.
- You gain gdb's `record` / reverse-step, which LLDB on Linux does not have.

### 9b. cpptools (`OpenDebugAD7`)

Already installed at `~/.local/share/nvim/mason/bin/OpenDebugAD7`. It drives gdb over MI and accepts
VS Code-style `setupCommands`, so it is the choice if you want to script pretty-printer setup per
launch. Heavier than 9a for no gain on this repo.

### 9c. Plain `:terminal gdb`

No DAP at all, but `layout src`, `tui enable` and gdb's own command loop. Good when you want
`record full` or are chasing something the adapter abstracts away.

### Which debugger for which toolchain

This is not a matter of taste — it follows the standard library:

```text
clang-debug / clang-release   -> libc++    -> codelldb (LLDB has libc++ formatters)
gcc-debug / gcc-release       -> libstdc++ -> gdb (gdb has the libstdc++ printers)
gcc-asan-debug                -> either; ASan reports arrive on stderr
```

Measured on `dutch_national_flag`, same source, same breakpoint:

```text
gdb   on gcc-debug   (libstdc++):  A = std::vector of length 4, capacity 4 = {...}      good
lldb  on clang-debug (libc++):     A_ptr = size=4, Color enumerators by name            good
gdb   on clang-debug (libc++):     $1 = {__begin_ = 0x..., __end_ = 0x..., __cap_ = ...} raw
```

gdb against the libc++ build still single-steps fine; you just lose container summaries. Reach for
the matching pair instead.

## 10. Python problems

Same harness, same rules.

```sh
cd ~/Work/learn/eopicode          # judge root, for the test_data probe
nvim epi_judge_python/parity.py
```

`<leader>db` on a line inside your function, `<leader>dc` → pick dap-python's **file** config. To pass
the harness flags use `<leader>da` and enter `--no-update-js --no-tty --no-color`.

Two notes: debugpy's default `justMyCode` hides the `test_framework` frames, so add
`"justMyCode": false` if you need to step into the harness; and `<leader>dPt` / `<leader>dPc` (debug
test method / class) are pytest-oriented and do not apply to these standalone `main()` scripts.

## 11. Troubleshooting

| Symptom | Cause and fix |
|---|---|
| `Can't find test data directory` | nvim's cwd is too deep. `:cd` to the judge root and relaunch (section 3). |
| `problem_mapping.js` shows as modified | the session ran without `--no-update-js` (section 3). |
| `warning: Source file is more recent than executable` (gdb) | stale binary — the breakpoint will bind to the wrong line, and may land in a neighbouring function. Rebuild the target. |
| Breakpoint drawn as rejected / never hits | same staleness; or the file was opened through a symlink so its path differs from the DWARF path — then add a `sourceMap` to the launch config. |
| gdb pauses asking about debuginfod | pass `-iex 'set debuginfod enabled off'`, or put `set debuginfod enabled off` in `~/.gdbinit`. |
| `std::vector` shows as `__begin_/__end_/__cap_` | gdb against a libc++ (clang) build — see section 9. |
| Containers truncated at 24 entries | `settings set target.max-children-count 200` in the LLDB console. |
| Adapter seems dead | `:checkhealth dap`, `:DapShowLog`; for codelldb `log enable lldb dap` in the REPL. |
| Want to know what the adapter can do | `:lua =vim.inspect(require('dap').session().capabilities)` |

## 12. Appendix: the same thing without nvim

When a session misbehaves, this isolates "debugger problem" from "editor problem". Both commands run
from the **judge root**.

LLDB (codelldb's bundled copy, clang build):

```sh
~/.local/share/nvim/mason/packages/codelldb/extension/lldb/bin/lldb --batch \
  -o 'breakpoint set --file dutch_national_flag.cc --line 33' \
  -o run -o bt -o 'frame variable' -o continue \
  -- ./epi_judge_cpp/build/clang-debug/dutch_national_flag --no-update-js --no-tty --no-color
```

gdb (gcc build):

```sh
gdb --batch -iex 'set debuginfod enabled off' \
  -ex 'break dutch_national_flag.cc:33' \
  -ex 'run --no-update-js --no-tty --no-color' \
  -ex 'bt 5' -ex 'info locals' \
  ./epi_judge_cpp/build/gcc-debug/dutch_national_flag
```

Interactive instead of batch: drop `--batch` and the `-o`/`-ex` chain, then `settings set
target.run-args …` (LLDB) or `run …` (gdb).
