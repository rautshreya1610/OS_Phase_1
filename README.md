# MOS Phase 1 — Multiprogramming Operating System Simulation

### VIT Pune | Operating Systems Lab | 2025-26

A C++ simulation of a single-task batch Operating System demonstrating core OS concepts.

## What This Project Does
- Simulates a basic OS with 100x4 char memory
- Executes 7 instructions: LR, SR, CR, BT, GD, PD, H
- Handles I/O via software interrupts (SI register)
- Processes 15 batch jobs from input.txt

- | Real Computer | MOS Simulation |
|---|---|
| RAM | `M[100][4]` — 100 boxes of 4 chars each |
| CPU Registers | `IR, IC, R, C, SI` — C++ variables |
| Running a program | `EXECUTE_USER_PROGRAM()` function |
| OS Kernel | `MOS()` function |
| Reading input | `READ()` via SI=1 interrupt |
| Writing output | `WRITE()` via SI=2 interrupt |
| System Call | Setting SI register |

---

## ⚙️ Instruction Set (7 Instructions)

| Instruction | Meaning | What It Does |
|---|---|---|
| `GD XY` | Get Data | Read 40 chars from input into M[XY..XY+9] |
| `PD XY` | Put Data | Print M[XY..XY+9] to output |
| `LR XY` | Load Register | Copy M[XY] into Register R |
| `SR XY` | Store Register | Copy Register R into M[XY] |
| `CR XY` | Compare Register | Check if R == M[XY], set flag C |
| `BT XY` | Branch if True | If C is true, jump to instruction XY |
| `H` | Halt | Stop the current program |

------

## 🔔 Interrupt Mechanism

| SI Value | Triggered By | Handler | Action |
|---|---|---|---|
| 0 | Default | — | Normal execution |
| 1 | GD instruction | READ() | Read 40 chars from input into memory |
| 2 | PD instruction | WRITE() | Write 40 chars from memory to output |
| 3 | H instruction | TERMINATE() | End job, write blank line |

---

## 🖥️ How to Run

```bash
# Step 1: Compile
g++ -o mos mos.cpp

# Step 2: Run
mos.exe          # Windows
./mos            # Linux / Mac

# Step 3: See output
type output.txt  # Windows
cat output.txt   # Linux / Mac
```

---

## 📁 Project Files

| File | Description |
|---|---|
| `mos.cpp` | Main C++ source — the entire OS simulation |
| `input.txt` | 15 batch jobs to run |
| `output.txt` | Results after execution |
| `MOS_Complete_Documentation.docx` | Full project documentation |
| `MOS_Phase1_Report.docx` | Lab report |

---

## 💡 OS Concepts Demonstrated

- ✅ Batch Processing
- ✅ Fetch-Decode-Execute CPU Cycle
- ✅ Interrupt Handling (System Calls)
- ✅ Dual Mode Operation (User vs Kernel)
- ✅ Memory Management (100×4 word memory)
- ✅ Job Control Language ($AMJ/$DTA/$END)

---

## 👩‍💻 Developer

**Shreya Raut**
SE Computer Engineering | VIT Pune | 2024–25
GitHub: [@rautshreya1610](https://github.com/rautshreya1610)

## 🗂️ Job Format in input.txt

## How to Run
g++ -o mos mos.cpp
mos.exe

## Files
- mos.cpp → Main OS simulation (C++ source)
- input.txt → 15 test jobs
- output.txt → Program output

## Team
- Shreya Raut | SE Comp | VIT Pune
