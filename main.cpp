#include <iostream>
#include <vector>
#include "CSegmentTree.h"
int main() {
    std::vector<int> data = {15, 20, 12, 30, 45, 10, 50, 25};

    // CSegmentTree<int, MinOp<int>> minTree(data);
    
    // CSegmentTree<int, MaxOp<int>> maxTree(data);

    CSegmentTree<int, LazySumOp<int>> sumTree(data);


    std::cout << "--- Estado Inicial ---" << std::endl;
    // std::cout << "Minimo (2-6): " << minTree.Query(2, 6) << std::endl;
    // std::cout << "Maximo (2-6): " << maxTree.Query(2, 6) << std::endl;
    std::cout << "Suma (2-6): " << sumTree.Query(2, 6) << std::endl;

    std::cout << "\n[!] Actualizo data[5,6,7] adicionando 10 a cada valor" << std::endl;
    // minTree.Update(5, 100);
    // maxTree.Update(5, 100);
    sumTree.Update(5, 7, 10);

    // std::vector<int> data = {15, 20, 12, 30, 45, 100, 50, 25};

    // std::cout << "--- Tras Actualizacion ---" << std::endl;
    // std::cout << "Minimo (2-6): " << minTree.Query(2, 6) << std::endl;
    // std::cout << "Maximo (2-6): " << maxTree.Query(2, 6) << std::endl;
    std::cout << "Suma (2-6): " << sumTree.Query(2, 6) << std::endl;

    return 0;
}