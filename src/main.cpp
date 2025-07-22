#include <iostream>
#include "board.h"

int main() {
    Board board;
    std::cout << "Initial board:\n";
    board.print();

    while (true) {
        std::string from, to;
        std::cout << "Enter move (e.g. e2 e4, or 'quit'): ";
        if (!(std::cin >> from)) break;
        if (from == "quit" || from == "exit") break;
        if (!(std::cin >> to)) break;

        try {
            board.movePiece(from, to);
            board.print();
        }
        catch (const std::exception& ex) {
            std::cerr << "Invalid move: " << ex.what() << "\n";
        }
    }
    return 0;
}
