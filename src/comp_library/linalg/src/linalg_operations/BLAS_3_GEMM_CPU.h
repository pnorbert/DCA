//-*-C++-*-

#ifndef LINALG_GEMM_CPU_H
#define LINALG_GEMM_CPU_H

namespace LIN_ALG {

  template<>
  class GEMM<CPU>
  {
  public:

    template<typename scalartype>
    static void execute(matrix<scalartype, CPU>& A, matrix<scalartype, CPU>& B, matrix<scalartype, CPU>& C)
    {
      char TRANSA = 'N';
      char TRANSB = 'N';

      assert(test_sizes(TRANSA, TRANSB, A, B, C));

      scalartype alpha = 1.;
      scalartype beta  = 0.;

      int M = A.get_current_size().first;
      int K = A.get_current_size().second;
      int N = B.get_current_size().second;

      int LDA = A.get_global_size().first;
      int LDB = B.get_global_size().first;
      int LDC = C.get_global_size().first;

      execute(TRANSA, TRANSB, M, N, K, alpha, A.get_ptr(), LDA, B.get_ptr(), LDB, beta, C.get_ptr(), LDC);
    }

    template<typename scalartype>
    static void execute(scalartype a, matrix<scalartype, CPU>& A, matrix<scalartype, CPU>& B,
                        scalartype b, matrix<scalartype, CPU>& C,
                        int /*thread_id=0*/, int /*stream_id=0*/)
    {
      char TRANSA = 'N';
      char TRANSB = 'N';

      assert(test_sizes(TRANSA, TRANSB, A, B, C));

      scalartype alpha = a;
      scalartype beta  = b;

      int M = A.get_current_size().first;
      int K = A.get_current_size().second;
      int N = B.get_current_size().second;

      int LDA = A.get_global_size().first;
      int LDB = B.get_global_size().first;
      int LDC = C.get_global_size().first;

      execute(TRANSA, TRANSB, M, N, K, alpha, A.get_ptr(), LDA, B.get_ptr(), LDB, beta, C.get_ptr(), LDC);
    }

    template<typename scalartype>
    static void execute(char TRANSA, char TRANSB,
                        matrix<scalartype, CPU>& A,
                        matrix<scalartype, CPU>& B,
                        matrix<scalartype, CPU>& C)
    {
      assert(test_sizes(TRANSA, TRANSB, A, B, C));

      execute(TRANSA, TRANSB, A, B, C, 0, 0);
    }

    template<typename scalartype>
    static void execute(char TRANSA, char TRANSB,
                        matrix<             scalartype , CPU>& A,
                        matrix<std::complex<scalartype>, CPU>& B,
                        matrix<std::complex<scalartype>, CPU>& C)
    {
      assert(test_sizes(TRANSA, TRANSB, A, B, C));

      matrix<scalartype, CPU> B_re("B_re", B.get_current_size(), B.get_global_size());
      matrix<scalartype, CPU> B_im("B_im", B.get_current_size(), B.get_global_size());

      for(int j=0; j<B.get_current_size().second; j++)
        for(int i=0; i<B.get_current_size().first; i++)
          B_re(i,j) = real(B(i,j));

      for(int j=0; j<B.get_current_size().second; j++)
        for(int i=0; i<B.get_current_size().first; i++)
          B_im(i,j) = imag(B(i,j));

      matrix<scalartype, CPU> C_re("C_re", C.get_current_size(), C.get_global_size());
      matrix<scalartype, CPU> C_im("C_im", C.get_current_size(), C.get_global_size());

      execute(TRANSA, TRANSB, A, B_re, C_re, 0, 0);
      execute(TRANSA, TRANSB, A, B_im, C_im, 0, 0);

      std::complex<scalartype> I(0,1);
      for(int j=0; j<C.get_current_size().second; j++)
        for(int i=0; i<C.get_current_size().first; i++)
          C(i,j) = C_re(i,j)+I*C_im(i,j);
    }

    //template<typename scalartype>
    static void execute(char TRANSA, char TRANSB,
                        matrix<std::complex<double>, CPU>& A,
                        matrix<             double , CPU>& B,
                        matrix<std::complex<double>, CPU>& C)
    {
      assert(test_sizes(TRANSA, TRANSB, A, B, C));

      matrix<double, CPU> A_re("A_re", A.get_current_size(), A.get_global_size());
      matrix<double, CPU> A_im("A_im", A.get_current_size(), A.get_global_size());

      for(int j=0; j<A.get_current_size().second; j++)
        for(int i=0; i<A.get_current_size().first; i++)
          A_re(i,j) = real(A(i,j));

      for(int j=0; j<A.get_current_size().second; j++)
        for(int i=0; i<A.get_current_size().first; i++)
          A_im(i,j) = imag(A(i,j));

      matrix<double, CPU> C_re("C_re", C.get_current_size(), C.get_global_size());
      matrix<double, CPU> C_im("C_im", C.get_current_size(), C.get_global_size());

      execute(TRANSA, TRANSB, A_re, B, C_re, 0, 0);
      execute(TRANSA, TRANSB, A_im, B, C_im, 0, 0);

      std::complex<double> I(0,1);
      for(int j=0; j<C.get_current_size().second; j++)
        for(int i=0; i<C.get_current_size().first; i++)
          C(i,j) = C_re(i,j)+I*C_im(i,j);
    }

    template<typename scalartype>
    static void execute(char TRANSA, char TRANSB,
                        matrix<scalartype, CPU>& A,
                        matrix<scalartype, CPU>& B,
                        matrix<scalartype, CPU>& C,
                        int /*thread_id*/, int /*stream_id*/)
    {
      assert(test_sizes(TRANSA, TRANSB, A, B, C));

      scalartype alpha = 1;
      scalartype beta  = 0;

      int M, K, N;
      {
        M = C.get_current_size().first;

        if(TRANSA=='N')
          K = A.get_current_size().second;
        else
          K = A.get_current_size().first;

        N = C.get_current_size().second;
      }

      int LDA = A.get_global_size().first;
      int LDB = B.get_global_size().first;
      int LDC = C.get_global_size().first;

      execute(TRANSA, TRANSB, M, N, K, alpha, A.get_ptr(), LDA, B.get_ptr(), LDB, beta, C.get_ptr(), LDC);
    }

    template<typename scalartype>
    static void execute(char TRANSA, char TRANSB,
                        scalartype a, matrix<scalartype, CPU>& A, matrix<scalartype, CPU>& B,
                        scalartype b, matrix<scalartype, CPU>& C,
                        int /*thread_id=0*/, int /*stream_id=0*/)
    {
      assert(A.get_current_size().first  == C.get_current_size().first);
      assert(A.get_current_size().second == B.get_current_size().first);
      assert(B.get_current_size().second == C.get_current_size().second);

      scalartype alpha = a;
      scalartype beta  = b;

      int M, K, N;
      {
        M = C.get_current_size().first;

        if(TRANSA=='N')
          K = A.get_current_size().second;
        else
          K = A.get_current_size().first;

        N = C.get_current_size().second;
      }

      int LDA = A.get_global_size().first;
      int LDB = B.get_global_size().first;
      int LDC = C.get_global_size().first;

      execute(TRANSA, TRANSB, M, N, K, alpha, A.get_ptr(), LDA, B.get_ptr(), LDB, beta, C.get_ptr(), LDC);
    }


    template<typename scalartype>
    inline static void execute(char TRANSA, char TRANSB, int M, int N, int K,
                               scalartype alpha,
                               scalartype* A, int LDA,
                               scalartype* B, int LDB,
                               scalartype  beta,
                               scalartype* C, int LDC,
                               int /*thread_id*/=0, int /*stream_id*/=0)
    {
      dca::linalg::gemm(&TRANSA, &TRANSB, M, N, K, alpha, A, LDA, B, LDB, beta, C, LDC);
    }

  private:

    template<typename scalartype_1, typename scalartype_2, typename scalartype_3>
    inline static bool test_sizes(char TRANSA, char TRANSB,
                                  matrix<scalartype_1, CPU>& A,
                                  matrix<scalartype_2, CPU>& B,
                                  matrix<scalartype_3, CPU>& C)
    {
      if(TRANSA=='N')
      {
        if(TRANSB=='N')
        {
          return ((A.get_current_size().first == C.get_current_size().first) &&
                  (A.get_current_size().second == B.get_current_size().first) &&
                  (B.get_current_size().second == C.get_current_size().second));
        }
        else
        {
          return ((A.get_current_size().first == C.get_current_size().first) &&
                  (A.get_current_size().second == B.get_current_size().second) &&
                  (B.get_current_size().first == C.get_current_size().second));
        }
      }
      else
      {
        if(TRANSB=='N')
        {
          return ((A.get_current_size().second == C.get_current_size().first) &&
                  (A.get_current_size().first == B.get_current_size().first) &&
                  (B.get_current_size().second == C.get_current_size().second));
        }
        else
        {
          return ((A.get_current_size().second == C.get_current_size().first) &&
                  (A.get_current_size().first == B.get_current_size().second) &&
                  (B.get_current_size().first == C.get_current_size().second));
        }
      }
      return false;
    }

  };

}

#endif
