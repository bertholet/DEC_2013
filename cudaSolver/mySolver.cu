#include "mySolver.h"
#include <iostream>

#include <stdio.h>

#include <thrust/version.h>
#include <cusp/version.h>
#include <cusp/csr_matrix.h>
#include <cusp/print.h>
//#include <cusp/krylov/cg.h>
#include <cusp/krylov/bicgstab.h>

//#include <cusp/precond/smoothed_aggregation.h>
//#include <cusp/precond/ainv.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

//Hide Cuda Api calls
cudaError_t addWithCuda_(int *c, const int *a, const int *b, size_t size);
__global__ void addKernel_(int *c, const int *a, const int *b);


CUDASOLVER_EXPORT mySolver::mySolver()
{
	myMatrix = new cusp::csr_matrix<int,float,cusp::device_memory>();
}

CUDASOLVER_EXPORT mySolver::~mySolver()
{
	delete myMatrix;
}


void CUDASOLVER_EXPORT mySolver::setMatrix( cpuCSRMatrix & mat )
{
	delete myMatrix;
	myMatrix = new cusp::csr_matrix<int,float,cusp::device_memory>(mat.getn(), mat.getm(), mat.geta().size());
	updateMatrix(mat);

	//cusp::print(*myMatrix);
}


void CUDASOLVER_EXPORT mySolver::updateMatrix(cpuCSRMatrix & mat)
{
	myMatrix->column_indices = cusp::array1d<float, cusp::host_memory>(mat.getja().begin(), mat.getja().end());
	myMatrix->row_offsets = cusp::array1d<float, cusp::host_memory>(mat.getia().begin(), mat.getia().end());
	myMatrix->values = cusp::array1d<float, cusp::host_memory>(mat.geta().begin(), mat.geta().end());

}



void CUDASOLVER_EXPORT mySolver::solve( floatVector & x, floatVector & b )
{
	cusp::csr_matrix<int,float,cusp::device_memory> & A = *myMatrix;

	// allocate storage for solution (x) and right hand side (b)
    cusp::array1d<float, cusp::device_memory> x_(x.size(), 0);//x_(x.begin(), x.end());
	cusp::array1d<float, cusp::device_memory> b_(b.begin(), b.end());

    // set stopping criteria:
    //  iteration_limit    = 100
    //  relative_tolerance = 1e-3
    cusp::convergence_monitor<float> monitor(b_, 1000, 1e-6);

    // set preconditioner (identity)
    cusp::identity_operator<float, cusp::device_memory> M(A.num_rows, A.num_rows);
	//cusp::precond::scaled_bridson_ainv<float, cusp::device_memory> M(A, .1);
	//cusp::precond::smoothed_aggregation<int, float, cusp::device_memory> M(A);

    // solve the linear system A * x = b with the Conjugate Gradient method
   // cusp::krylov::cg(A, x_, b_, monitor, M);
	cusp::krylov::bicgstab(A, x_, b_, monitor, M);
	
	monitor.print();


	//looks frigging inefficient
	for(unsigned int i = 0; i < x.size(); i++){
		x[i] = x_[i];
	}

	//cusp::print(b_);
}


void CUDASOLVER_EXPORT mySolver::checkMatrix( cpuCSRMatrix & mat )
{

}


int  CUDASOLVER_EXPORT mySolver::main()
{

	int cuda_major =  CUDA_VERSION / 1000;
    int cuda_minor = (CUDA_VERSION % 1000) / 10;

    int thrust_major = THRUST_MAJOR_VERSION;
    int thrust_minor = THRUST_MINOR_VERSION;

    int cusp_major = CUSP_MAJOR_VERSION;
    int cusp_minor = CUSP_MINOR_VERSION;

    std::cout << "CUDA   v" << cuda_major   << "." << cuda_minor   << std::endl;
    std::cout << "Thrust v" << thrust_major << "." << thrust_minor << std::endl;
	std::cout << "Cusp   v" << cusp_major   << "." << cusp_minor   << std::endl;

	cusp::csr_matrix<int,float,cusp::host_memory> A(4,3,6);

    const int arraySize = 5;
    const int a[arraySize] = { 1, 2, 3, 4, 5 };
    const int b[arraySize] = { 10, 20, 30, 40, 50 };
    int c[arraySize] = { 0 };
	

    // Add vectors in parallel.
    cudaError_t cudaStatus = addWithCuda_(c, a, b, arraySize);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addWithCuda failed!");
        return 1;
    }

    printf("{1,2,3,4,5} + {10,20,30,40,50} = {%d,%d,%d,%d,%d}\n",
        c[0], c[1], c[2], c[3], c[4]);

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

	std::cout << "Now, enter a number and press Enter\n";
	int anything;
	std::cin >> anything;

    return 0;
}

__global__ void addKernel_(int *c, const int *a, const int *b)
{
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}


// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCuda_(int *c, const int *a, const int *b, size_t size)
{
    int *dev_a = 0;
    int *dev_b = 0;
    int *dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    addKernel_<<<1, size>>>(dev_c, dev_a, dev_b);

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
    cudaFree(dev_c);
    cudaFree(dev_a);
    cudaFree(dev_b);
    
    return cudaStatus;
}
