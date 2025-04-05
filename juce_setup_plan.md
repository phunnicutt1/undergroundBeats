# Plan: JUCE Project Structure & CMake Setup (Subtask 1)

This plan outlines the steps to ensure the UndergroundBeats project structure conforms to a standard JUCE GUI Application and configure the root `CMakeLists.txt` accordingly.

**Goal:** Ensure a single, standard JUCE entry point and refine the CMake configuration for clarity.

**Analysis Summary:**

*   `**CMakeLists.txt**`**:** Generally well-structured for JUCE/CMake, defines library, app, and plugin targets. Minor refinement needed for source file clarity.
*   `**src/Main.cpp**`**:** Contains a complete JUCE GUI application entry point (`START_JUCE_APPLICATION`, `UndergroundBeatsApplication`, `MainWindow`). Meets requirements.
*   `**src/core/main.cpp**`**:** Contains a _second_, conflicting `START_JUCE_APPLICATION` macro. Needs removal.

**Detailed Plan:**

**Resolve Entry Point Conflict:**

*   **File:** `src/core/main.cpp`
*   **Action:** Remove or comment out line 9: `START_JUCE_APPLICATION(undergroundBeats::core::Application)`
*   **Reason:** Establish `src/Main.cpp` as the single, definitive entry point.

**Refine** `**CMakeLists.txt**` **for Clarity:**

*   **File:** `CMakeLists.txt`
*   **Action 1:** Remove `src/Main.cpp` from the `SOURCES` list within the `set(SOURCES ...)` block (around line 67).
*   **Action 2:** Ensure the `target_sources(undergroundBeats PRIVATE src/Main.cpp)` block (lines 137-140) remains.
*   **Reason:** Explicitly define the application entry point source only for the application target, not the general library.
*   **Verification:** Confirm required JUCE modules (`juce_core`, `juce_gui_basics`, `juce_audio_basics`, `juce_audio_devices`, `juce_audio_processors`) remain linked to `undergroundBeatsLib` (lines 104-118). (Confirmed: They are present).

**No Changes to** `**src/Main.cpp**`**:**

*   **File:** `src/Main.cpp`
*   **Action:** None required.
*   **Reason:** Already fulfills requirements for the JUCE application class and basic window initialization.

**Visual Plan:**

```
graph TD
    A[Start] --> B{Review Files};
    B --> C[CMakeLists.txt: OK, minor refinement needed];
    B --> D[src/Main.cpp: OK, standard JUCE entry point];
    B --> E[src/core/main.cpp: CONFLICT - Remove entry point];

    E --> F[Plan: Modify src/core/main.cpp];
    C --> G[Plan: Refine CMakeLists.txt];
    D --> H[Plan: No changes needed for src/Main.cpp];

    F --> I{Implement Changes};
    G --> I;
    H --> I;

    I --> J[End];
```

**Next Step:** Switch to Code mode to implement these changes.