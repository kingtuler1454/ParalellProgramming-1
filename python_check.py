import numpy as np
import sys


matrix1_path=sys.argv[1]
matrix2_path=sys.argv[2]

matrix1 = np.genfromtxt(matrix1_path)
matrix2 = np.genfromtxt(matrix2_path)


result = np.dot(matrix1, matrix2)


np.savetxt("data\matrix_result_python.txt", result, fmt='%d')
