#include <iostream>
#include <cstdlib>
#include <ctime>
#include "omp.h"

using namespace std;

const int rows = 20000;
const int cols = 20000;

int min_row = 0;
int sum = INT_MAX;

int matrix[rows][cols];

void init_arr();
long long part_sum(int, int, int);
void part_min(int, int, int);

int main() {
    init_arr();

    omp_set_nested(1);
    double t1 = omp_get_wtime();
#pragma omp parallel sections
    {
#pragma omp section
        {
            for (int i = 1; i <= 32; i = i * 2)
            {
                double tempT1 = omp_get_wtime();
                part_min(0, rows, i);
                double tempT2 = omp_get_wtime();
                cout << "Time for min " << i <<" = " << tempT2 - tempT1 << " sec" << endl;
            }
        }
#pragma omp section
        {
            for (int i = 1; i <= 32; i = i * 2)
            {
                double tempT1 = omp_get_wtime();
                cout << "sum "<< i <<" = " << part_sum(0, rows, i) << endl;
                double tempT2 = omp_get_wtime();
                cout << "Time for sum "<< i <<" = " << tempT2 - tempT1 << " sec" << endl;
            }
        }
    }
    double t2 = omp_get_wtime();

    cout << "Total time = " << t2 - t1 << " sec" << endl;
    cout << "Row with min sum: " << min_row << ", Sum: " << sum << endl;
    return 0;
}

void init_arr() {
    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = rand() % 100;
        }
    }
}

long long part_sum(int start_row, int end_row, int num_threads) {
    long long total_sum = 0;
#pragma omp parallel for reduction(+:total_sum) num_threads(num_threads)
    for (int i = start_row; i < end_row; i++) {
        long long row_sum = 0;
        for (int j = 0; j < cols; j++) {
            row_sum += matrix[i][j];
        }
        total_sum += row_sum;
    }
    return total_sum;
}

void part_min(int start_row, int end_row, int num_threads) {
#pragma omp parallel for  num_threads(num_threads) // Кількість ітерацій розділена між потоками
    for (int i = start_row; i < end_row; i++) {
        long long row_sum = 0;
        for (int j = 0; j < cols; j++) {
            row_sum += matrix[i][j];
        }
        if (row_sum < sum) {
#pragma omp critical
            {
                if (row_sum < sum) {
                    sum = row_sum;
                    min_row = i;
                }
            }
        }
    }
}

//Виконання ітерацій : Кожен потік виконує свої ітерації незалежно від інших потоків.Це означає, що кожен потік виконує код циклу для свого діапазону індексів.
//Збір результатів : Після завершення виконання всіх ітерацій кожен потік повертає свій результат.У випадку використання конструкції reduction або critical, результати можуть бути об'єднані або згруповані, щоб отримати загальний результат.
//Завершення циклу : Коли кожен потік завершив виконання своїх ітерацій, він продовжує виконання наступного коду після циклу, якщо такий є.