import random

def generate_random_matrix(rows, cols):
    matrix = [[random.randint(1, 100) for _ in range(cols)] for _ in range(rows)]
    return matrix

def write_matrix_to_file(matrix, filename):
    with open(filename, 'w') as file:
        for row in matrix:
            file.write(' '.join(map(str, row)) + '\n')

rows = 1000  # количество строк
cols = 1000  # количество столбцов

matrix = generate_random_matrix(rows, cols)
write_matrix_to_file(matrix, 'example.txt')
