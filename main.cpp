#include <iostream>
#include "DispatchQueue.h"

int main() {
    std::cout << "Dispatch Tests..." << std::endl;

    DispatchQueue q(1);

    q.dispatch([&]{
        std::cout << "Dispatch 1!" << std::endl;
    });

    q.dispatch([&]{
        std::cout << "Dispatch 2!" << std::endl;
    });

    q.dispatch([&]{
        std::cout << "Dispatch 3!" << std::endl;
    });

    q.dispatch([&]{
        std::cout << "Dispatch 4!" << std::endl;
    });

    q.dispatch([&]{
        std::cout << "Dispatch 5!" << std::endl;
    });

    q.dispatch([&]{
        std::cout << "Dispatch 6!" << std::endl;
    });

    system("PAUSE");
    return 0;
}
