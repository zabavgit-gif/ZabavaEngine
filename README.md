<div align="center">
  <img src="https://cdn-icons-png.flaticon.com/512/3565/3565418.png" alt="ZabavaEngine Logo" width="100" height="100">

  # ZabavaEngine 🧠♟️

  **A high-performance UCI chess engine written in C++ focusing on efficiency and classic search algorithms.**

  [![C++ Version](https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg)](https://isocpp.org/)
  [![CMake](https://img.shields.io/badge/CMake-Ready-success.svg)](https://cmake.org/)
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)

</div>

<br>

## 📖 About the Project

**ZabavaEngine** is a complete, from-scratch UCI-compatible chess engine. It features a fast alpha-beta searcher with quiescence search, Zobrist hashing, tapered evaluation with piece-square tables, and full move generation that correctly handles all special chess rules (castling, en passant, promotion).

The engine is designed as a showcase of **C++ systems programming** — low-level optimizations, memory management, and high-performance search algorithms. It works with any modern chess GUI (Arena, Cute Chess, Fritz, etc.).

> **Developer's Note:** This project was built as a deep dive into chess engine architecture. I implemented the core algorithms, debugged move generation, optimized the search, and ensured 100% legal moves through extensive Perft testing.

---

## 🚀 Features & Capabilities

### Core Engine (Completed)
- **Search:** Alpha-Beta Pruning + Quiescence Search + Null Move Pruning + Late Move Reductions
- **Move Ordering:** MVV-LVA, Killer Moves, History Heuristics
- **Evaluation:** Tapered Eval (MG/EG) with Piece-Square Tables + pawn shield safety
- **Hashing:** Zobrist Hashing + Transposition Table (1M entries)
- **Protocol:** Full UCI support
- **Verification:** Perft (Performance Test) suite for 100% move generation accuracy

---

## 🛠 Tech Stack

<div align="center">
  <a href="https://isocpp.org/"><img src="https://skillicons.dev/icons?i=cpp" alt="C++" /></a>
  <a href="https://cmake.org/"><img src="https://skillicons.dev/icons?i=cmake" alt="CMake" /></a>
  <a href="https://gcc.gnu.org/"><img src="https://skillicons.dev/icons?i=linux" alt="Linux" /></a>
  <a href="https://www.jetbrains.com/clion/"><img src="https://skillicons.dev/icons?i=clion" alt="CLion" /></a>
</div>

---

## 🎮 How to Run

The project builds **two separate executables**:

### ♟️ ZabavaEngine
* Automatically detects mode:
 * **UCI Mode** — connect to any chess GUI
  * **Console Mode** — play against the engine directly in terminal (`e2e4` format)

### 🧪 ZabavaTests
* Runs Perft test suite
* **Perft Suites:** Executes deep performance tests to ensure move generation accuracy.
* **Validation:** Verifies engine logic across various complex chess positions (Kiwipete, Endgames, etc.).
    
---

## 📥 Installation & Build

The project uses **CMake** for cross-platform building, but can also be compiled manually via terminal.

```bash
# 1. Clone
git clone https://github.com/zabavgit-gif/ZabavaEngine.git
cd ZabavaEngine

# 2. Build (Recommended)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# 3. Run
./ZabavaEngine       # Run the engine
./ZabavaTests        # Run the tests
               
