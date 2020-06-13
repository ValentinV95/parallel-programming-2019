#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <chrono> 
#include <string>
#include <omp.h>

typedef double data_t;

// Ax=b, invert A to find x
// Algorithm Steps:
// Augment matrix A with equal dimension identity matrix
// Pivot matrix rows as needed given leading zeros
// Divide each row by the leading value
// Subtract from each column j the entry in ij from all rows not i
// Return inverted matrix from the augmented side



// Helper structs:
struct SPCOORD_MAT_STRUCT {
  int M = 0;
  int N = 0;
  int size = 0;
  int * i = NULL;
  int * j = NULL;
  data_t * v = NULL;
} ;

struct DN_MAT_STRUCT {
  int size_n = 0;
  int size_m = 0;
  data_t * A = NULL;
} ;

//Prototypes:
void pprint_mat(data_t * A, int size_n, int size_m);
SPCOORD_MAT_STRUCT * read_MMmatrix(std::string fname);

// Helper functions:
void usage(char ** argv){
  std::cerr << "USAGE: " << argv[0] << " matrix_file" << std::endl;
  exit( EXIT_FAILURE );
}


// Read a matrix in the matrix market format:
SPCOORD_MAT_STRUCT * read_MMmatrix(std::string fname)
{
  std::cout << "Reading " << fname << std::endl;
  std::string line_buffer;
  std::ifstream MMf(fname.c_str(), std::ifstream::in);
  int M, N, L, i, j, n, mode = 0;
  data_t v;
  SPCOORD_MAT_STRUCT * matrix = new SPCOORD_MAT_STRUCT;
  while ( !MMf.eof() )
  {
    std::getline( MMf, line_buffer );
    //std::cout << line_buffer << std::endl;
    if ( line_buffer.size() == 0 || line_buffer[0] == '%' ) continue;
    // Read the Matrix Dimensions and Number of lines to read:
    if ( mode == 0)
    {
        n = sscanf( line_buffer.c_str(), "%d %d %d", &M, &N, &L);
        if ( n <= 0 ){
          std::cerr << "ERROR: Could not read matrix dimensions from " << fname << std::endl;
        } else {
          std::cout << "Preparing to read matrix of dimensions " << M << " x " << N << std::endl;
          matrix->size = 0;
          matrix->M = M;
          matrix->N = N;
          matrix->i = new int[L];
          matrix->j = new int[L];
          matrix->v = new data_t[L];
          mode = 1;
        }
    } else {
      n = sscanf( line_buffer.c_str(), "%d %d %lf", &i, &j, &v);
      if ( n <= 0 ) continue;
      // Note: Matrix market formats are 1-indexed
      matrix->i[matrix->size] = i-1;
      matrix->j[matrix->size] = j-1;
      matrix->v[matrix->size] = v;
      matrix->size ++;
      if ( matrix->size > L )
      {
        matrix->size --;
        break;
      }
    }
  }
  MMf.close();
  std::cout << "Reached end of MM file." << std::endl;
  if ( matrix->size == 0 )
  {
    delete [] matrix->i;
    delete [] matrix->j;
    delete [] matrix->v;
    delete [] matrix;
    matrix = NULL;
  }
  return matrix;
}

// Convert sparse coordinate format to dense format
void spcoord_to_dense(SPCOORD_MAT_STRUCT * spcoord_mat, DN_MAT_STRUCT *& dn_mat)
{
  if (spcoord_mat == NULL)
  {
    return;
  } else if ( dn_mat == NULL )
  {
    dn_mat = new DN_MAT_STRUCT;
  }
  if (dn_mat->size_n > 0 && dn_mat->size_m > 0 || dn_mat->A != NULL)
  {
    delete [] dn_mat->A;
    dn_mat->A = NULL;
    dn_mat->size_n = 0;
    dn_mat->size_m = 0;
  }
  dn_mat->size_n = spcoord_mat->M;
  dn_mat->size_m = spcoord_mat->N;
  dn_mat->A = new data_t[dn_mat->size_n*dn_mat->size_m];
  for (int i = 0; i < spcoord_mat->size; i++)
  {
    int row = spcoord_mat->i[i];
    int col = spcoord_mat->j[i];
    dn_mat->A[ row * dn_mat->size_n + col ] = spcoord_mat->v[i];
  }
}

// Create identity matrix
data_t *  identity_dn(int size)
{
  data_t * I = new data_t[size*size]();  // Note parens cause 0-initialization
  for (int i = 0; i < size; i++)
  {
    I[ i*size + i ] = 1.0;
  }
  return I;
}

// Flat matrix printer
void pprint_mat(data_t * A, int size_n, int size_m)
{
  if (A == NULL) return;
  for ( int i = 0; i < size_n; i++ )
  {
    for ( int j = 0; j < size_m; j++ )
    {
      std::cout << *(A + (i*size_n + j )) << " ";
    }
    std::cout << std::endl;
  }
}

// Dense matrix struct printer
void pprint_dn(DN_MAT_STRUCT * dn_mat)
{
  if ( dn_mat == NULL ) return;
  pprint_mat(dn_mat->A, dn_mat->size_n, dn_mat->size_m);
}

// Parallelized Gauss-Jordan Elimintation
data_t * dn_gauss_jordan_omp_parallel(DN_MAT_STRUCT * dn_mat)
{
  omp_set_num_threads(8);
  if (dn_mat == NULL) return NULL;
  if (dn_mat->size_n != dn_mat->size_m || dn_mat->size_n == 0 || dn_mat->size_m == 0)
  {
    std::cerr << "ERROR: Wrong matrix dimensions to perform GJE. M: " << dn_mat->size_m << 
      " N: " << dn_mat->size_n << std::endl;
    return NULL;
  }
  int sz = dn_mat->size_n;
  data_t * I = identity_dn(sz);
  data_t * A = dn_mat->A;
  if ( I == NULL )
  {
    std::cerr << "ERROR: NULL Identity matrix!" << std::endl;
    return NULL;
  }
  // For each row:
  for (int i = 0; i < sz; i++)
  {
    // Normalize all rows by their diagonal entries
    // (Do the same to the ident matrix)
    data_t diag_inv = 1/A[ i*sz + i ];
    
    // iterate over columns
    #pragma omp parallel for simd
    for (int j = 0; j < sz; j++) 
    {
      I[ i*sz + j ] *= diag_inv;
      A[ i*sz + j ] *= diag_inv;
    }
    // Subtract current row from all other rows to cancel the leading entries
    #pragma omp parallel for //collapse(2)
    for ( int k = 0; k < sz-1; k++ )
    {
      int k_p = k;
      if ( k >= i) k_p ++;
      data_t ref_scale = A[ k_p*sz + i ];
      // Iterate over columns
      for (int j = 0; j < sz; j++ )
      {
        if ( i == j ) 
        {
          A[ k_p*sz + j ] = 0; 
        }
          else
        {
          A[ k_p*sz + j ] -= ref_scale * A[ i*sz + j ];
        }
        I[ k_p*sz + j ] -= ref_scale * I[ i*sz + j ];
      }
    }
  }
  return I;
}

// Minimal single-threaded working example of matrix inverse
data_t * dn_gauss_jordan(DN_MAT_STRUCT * dn_mat)
{
  if (dn_mat == NULL) return NULL;
  if (dn_mat->size_n != dn_mat->size_m || dn_mat->size_n == 0 || dn_mat->size_m == 0)
  {
    std::cerr << "ERROR: Wrong matrix dimensions to perform GJE. M: " << dn_mat->size_m << 
      " N: " << dn_mat->size_n << std::endl;
    return NULL;
  }
  int sz = dn_mat->size_n;
  data_t * I = identity_dn(sz);
  data_t * A = dn_mat->A;
  if ( I == NULL )
  {
    std::cerr << "ERROR: NULL Identity matrix!" << std::endl;
    return NULL;
  }
  // For each row:
  for (int i = 0; i < sz; i++)
  {
    // Normalize all rows by their diagonal entries
    // (Do the same to the ident matrix)
    data_t diag_inv = 1/A[ i*sz + i ];
    
    // iterate over columns
    for (int j = 0; j < sz; j++) 
    {
      I[ i*sz + j ] *= diag_inv;
      A[ i*sz + j ] *= diag_inv;
    }
    // Subtract current row from all other rows to cancel the leading entries
    for ( int k = 0; k < sz; k++ )
    {
      if ( k == i ) continue;
      // Iterate over columns
      data_t ref_scale = A[ k*sz + i ];
      for (int j = 0; j < sz; j++ )
      {
        A[ k*sz + j ] -= ref_scale * A[ i*sz + j ];
        I[ k*sz + j ] -= ref_scale * I[ i*sz + j ];
      }
    }
  }
  return I;
}

bool compare(data_t * A, data_t * B)
{
  data_t eps = 0.000005;
  for (int i = 0; i < 5; i++){
    for (int j = 0; j < 5; j++)
    {
      if (abs(A[ i*5+j ] - B[ i*5+j ]) > eps) return false;
    }
  }
  return true;
}

bool test()
{
// Read in the test matrix with known inverse
  SPCOORD_MAT_STRUCT * test_mat_coord = read_MMmatrix("5x5Dense.mtx");
  DN_MAT_STRUCT * test_mat_dn = NULL;
  spcoord_to_dense(test_mat_coord, test_mat_dn);
  pprint_dn(test_mat_dn); 
  std::cout << "Running serial matrix inversion." << std::endl;
  data_t * test_inv = dn_gauss_jordan(test_mat_dn);
  data_t correct_inv[] = {
        754.45, -407.733, -582.307, -900.553, 1369.93, 
        -1657.4, 1012.08, 1062.57, 2765.84, -3614.02, 
        -573.809, 257.713, 390.592, 1384.68, -1603.1, 
        218.047, -142.344, -105.894, -655.758, 834.627, 
        980.81, -461.333, -532.63, -2003.07, 2298.57
      };
  if (!compare(correct_inv, test_inv)) 
  {
    std::cout << "Single threaded test failed." << std::endl;
    return false;
  }
  std::cout << "Single threaded matrix inversion code succeeded on 5x5 matrix." << std::endl;
 // Now test parallel OMP code 
  std::cout << "Running parallel matrix inversion." << std::endl;
  delete [] test_inv;
  test_inv = NULL;
  spcoord_to_dense(test_mat_coord, test_mat_dn);
  pprint_dn(test_mat_dn); 
  test_inv = dn_gauss_jordan_omp_parallel( test_mat_dn );
  pprint_mat(test_inv, 5, 5); 
  if (!compare(correct_inv, test_inv)) 
  {
    std::cout << "OMP test failed." << std::endl;
    return false;
  }

  std::cout << "Test completed successfully." << std::endl;
  return true;
}

int main(int argc, char ** argv)
{
  clock_t t;
#ifdef TEST
  if (!test())
  {
    std::cerr << "ERROR: Failed test with known 5x5 matrix!" << std::endl;
    return 1;
  }
  std::cout << "Successfully inverted known 5x5 matrix." << std::endl;
#else
  // Read in the selected matrix file:
  std::string fname("");
  if (argc > 1)
  {
    fname = std::string( argv[1] );
  } else {
    usage(argv);
  }
  // Read matrix:
  SPCOORD_MAT_STRUCT * mat_ptr = NULL;
  mat_ptr = read_MMmatrix(fname);
  if ( mat_ptr == NULL ) {
    std::cerr << "Matrix is NULL." << std::endl;
  } else {
    std::cout << "Read " << mat_ptr->size << " values" << std::endl;
  }

  DN_MAT_STRUCT * dn_mat = NULL;
  spcoord_to_dense(mat_ptr, dn_mat);
  std::cout << "Converted matrix to dense format." << std::endl;
  int spcoord_size = mat_ptr->size * ( sizeof(int) * 2 + sizeof(data_t) ) + sizeof(mat_ptr);
  int dn_size = dn_mat->size_m*dn_mat->size_n * (sizeof(data_t)) + sizeof(dn_mat);
  std::cout << "Size of spcoord matrix representation: " << spcoord_size << std::endl;
  std::cout << "Size of dense represenation: " << dn_size << std::endl;
 
  if (dn_mat->size_n <= 10 && dn_mat->size_m <= 10)
  {
    std::cout << "Original matrix: " << std::endl;
    pprint_dn(dn_mat);
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  data_t * I = dn_gauss_jordan(dn_mat);
  auto t2 = std::chrono::high_resolution_clock::now();
  if (dn_mat->size_n <= 10 && dn_mat->size_m <= 10)
  {
    std::cout << "Modified matrix: " << std::endl;
    pprint_dn(dn_mat);
  }
  if (dn_mat->size_n <= 10 && dn_mat->size_m <= 10)
  {
    std::cout << "Inverse matrix: " << std::endl;
    pprint_mat(I, dn_mat->size_m, dn_mat->size_n);
  }
  float seconds = (float)std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()/1000; 
  std::cout << "Completed serial matrix inversion in " << seconds << " seconds." << std::endl;
  delete [] I;
  std::cout << "Running parallel inversion with " << omp_get_num_threads() << " threads." << std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  I = dn_gauss_jordan_omp_parallel(dn_mat);
  t2 = std::chrono::high_resolution_clock::now();
  seconds = (float)std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()/1000; 
  std::cout << "Completed parallel matrix inversion in " << seconds << " seconds." << std::endl;
#endif
  return 0;
}