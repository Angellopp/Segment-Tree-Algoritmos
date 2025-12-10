#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <numeric>
#include "CSegmentTree.h"

// Función para consulta de suma de forma lineal
long long linearQuery(const std::vector<long long>& data, int l, int r) {
    long long sum = 0;
    for (int i = l; i <= r; ++i) {
        sum += data[i];
    }
    return sum;
}

// Función para actualización de rango de forma lineal
void linearUpdate(std::vector<long long>& data, int l, int r, int val) {
    for (int i = l; i <= r; ++i) {
        data[i] += val;
    }
}

struct Operation {
    bool isQuery; // true para query, false para update
    int l, r, val;
};

int main() {
    const int ARRAY_SIZE = 100000; 
    const int NUM_OPERATIONS = 100000;

    // --- 1. Generacion de los datos ---
    std::cout << "Generando " << ARRAY_SIZE << " elementos y " << NUM_OPERATIONS << " operaciones" << std::endl;
    std::vector<long long> data(ARRAY_SIZE);
    // Mersenne Twister algorithm
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> data_dist(1, 1000);
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        data[i] = data_dist(rng);
    }

    std::vector<Operation> operations(NUM_OPERATIONS);
    std::uniform_int_distribution<int> op_type_dist(0, 1);
    std::uniform_int_distribution<int> index_dist(0, ARRAY_SIZE - 1);
    std::uniform_int_distribution<int> val_dist(1, 100);

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        int l = index_dist(rng);
        int r = index_dist(rng);
        if (l > r) std::swap(l, r);
        operations[i] = {op_type_dist(rng) == 1, l, r, val_dist(rng)};
    }

    
    std::cout << "Segment Tree ---" << std::endl;
    CSegmentTree<long long, LazySumOp<long long>> sumTree(data);
    auto start_st = std::chrono::high_resolution_clock::now();
    for (const auto& op : operations) {
        if (op.isQuery) {
            sumTree.Query(op.l, op.r);
        } else {
            sumTree.Update(op.l, op.r, op.val);
        }
    }
    auto end_st = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_st = end_st - start_st;
    std::cout << "Tiempo total para " << NUM_OPERATIONS << " operaciones: " << duration_st.count() << " ms\n" << std::endl;

    // Enfoque Lineal ---
    std::cout << "Enfoque Lineal (bucle for) ---" << std::endl;
    std::vector<long long> linear_data = data; // Copia para el lineal
    auto start_linear = std::chrono::high_resolution_clock::now();
    for (const auto& op : operations) {
        if (op.isQuery) {
            linearQuery(linear_data, op.l, op.r);
        } else {
            linearUpdate(linear_data, op.l, op.r, op.val);
        }
    }
    auto end_linear = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_linear = end_linear - start_linear;
    std::cout << "Tiempo total para " << NUM_OPERATIONS << " operaciones: " << duration_linear.count() << " ms\n" << std::endl;
    // Cout <<
    std::cout << "\n Ejemplo de << con 10 elementos ---\n" << std::endl;
    std::vector<long long> small_st = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    CSegmentTree<long long, LazySumOp<long long>> smallSumTree(small_st);

    std::cout << "Vector inicial: ";
    for(const auto& val : small_st) std::cout << val << " ";
    std::cout << "\n" << std::endl;

    std::cout << "Estado inicial del arbol: " << std::endl;
    std::cout << smallSumTree << std::endl << std::endl;

    // Suma de 3+4+5+6 = 18
    std::cout << "Consulta rango [2, 5] (elementos en indices 2 a 5): " << smallSumTree.Query(2, 5) << std::endl; 
    
    std::cout << "\nActualizando rango [3, 7] sumando 10 a cada elemento" << std::endl << std::endl;
    smallSumTree.Update(3, 7, 10);

    std::cout << "Estado del arbol despues de actualizacion: " << smallSumTree << std::endl;

    // El rango [2,5] ahora contiene {3, 4+10, 5+10, 6+10}. La suma es 48
    std::cout << "Nueva consulta, rango [2, 5]: " << smallSumTree.Query(2, 5) << std::endl;
    std::cout << "Nueva consulta, todo el arbol [0, 9]: " << smallSumTree.Query(0, 9) << std::endl;

    return 0;
}