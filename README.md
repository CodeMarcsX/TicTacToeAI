# 🎮 Tic Tac Toe (Machine Learning)

This is a classic Tic Tac Toe game developed in C that uses **Reinforcement Learning (Q-Learning)** to train an artificial intelligence. The program offers a colorful terminal interface and three difficulty levels based on the AI's training volume.

---

## ✨ Features

- **Player vs Player Mode:** Play locally against a friend.
- **Player vs AI Mode:** Face an AI that learns by playing against itself through thousands of simulations.
- **Difficulty Levels:**
  - **Easy:** Fast training (500 iterations).
  - **Normal:** Balance between time and skill (3,000 iterations).
  - **Hard:** Highly competitive AI (200,000 iterations).
- **Immersive Interface:** *Typewriter* effect (typing) and ANSI colors for better visual feedback.
- **Cross-Platform:** Smart screen clearing system compatible with Windows, Linux, and macOS.

---

## ⬇️ Setup (Cloning the Repository)

If this project is hosted on GitHub, you can clone it to your local machine:

```bash

git clone https://github.com/CodeMarcsX/TicTacToeAI.git  
cd Tic-Tac-Toe-AI

```

---

## 🛠️ How to Compile and Run

### Requirements

You will need a C compiler installed, such as GCC or Clang.

### Compilation
```bash

gcc main.c -o tictactoe

```
---

## ▶️ Execution

### Linux/macOS
```bash

./tictactoe

```

### Windows
```bash

.\tictactoe.exe

```
---

⚠️ **Important:** If the banner or special characters appear "glitched" in the terminal, first set the code page to UTF-8:

```bash

chcp 65001

```

---

## 🚀 How to Play

1. **Choose Mode:** Select Player vs Player or Player vs AI in the main menu.

2. **Training:** If you choose AI, wait a few moments while it "learns" the best moves.

3. **Position Input:** The board uses a numeric mapping from 1 to 9:


```bash
      1 | 2 | 3
     ---+---+---
      4 | 5 | 6
     ---+---+---
      7 | 8 | 9
```


**Restart:** At the end, simply type `y` for a rematch or `n` to return to the main menu.

---

## 🧠 AI Logic (Q-Learning)

The AI uses a Q-Table to manage the 19,683 possible game states.

It learns through a reward system:

- Win: +1.0 point  
- Draw: +0.3 points  
- Loss: -1.0 point  

Moves are chosen based on learned values, with a "noise" factor (randomness) to ensure it explores new strategies instead of always repeating the same moves.

---

## 📚 About

Developed as a study of Artificial Intelligence and programming logic in C.
