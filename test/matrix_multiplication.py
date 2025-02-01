import random

# size of matrix
N = 32

# base addresses of matrices (collides if N too big)
ADDR_STEP = 0x0004
BASE_A = 0x00000
BASE_B = BASE_A + (N * N * ADDR_STEP)
BASE_C = BASE_B + (N * N * ADDR_STEP)

# initialize matrices
A = [[random.randint(1, 100) for j in range(N)] for i in range(N)]
B = [[random.randint(1, 100) for j in range(N)] for i in range(N)]
C = [[0] * N for _ in range(N)]  # result in C

logfile = "matrix_access.csv"

def get_address(matrix, row, col):
    if matrix == 'A':
        base = BASE_A
    elif matrix == 'B':
        base = BASE_B
    elif matrix == 'C':
        base = BASE_C
    return base + (row * N + col) * ADDR_STEP

def log_memory_access(access_type, address, value=None):
    with open(logfile, mode="a") as file:
        if value is not None:
            file.write(f"{access_type}, {address}, {value}\n")
        else:
            file.write(f"{access_type}, {address}\n")

# clear file
open(logfile, "w").close()

# multiplication
for i in range(N):
    for j in range(N):
        c_addr = get_address('C', i, j)  # address of C[i][j]
        temp = 0 # CPU register is used here!

        for k in range(N):
            a_addr = get_address('A', i, k)  # address of A[i][k]
            b_addr = get_address('B', k, j)  # address of B[k][j]

            # read A[i][k] and B[k][j]
            log_memory_access("R", a_addr)
            log_memory_access("R", b_addr)

            # aggregate partial results
            temp += A[i][k] * B[k][j]

        # write to C[i][j]
        C[i][j] = temp
        log_memory_access("W", c_addr, C[i][j])
