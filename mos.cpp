/*
 * ============================================================
 *  MOS - Multiprogramming Operating System (Phase 1)
 *  Single-Task Simulation | VIT Pune | OS Lab Project
 * ============================================================
 *  Simulates a basic OS with:
 *   - Memory: 100 locations, each 4 characters wide
 *   - Registers: IR, IC, R, C, SI
 *   - Instructions: LR, SR, CR, BT, GD, PD, H
 *   - Interrupt Handling via SI register
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
using namespace std;

// ─── Constants ────────────────────────────────────────────
const int MEM_SIZE   = 100;   // Total memory locations
const int WORD_SIZE  = 4;     // Each location holds 4 chars

// ─── Global Hardware Simulation ───────────────────────────
char M[MEM_SIZE][WORD_SIZE + 1]; // Memory (null-terminated words)
char IR[WORD_SIZE + 1];          // Instruction Register
char R[WORD_SIZE + 1];           // General Purpose Register
int  IC  = 0;                    // Instruction Counter (Program Counter)
bool C   = false;                // Condition Flag
int  SI  = 0;                    // Service Interrupt: 1=READ, 2=WRITE, 3=HALT

// ─── I/O File Streams ─────────────────────────────────────
ifstream inputFile;
ofstream outputFile;

// ──────────────────────────────────────────────────────────
//  UTILITY: Clear a memory word to spaces
// ──────────────────────────────────────────────────────────
void clearWord(char* word) {
    for (int i = 0; i < WORD_SIZE; i++) word[i] = ' ';
    word[WORD_SIZE] = '\0';
}

// ──────────────────────────────────────────────────────────
//  UTILITY: Print current memory state (for debugging)
// ──────────────────────────────────────────────────────────
void dumpMemory() {
    cout << "\n--- Memory Dump ---\n";
    for (int i = 0; i < MEM_SIZE; i++) {
        if (M[i][0] != '\0' && M[i][0] != ' ') {
            cout << "M[" << i << "] = [" << M[i] << "]\n";
        }
    }
    cout << "-------------------\n";
}

// ──────────────────────────────────────────────────────────
//  READ() — Interrupt Service Routine for SI = 1
//  Reads one 40-char data line from input, stores into
//  10 consecutive memory locations starting at address
//  specified in IR[2..3]
// ──────────────────────────────────────────────────────────
void READ() {
    // Extract address from IR positions 2 and 3
    int address = (IR[2] - '0') * 10 + (IR[3] - '0');

    char buffer[41];
    memset(buffer, ' ', 40);
    buffer[40] = '\0';

    // Read a line from input file
    if (inputFile.getline(buffer, 41)) {
        // Pad with spaces if shorter than 40
        int len = strlen(buffer);
        for (int i = len; i < 40; i++) buffer[i] = ' ';
    }

    // Store 10 words (4 chars each) into memory
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < WORD_SIZE; j++) {
            M[address + i][j] = buffer[i * WORD_SIZE + j];
        }
        M[address + i][WORD_SIZE] = '\0';
    }
}

// ──────────────────────────────────────────────────────────
//  WRITE() — Interrupt Service Routine for SI = 2
//  Writes 10 consecutive memory words to output file
//  starting at address from IR[2..3]
// ──────────────────────────────────────────────────────────
void WRITE() {
    int address = (IR[2] - '0') * 10 + (IR[3] - '0');

    for (int i = 0; i < 10; i++) {
        outputFile << M[address + i];
    }
    outputFile << "\n";
}

// ──────────────────────────────────────────────────────────
//  TERMINATE() — Interrupt Service Routine for SI = 3
//  Marks end of program execution
// ──────────────────────────────────────────────────────────
void TERMINATE() {
    outputFile << "\n";  // Blank line after each job
    SI = 0;              // Reset SI
}

// ──────────────────────────────────────────────────────────
//  MOS() — Master Mode Interrupt Handler
//  Called whenever SI is set by user program
// ──────────────────────────────────────────────────────────
void MOS() {
    switch (SI) {
        case 1: READ();      break;
        case 2: WRITE();     break;
        case 3: TERMINATE(); break;
        default:
            outputFile << "[ERROR] Unknown interrupt: SI = " << SI << "\n";
    }
    SI = 0;  // Clear interrupt after handling
}

// ──────────────────────────────────────────────────────────
//  EXECUTE_USER_PROGRAM() — Slave Mode
//  Fetches, decodes, and executes one instruction at a time
//  Returns false when program should stop (H instruction)
// ──────────────────────────────────────────────────────────
bool EXECUTE_USER_PROGRAM() {
    // ── FETCH: Load instruction from memory[IC] into IR ──
    strncpy(IR, M[IC], WORD_SIZE);
    IR[WORD_SIZE] = '\0';
    IC++;  // Advance instruction counter

    // ── DECODE & EXECUTE ──────────────────────────────────
    // Each instruction occupies 4 chars: [OP][OP][A][A]
    // e.g., "LR10" → opcode=LR, address=10

    // ── LR: Load Register ─────────────────────────────────
    // LR XY → R = M[XY]
    if (IR[0] == 'L' && IR[1] == 'R') {
        int addr = (IR[2] - '0') * 10 + (IR[3] - '0');
        strncpy(R, M[addr], WORD_SIZE);
        R[WORD_SIZE] = '\0';
    }

    // ── SR: Store Register ────────────────────────────────
    // SR XY → M[XY] = R
    else if (IR[0] == 'S' && IR[1] == 'R') {
        int addr = (IR[2] - '0') * 10 + (IR[3] - '0');
        strncpy(M[addr], R, WORD_SIZE);
        M[addr][WORD_SIZE] = '\0';
    }

    // ── CR: Compare Register ─────────────────────────────
    // CR XY → C = (R == M[XY])
    else if (IR[0] == 'C' && IR[1] == 'R') {
        int addr = (IR[2] - '0') * 10 + (IR[3] - '0');
        C = (strncmp(R, M[addr], WORD_SIZE) == 0);
    }

    // ── BT: Branch if True ────────────────────────────────
    // BT XY → if C is true, IC = XY
    else if (IR[0] == 'B' && IR[1] == 'T') {
        if (C) {
            IC = (IR[2] - '0') * 10 + (IR[3] - '0');
        }
    }

    // ── GD: Get Data (triggers READ interrupt) ────────────
    // GD XY → SI = 1, address stored in IR for MOS
    else if (IR[0] == 'G' && IR[1] == 'D') {
        SI = 1;
        MOS();  // Handle interrupt in master mode
    }

    // ── PD: Put Data (triggers WRITE interrupt) ───────────
    // PD XY → SI = 2, address stored in IR for MOS
    else if (IR[0] == 'P' && IR[1] == 'D') {
        SI = 2;
        MOS();  // Handle interrupt in master mode
    }

    // ── H: Halt (triggers TERMINATE interrupt) ────────────
    // H → SI = 3
    else if (IR[0] == 'H') {
        SI = 3;
        MOS();  // Handle interrupt in master mode
        return false;  // Stop execution loop
    }

    return true;  // Continue execution
}

// ──────────────────────────────────────────────────────────
//  START_EXECUTION() — Begins execution of loaded program
//  Loops EXECUTE_USER_PROGRAM() until H is hit
// ──────────────────────────────────────────────────────────
void START_EXECUTION() {
    bool running = true;
    while (running) {
        running = EXECUTE_USER_PROGRAM();
    }
}

// ──────────────────────────────────────────────────────────
//  LOAD() — Loads job into memory
//  Reads lines after $AMJ until $DTA is found
//  Each line packs up to 10 instructions (4 chars each)
// ──────────────────────────────────────────────────────────
void LOAD() {
    // Clear all memory
    for (int i = 0; i < MEM_SIZE; i++) {
        clearWord(M[i]);
    }

    IC = 0;  // Reset instruction counter
    int memPtr = 0;  // Current memory address to write

    string line;
    while (getline(inputFile, line)) {
        // Pad line to 40 characters
        while ((int)line.size() < 40) line += ' ';

        // Check for $DTA - end of program section
        if (line.substr(0, 4) == "$DTA") {
            break;
        }

        // Store line as 10 words of 4 chars each
        for (int i = 0; i < 10 && memPtr < MEM_SIZE; i++) {
            for (int j = 0; j < WORD_SIZE; j++) {
                M[memPtr][j] = line[i * WORD_SIZE + j];
            }
            M[memPtr][WORD_SIZE] = '\0';
            memPtr++;
        }
    }
}

// ──────────────────────────────────────────────────────────
//  MAIN — Driver: reads $AMJ ... $DTA ... $END blocks
// ──────────────────────────────────────────────────────────
int main() {
    inputFile.open("input.txt");
    outputFile.open("output.txt");

    if (!inputFile.is_open()) {
        cerr << "[FATAL] Cannot open input.txt\n";
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        // Scan for start-of-job card
        if (line.substr(0, 4) == "$AMJ") {
            LOAD();          // Load program into memory
            START_EXECUTION();  // Execute loaded program
        }
        // $END card is implicitly handled by LOAD/TERMINATE
    }

    inputFile.close();
    outputFile.close();

    cout << "Execution complete. Check output.txt for results.\n";
    return 0;
}
