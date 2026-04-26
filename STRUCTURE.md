# Project Structure

```
.
├── cmake-build-debug
│   ├── build.ninja
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   │   ├── 4.2.2
│   │   │   ├── CMakeCCompiler.cmake
│   │   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   │   ├── CMakeSystem.cmake
│   │   │   └── CompilerIdC
│   │   │       ├── a.out
│   │   │       ├── CMakeCCompilerId.c
│   │   │       └── tmp
│   │   ├── clion-Debug-log.txt
│   │   ├── clion-environment.txt
│   │   ├── cmake.check_cache
│   │   ├── CMakeConfigureLog.yaml
│   │   ├── core.dir
│   │   │   └── src
│   │   │       └── core
│   │   │           └── linux
│   │   ├── cpu_test.dir
│   │   │   └── tests
│   │   │       └── src
│   │   │           └── metrics
│   │   ├── ctm.dir
│   │   │   └── src
│   │   ├── hash_map_test.dir
│   │   │   └── tests
│   │   │       └── src
│   │   │           └── core
│   │   ├── InstallScripts.json
│   │   ├── mem_test.dir
│   │   │   └── tests
│   │   │       └── src
│   │   │           └── metrics
│   │   ├── metrics.dir
│   │   │   └── src
│   │   │       └── metrics
│   │   ├── pkgRedirects
│   │   ├── process_manager_test.dir
│   │   │   └── tests
│   │   │       └── src
│   │   │           └── metrics
│   │   ├── process_test.dir
│   │   │   └── tests
│   │   │       └── src
│   │   │           └── metrics
│   │   ├── ring_buffer_test.dir
│   │   │   └── tests
│   │   │       └── src
│   │   │           └── core
│   │   ├── rules.ninja
│   │   ├── TargetDirectories.txt
│   │   ├── testing.dir
│   │   │   └── tests
│   │   │       └── src
│   │   └── ui.dir
│   │       └── src
│   │           └── ui
│   │               └── tui
│   ├── cmake_install.cmake
│   ├── compile_commands.json
│   ├── CTestTestfile.cmake
│   └── Testing
│       └── Temporary
│           └── LastTest.log
├── CMakeLists.txt
├── Doxyfile
├── include
│   ├── app
│   │   └── sound.h
│   ├── core
│   │   ├── hash_map.h
│   │   ├── list.h
│   │   ├── log.h
│   │   ├── platform.h
│   │   └── ring_buffer.h
│   ├── metrics
│   │   ├── cpu.h
│   │   ├── mem.h
│   │   ├── process.h
│   │   └── sampler.h
│   └── ui
│       ├── process_tab.h
│       └── ui.h
├── src
│   ├── core
│   │   ├── hash_map.c
│   │   ├── linux
│   │   │   └── platform.c
│   │   ├── list.c
│   │   ├── log.c
│   │   ├── ring_buffer.c
│   │   └── win9x
│   ├── main.c
│   ├── metrics
│   │   ├── cpu.c
│   │   ├── internal
│   │   │   ├── cpu_internal.h
│   │   │   ├── mem_internal.h
│   │   │   └── process_internal.h
│   │   ├── mem.c
│   │   ├── process.c
│   │   ├── process_manager.c
│   │   └── sampler.c
│   ├── sound.c
│   └── ui
│       ├── gtk
│       │   ├── app_context.c
│       │   ├── process_tab.c
│       │   ├── status_bar.c
│       │   └── ui.c
│       ├── tui
│       │   ├── internal
│       │   │   └── process_tab_internal.h
│       │   ├── process_tab.c
│       │   └── ui.c
│       └── win9x_ui
├── STRUCTURE.md
└── tests
    ├── include
    │   └── test.h
    └── src
        ├── core
        │   ├── hash_map.c
        │   └── ring_buffer.c
        ├── metrics
        │   ├── cpu.c
        │   ├── mem.c
        │   ├── process.c
        │   └── process_manager.c
        ├── sound.c
        └── test.c

70 directories, 64 files
```
