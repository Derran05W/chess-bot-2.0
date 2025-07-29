# Chess Bot 🏰♟️

Welcome to **Chess Bot**, a lightweight C++ program that plays chess using bitboards and backtracking. Whether you’re a chess fan, learning C++ bit manipulation, or just exploring AI strategies, this project is for you!

---

## ✨ Features

- **Bitboard Representation**  
  Efficiently stores the entire board in 64-bit integers for very fast move generation.
- **Backtracking Search**  
  Implements a simple minimax/backtracking algorithm to explore moves up to a configurable depth.
- **Easy to Build & Run**  
  One-line build, no external dependencies (just a C++20 compiler).
- **Customizable**  
  Tweak search depth, evaluation parameters, or add new move heuristics in minutes.

---

## 🛠️ Getting Started

### Prerequisites

- A Linux or macOS machine (Windows users can use WSL).
- **g++** (supports C++20).
- **make** (optional, for convenience).

### Installation

1. **Clone the repo**  

First in terminal, clone the repo by doing: 
   
```git clone https://github.com/your-username/chess-bot.git ```

Then, swap into the directory
   
```cd chess-bot

And finally, use Make to build and run the bot!
   
```make && ./chess-bot

### Play the bot
Currently, this is just a console app, so input the coordinate of the piece and where you would like to place it to make your move. Enter "Quit" to end the game early. 
Visuals coming soon!
