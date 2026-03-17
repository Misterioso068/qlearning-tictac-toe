# Tic-Tac-Toe AI

A **reinforcement learning (Q-learning) Tic-Tac-Toe agent** written in C.  
This program can train itself via self-play, store its learned intelligence, and play against a human with adjustable difficulty.

---

## Features

- **Self-play training:** The agent improves by playing games against itself.  
- **Human play mode:** Play against the trained agent.  
- **Adjustable difficulty:** Control randomness/exploration with `epsilon`.  
- **Persistent intelligence:** Learned strategies are saved/loaded from a file.

---

## Implementation

- **Q-Learning:** The agent maintains a Q-table of size `19683 × 9` (3^9 board states × 9 possible moves).  
- **Epsilon-greedy policy:** Chooses the best move most of the time, but occasionally explores random moves.  
- **Self-play training:** One Q-table is used for both “players,” so the agent learns optimal strategies over time.  
- **Board encoding:** Each board is encoded as a base-3 number to uniquely identify states.  

---

## Requirements

- GCC (or compatible C compiler)  
- Linux, macOS, or Windows environment  
- Make (optional, recommended for building)  

---

## Compilation

Build the program using the included `Makefile`:
```bash
make
```

This will compile the program and place the executable in the `build/` folder:
```bash
build/tictactoe
```

To clean build artifacts:
```bash
make clean
```

* * * * *

Usage
-----

The program is run from the terminal using **command-line arguments**.

### Required flags

-   `-M <mode>` -- Mode of operation:

    -   `play` -- Play against the agent.

    -   `train` -- Train the agent via self-play.

-   `-F <filename>` -- Path to the existing Q-table file or path you want the file made and stored.

### Optional flags

-   `-D <difficulty>` -- Set the AI difficulty when playing against it:

    -   `easy` (epsilon = 0.5)

    -   `medium` (epsilon = 0.3)

    -   `hard` (epsilon = 0.1)
    
    -   `impossible` (epsilon = 0.0)

### Examples
Train a new agent:
```bash
./build/tictactoe -M train -F agents/agent1.bin
```

Play against the trained agent (hard difficulty):
```bash
./build/tictactoe -M play -F agents/agent1.bin -D hard
```

* * * * *

Project Structure
-----------------
```bash
.
├── agents
│   ├── agent0.bin
│   └── agent1.bin
├── build
│   └── tictactoe
├── include
│   └── error_logger.h
├── LICENSE
├── Makefile
├── README.md
└── src
    └── main.c
```
* * * * *

How It Works
------------

### Training Mode

-   The agent plays itself repeatedly.

-   Q-values are updated using the formula: `Q(s,a) ← Q(s,a) + α * (reward - Q(s,a))`

-   After enough iterations, the agent learns perfect play.

### Play Mode

-   Human plays as X; the agent plays as O.

-   The AI chooses moves using the learned Q-table and `epsilon` for exploration.

* * * * *

License
-------

This project is licensed under the MIT License.