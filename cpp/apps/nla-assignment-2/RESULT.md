# Results

- Task 1: norm 1210497678.4449072
- Task 2: lambda_1 = 34645.143595853755, lambda_2 = 10623.769487838044
- Task 3: using cached artifacts/ATA.mtx
	- command `./build/cpp/apps/lis-eigen artifacts/ATA.mtx artifacts/eigenvectors.txt artifacts/history.txt -e pi -i cg -etol 1e-10`
- Task 4: command `./build/cpp/apps/lis-eigen artifacts/ATA.mtx artifacts/eigenvectors.txt artifacts/history.txt -e pi -i cg -etol 1e-10 -shift 5.6432e7`
- Task 5: norm(Sigma) = 41415.341203471995
- Task 6 & 7: k = 30, nnz(C) = 19680, nnz(D) = 19680, save_path = artifacts/deer_CD_30.png
	C: 656x30
	D: 656x30
- Task 6 & 7: k = 90, nnz(C) = 59040, nnz(D) = 59040, save_path = artifacts/deer_CD_90.png
	C: 656x90
	D: 656x90
- Task 8: norm = 72124.89168102785
- Task 9: path = artifacts/noisy_checkboard.png
- Task 10: sigma_1 = 51045.76786783796, sigma_2 = 50984.97168912582
- Task 11 & 12: k = 6, nnz(C) = 2400, nnz(D) = 2400, save_path = artifacts/noisy_checkboard_CD_06.png
	C: 400x6
	D: 400x6
- Task 11 & 12: k = 12, nnz(C) = 4800, nnz(D) = 4800, save_path = artifacts/noisy_checkboard_CD_12.png
	C: 400x12
	D: 400x12

## LIS runs

(use `etest1`).

`./build/cpp/apps/lis-eigen artifacts/ATA.mtx artifacts/eigenvectors.txt artifacts/history.txt -e pi -i cg -etol 1e-10`

```
number of processes = 1
matrix size = 656 x 656 (430336 nonzero entries)

initial vector x      : all components set to 1
precision             : double
eigensolver           : Power
convergence condition : ||lx-(B^-1)Ax||_2 <= 1.0e-10 * ||lx||_2
matrix storage format : CSR
shift                 : 0.000000e+00
eigensolver status    : normal end

Power: mode number          = 0
Power: eigenvalue           = 1.200286e+09
Power: number of iterations = 10
Power: elapsed time         = 3.251076e-03 sec.
Power:   preconditioner     = 0.000000e+00 sec.
Power:     matrix creation  = 0.000000e+00 sec.
Power:   linear solver      = 0.000000e+00 sec.
Power: relative residual    = 8.349221e-11
```


`./build/cpp/apps/lis-eigen artifacts/ATA.mtx artifacts/eigenvectors.txt artifacts/history.txt -e pi -i cg -etol 1e-10 -shift 5.6432e7`

```
number of processes = 1
matrix size = 656 x 656 (430336 nonzero entries)

initial vector x      : all components set to 1
precision             : double
eigensolver           : Power
convergence condition : ||lx-(B^-1)Ax||_2 <= 1.0e-10 * ||lx||_2
matrix storage format : CSR
shift                 : 5.643200e+07
eigensolver status    : normal end

Power: mode number          = 0
Power: eigenvalue           = 1.200286e+09
Power: number of iterations = 9
Power: elapsed time         = 2.987862e-03 sec.
Power:   preconditioner     = 0.000000e+00 sec.
Power:     matrix creation  = 0.000000e+00 sec.
Power:   linear solver      = 0.000000e+00 sec.
Power: relative residual    = 7.194899e-12
```
