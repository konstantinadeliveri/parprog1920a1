// your C program using sse instructions

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <emmintrin.h>
#define N 1000
#define M 1000
#define P 9

void get_walltime(double *wct) {
  struct timeval tp;
  gettimeofday(&tp,NULL);
  *wct = (double)(tp.tv_sec+tp.tv_usec/1000000.0);
}

// debug function
void print_m128_var(__m128 var) {
  float result [4];
  _mm_store_ps(result, var);

  printf("%f, %f, %f, %f\n", result[0], result[1], result[2], result[3]);
}


float horizontal_sum_ps(__m128 v) {
    __m128 shuf   = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sums   = _mm_add_ps(v, shuf);
    shuf          = _mm_movehl_ps(shuf, sums);
    sums          = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
}

int main() {
  int i, j;
  float *a, *b, *c, sum;
  double ts, te, mflops;
  __m128 *constants_vector;

 
  i = posix_memalign((void **)&a, 16, N * M * sizeof(float));
  if (i!=0) exit(1);
  i = posix_memalign((void **)&b, 16, P * sizeof(float));
  if (i!=0) { free(a); exit(1); }
  i = posix_memalign((void **)&c, 16, N * M * sizeof(float));
  if (i!=0) { free(a); free(b); exit(1); }
  
 
  for (i = 0; i < N * M; i++) {
     a[i] = (float)i*3.0;
     c[i] = -1.0;
  } 

  // Constants array initialization
  for(i = 0; i < P; i++) {
    if(i == 4) {
      b[i] = 5.0;
    }
    else {
      b[i] = 0.5;
    }
  }

  get_walltime(&ts);

  for(i = 1; i < N - 1; i++){
    for(j = 1; j < M - 1; j++){
           
      // alias the sse pointers to arrays
      sum = 0.0;
      constants_vector = (__m128 *)b;
      
      
      for(int z = 0; z < 3; z++) {
        switch(z) {
          case 0:
            sum += horizontal_sum_ps( _mm_mul_ps(*constants_vector, _mm_set_ps( *(a + ((i * M) + j - 1)), *(a + (((i - 1) * M) + j + 1)), *(a + (((i - 1) * M) + j)), *(a + (((i - 1) * M) + j - 1)))));
            constants_vector++;
            break;
          case 1: 
           sum += horizontal_sum_ps( _mm_mul_ps(*constants_vector, _mm_set_ps( *(a + (((i + 1) * M) + j)), *(a + (((i + 1) * M) + j - 1)), *(a + ((i * M) + j + 1)), *(a + ((i * M) + j)))));
            constants_vector++;
            break;
          case 2:
            sum += horizontal_sum_ps(_mm_mul_ps(*constants_vector, _mm_set_ps(0.0, 0.0, 0.0,*(a+ (((i + 1) * M) + j + 1)))));
            constants_vector++;
            break;
        } 
      }
   
      *(c + ((i * M) + j)) = sum;
    }
  }
  get_walltime(&te);

  printf("Xronos = %f sec\n",(te - ts));

 
  mflops = (N * M * 2.0) / ((te - ts) * 1e6);
  printf("Mflops/sec = %f\n", mflops);

  free(a);
  free(b);
  free(c);

  return 0;
}
