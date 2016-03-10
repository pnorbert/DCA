//-*-C++-*-

#ifndef MAGMA_GETRS_PLAN_H
#define MAGMA_GETRS_PLAN_H

namespace CUDA_GPU
{
  /*!
   *
   *  \author R. Solca, Peter Staar
   */
  template<typename scalartype>
  void magma_getrs_gpu_t(char& TRANS, int& N, int& NRHS, scalartype* DA, int& LDDA,
			 int* IPIV, scalartype* DB, int& LDDB, int& INFO)
  {
    throw std::logic_error(__PRETTY_FUNCTION__);
  }

  template<>
  void magma_getrs_gpu_t<float>(char& TRANS, int& N, int& NRHS, float* DA, int& LDDA,
				int* IPIV, float* DB, int& LDDB, int& INFO)
  {
    magma_sgetrs_gpu(TRANS, N, NRHS, DA, LDDA, IPIV, DB, LDDB, &INFO);
    
    if(INFO!=0)
      throw std::logic_error(__PRETTY_FUNCTION__); 
  }

  template<>
  void magma_getrs_gpu_t<double>(char& TRANS, int& N, int& NRHS, double* DA, int& LDDA,
				 int* IPIV, double* DB, int& LDDB, int& INFO)
  {
    magma_dgetrs_gpu(TRANS, N, NRHS, DA, LDDA, IPIV, DB, LDDB, &INFO);
    
    if(INFO!=0)
      throw std::logic_error(__PRETTY_FUNCTION__);
  }

  template<>
  void magma_getrs_gpu_t<cuComplex>(char& TRANS, int& N, int& NRHS, cuComplex* DA, int& LDDA,
				    int* IPIV, cuComplex* DB, int& LDDB, int& INFO)
  {
    magma_cgetrs_gpu(TRANS, N, NRHS, DA, LDDA, IPIV, DB, LDDB, &INFO);
    
    if(INFO!=0)
      throw std::logic_error(__PRETTY_FUNCTION__);
  }

  template<>
  void magma_getrs_gpu_t<cuDoubleComplex>(char& TRANS, int& N, int& NRHS, cuDoubleComplex* DA, int& LDDA,
					  int* IPIV, cuDoubleComplex* DB, int& LDDB, int& INFO)
  {
    magma_zgetrs_gpu(TRANS, N, NRHS, DA, LDDA, IPIV, DB, LDDB, &INFO);
    
    if(INFO!=0)
      throw std::logic_error(__PRETTY_FUNCTION__); 
  }

  template<typename scalartype>
  void magma_getrs(char& TRANS, int& N, int& NRHS, scalartype* Matrix_A, int& LDA,
		   int* IPIV, scalartype* Matrix_B, int& LDB, int& INFO)
  {
    //std::cout << "magma_getrs" << std::endl;
    
    typedef typename GET_CUBLAS_TYPE<scalartype>::type cuda_scalartype;
    
    cuda_scalartype *DA = NULL;
    cuda_scalartype *DB = NULL;
    
    int LDDA=LDA;
    int LDDB=LDB;

    allocate_gpu(&DA, LDDA, N);
    allocate_gpu(&DB, LDDB, NRHS);
    
    cublasSetMatrix(N, N, sizeof(cuda_scalartype), Matrix_A, LDA, DA, LDDA);
    cublasSetMatrix(N, NRHS, sizeof(cuda_scalartype), Matrix_B, LDB, DB, LDDB);
    
    magma_getrs_gpu_t(TRANS, N, NRHS, DA, LDDA, IPIV, DB, LDDB, INFO);
    
    cublasGetMatrix(N, NRHS, sizeof(cuda_scalartype), DB, LDDB, Matrix_B, LDB);
    
    deallocate_gpu(DA);
    deallocate_gpu(DB);   
  }
}

#endif