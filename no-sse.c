// your C program without sse instructions

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define N 1000
#define M 1000
#define P 9

void get_walltime(double *wct){
	struct timeval tp;
	gettimeofday(&tp, NULL);
	*wct = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}
int max(int k, int x) {
  return (k >= x) ? k : x;
}

int min(int k, int x) {
  return (k <= x) ? k : x;
}

int main(){

	float *a,*b,*c;
	double ts,te,mflops;
	float sum;
	int i, j, constant_count;
	
a = (float*) malloc(N* M* sizeof(float)); //pinakas pixel
if (a == NULL) {
    printf("Error!\n");
    exit(1);
  }
b = (float*) malloc(P* sizeof(float));
if ( b == NULL) {
    printf("Error!\n");
    free(a);
    exit(1);
  }
c = (float *)malloc(N* M* sizeof(float)); 
  if (c == NULL) {
    printf("Error!\n");
    free(a);
    free(b);
    exit(1);
  }

for (i = 0; i < N * M; i++) {
     a[i] = (float)i*3.0;
     c[i] = -1.0;
  } 
for(i = 0; i < P; i++) {
    if(i == 4) {
      b[i] = 5.0;
    }
    else {
      b[i] = 0.5;
    }
  }
get_walltime(&ts); //ksekinaei i metrisi xronou

for(i=0; i<N - 1; i++) {
	for(j=0; j<M - 1; j++) {
		constant_count = 0;
		sum = 0.0;
		for(int y = max(0, i-1); y <= min(i+1, N); y++){
        for(int z = max(0, j-1); z <= min(j+1, M); z++){
          sum += (*(a + (y * M)) + z) * (*(b + constant_count));
          constant_count++;
	   	 }
		}
	  
	  *(c + (i * M) + j) = sum;
	}
}
get_walltime(&te); //stamataei i metrisi
printf("Xronos %f\n", te-ts); //diafora xronou te-ts

mflops = (N* M* 2.0) / ((te - ts) * 1e6);	
printf("MFLOPS/sec = %f\n",mflops);

free(a); 
free(b); 
free(c);
return 0;
}
