#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <omp.h>
#include <vector>
#include <cmath>
#include <numeric>

using namespace std;

double calculateMean(const std::vector<int>& times) {
    double sum = std::accumulate(times.begin(), times.end(), 0);
    return sum / times.size();
}

double calculateStandardDeviation(const std::vector<int>& times, double mean) {
    double variance = 0;
    for (int time : times) {
        variance += pow(time - mean, 2);
    }
    return sqrt(variance / times.size());
}

int start(std::vector<int>& times) {


    double mean = calculateMean(times);
    double standardDeviation = calculateStandardDeviation(times, mean);

    double confidenceLevel = 1.96; // Для доверительного интервала с уровнем доверия 95%
    double marginOfError = confidenceLevel * standardDeviation / sqrt(times.size());

    double lowerBound = mean - marginOfError;
    double upperBound = mean + marginOfError;

    std::cout << "Interval min: \t" << lowerBound << "\t max: \t" << upperBound << "\n" << std::endl;

    return 0;
}


int SearchSizeOfMatrix(string name)
{
    int rows = 0;
    ifstream myfile;
    myfile.open(name);

    if (myfile.is_open()) {
        string line;
        while (getline(myfile, line))rows++;

        myfile.close();
        return rows; // Возвращаем количество строк в матрице
    }
    else {
        cout << "Cannot open file '" << name << "', press any key to continue" << endl;
        getchar();
        return 0; // Возвращаем 0, чтобы указать на ошибку
    }
}

int** CreateMatrix(int size) {
    int** Matrix = new int* [size];
    for (int i = 0; i < size; i++)
    {
        Matrix[i] = new int[size];
    }
    return Matrix;
}


void FreeMatrix(int** Matrix, int size) {
    for (int i = 0; i < size; i++) {
        delete[] Matrix[i];
    }
    delete[] Matrix;
}


void ReadMatrixFromFile(string path, int** matrix, int size) {
    ifstream file(path);
    if (file.is_open()) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                file >> matrix[i][j];
            }
        }
        file.close();
    }
    else {
        cout << "Unable to open file: " << path << endl;
    }
}
int** Multiplication(int** MatrixOne, int** MatrixTwo, int size, int count_thread) {

    int** MatrixCppFinal = CreateMatrix(size);
     int threads;
#pragma omp parallel num_threads(count_thread) shared(threads)
    {
        threads = omp_get_num_threads();
#pragma omp for
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int sum = 0;
                for (int k = 0; k < size; k++) {
                    sum += MatrixOne[i][k] * MatrixTwo[k][j];
                }
                MatrixCppFinal[i][j] = sum;
            }
        }
    }

    return MatrixCppFinal;

}


void WriteMatrix(int** Matrix, int size) {
    ofstream outFile("data\\matrix3.txt");
    if (outFile.is_open()) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                outFile << Matrix[i][j] << " ";
            }
            outFile << endl;
        }
        outFile.close();
        //cout << "Matrix has been successfully written" << endl;
    }
    else {
        cout << "Unable to open file matrix3 for writing." << endl;
    }
}


bool Verification(int** CppMatrix, int size, string path1, string path2) {


    std::string command = "C:\\Users\\smorozov\\AppData\\Local\\anaconda3\\python python_check.py  " + path1 + " " + path2;

    system(command.c_str());

    int** PythonMatrix = CreateMatrix(size);
    ReadMatrixFromFile("data\\matrix_result_python.txt", PythonMatrix, size);


    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (PythonMatrix[i][j] != CppMatrix[i][j])
            {
                cout << "Matrix has different\n";
                return false;
            }
        }
    }
   // cout << "Multiplication of matrix is True" << endl;
    FreeMatrix(PythonMatrix, size);
    return true;

}


auto cycle(string path1, string path2, int count_thread) {
   // cout << "______________________Work with files: " << path1 << " and " << path2 << "______________________" << endl;
    int size1 = SearchSizeOfMatrix(path1);
    int size2 = SearchSizeOfMatrix(path2);

    int** MatrixOne = CreateMatrix(size1);
    int** MatrixTwo = CreateMatrix(size2);

    ReadMatrixFromFile(path1, MatrixOne, size1);
    ReadMatrixFromFile(path2, MatrixTwo, size2);

    auto start_time = chrono::high_resolution_clock::now();
    int** MatrixCppFinal = Multiplication(MatrixOne, MatrixTwo, size1, count_thread);
    auto end_time = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);



    WriteMatrix(MatrixCppFinal, size1);
    Verification(MatrixCppFinal, size1, path1, path2);


    FreeMatrix(MatrixOne, size1);
    FreeMatrix(MatrixTwo, size2);
    FreeMatrix(MatrixCppFinal, size1);
    
    return duration.count();
}


void cycle_with_matrix_and_threads(string path1, string path2, int count_thread) {
    vector<int> times;
    for (int i = 0; i < 10; i++) {
        times.push_back(cycle(path1, path2, 20));
    }
    std::cout << "Size of Matrix:" << SearchSizeOfMatrix(path1) << "x" << SearchSizeOfMatrix(path2) << "\t count_threads: \t" << count_thread << "\t";
    start(times);

}

int main() {
    const int count_of_threads = 4;
    int option_threads[count_of_threads] = { 1, 6, 12, 20};



    for (int j = 1; j < 4; j++) // сначала для размера матриц 500 на 500,1000 на 1000, 1500 на 1500
    {
        for (int i = 0; i < count_of_threads; i++) //для каждого из количества потоков
        {
            std::string path1 = "data\\A" + std::to_string(j) + ".txt";
            std::string path2 = "data\\B" + std::to_string(j) + ".txt";
            cycle_with_matrix_and_threads(path1,path2, option_threads[i]);
        }
    }

    return 0;

}