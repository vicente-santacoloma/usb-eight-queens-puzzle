#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define FALSE 0
#define TRUE 1

int numHilos; 
int matrix[64][16];
int solucion [64];
float tiempo[64];
float elapse = 0.0;
FILE *fp;

double timeval_diff(struct timeval *a, struct timeval *b) {
  
    return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
  
void pantallaH () {
  
  int i,j;
  
  for(i = 1; i<=numHilos; i++) {
    
     printf("Resultado del hilo %d: \n",i);
     printf("\tSolucion: ");
     
     if (solucion[i-1]) {
    
       for (j=0; j<16; j++) {
	 printf("(%d,%d) ", matrix[i-1][j],matrix[i-1][j+1]);
	 j++;
       }
     }
     else {
	printf("No se encontro");
     }
   
     printf("\n\t\tTiempo: %f mseg.\n",tiempo[i-1]);
     printf("\t\tTablero inicial: (%d,%d)\n\n",(i-1)%8,(i-1)/8);
       
  }

}

void pantallaP () {
  
  char c;
  fp = fopen("HiloPrincipal","r");
  
  while((fscanf(fp,"%c",&c))!=EOF)
    printf("%c",c); 
    
  fclose(fp);
  
}

void escribirM (long Hilo, int x[8], int y[8]) {
  
  int j;
  
  for (j=0; j<8; j++) {
    
    matrix[Hilo-1][2*j] = y[j];
    matrix[Hilo-1][(2*j)+1] = x[j];
    
  }
  
  tiempo[Hilo-1] = elapse*1000;
  
}

void escribirP (int count [numHilos]) {
  
  int i, j;
  int k = 0;
  int aux = 1;
  int numSol = 0;
  
  for(i = 0; i<numHilos; i++) {
    
    if(solucion[i]){
      if (count[i]>1)
	numSol++;
      else
	numSol = numSol + count[i];
    }
  }
  
  fp = fopen("HiloPrincipal","w");
  
  fprintf(fp,"Nro. Total de soluciones diferentes: %d \n",numSol);
  
  for (i = 0; i<numHilos; i++) {
    
    if(solucion[i]) {
      if (matrix[i][0] != -1) {

	fprintf(fp,"\tSolucion %d: ",aux);
	aux++;
      
	for (j = 0; j<16; j++) { 
	  
	  fprintf(fp,"(%d,%d) ",matrix[i][j],matrix[i][j+1]);
	  j++;
	}
	fprintf(fp,"\n\t\tTiempo minimo: %f mseg.\n",tiempo[i]);
	fprintf(fp,"\t\tNro. de veces encontrada: %d\n",count[i]);

      }
    }
  }
  
  fclose(fp);
  pantallaP();
  
}

void check () {
 
  int i, j, k, aux, tmp1, tmp2;
  int count  [numHilos];
  
  for (i = 0; i<numHilos; i++)
    count[i] = 1;
 
  for (i = 0; i<numHilos; i++) {
   
    for (k = i+1; k<numHilos; k++) {
    
      aux = 1;
    
      for (j = 0; j<16; j++) {
      
	tmp1 = matrix[i][j];
	tmp2 = matrix[k][j];
      
	if (! (tmp1==tmp2) || (tmp1==-1) || (tmp2==-1)) {
	  j = 16;
	  aux = 0;
	}
      }
    
      if (aux) {
      
	matrix[k][0] = -1;
	count[i]++;
	count[k] = 0;
	
	if (tiempo[i]>tiempo[k]) {
	  tiempo[i] = tiempo[k];
	  tiempo[k] = -1.0;
	
	}
      }
    }
  }
  
  escribirP(count);
  
}

void result (long Hilo, int x[8], int y[8], struct timeval start) {
  
  solucion[Hilo-1] = 1;
  struct timeval end;
  gettimeofday(&end,NULL);
  elapse = timeval_diff(&end,&start);
  escribirM(Hilo, x, y);
  pthread_exit((void*)Hilo);
  
}

void queen (int i, long Hilo, int a[8], int b[15], int c[15], int d[8], int x[8], int y[8], struct timeval start) {

  int j;

  for (j=0; j<8; j++) {
    
    if (!d[i]) {
      
      j = 8;
      queen(i+1, Hilo, a, b, c, d, x, y,start);
      if (i==7) 
       	result(Hilo, x, y,start);
      
    }
    else if (a[j] && b[i+j] && c[i-j+7]) {

      x[i] = j; //Columna de la reina i
      y[i] = i; //Fila de la reina i
      a[j] = FALSE; //row
      b[i+j] = FALSE; //slash
      c[i-j+7] = FALSE; //backslash
      
      if (i<7) 
	queen(i+1, Hilo, a, b, c, d, x, y,start);
      else 
	result(Hilo, x, y,start);
   
      if (d[i]) {
      a[j] = TRUE;
      b[i+j] =TRUE;
      c[i-j+7] = TRUE;
      }
    }
  }
}

void Tablero (long t, int a[8], int b[15], int c[15], int d[8]) {

  int x [8];
  int y [8];
  long i;
  long Hilo;
  struct timeval start;
  Hilo = t;
  i = t;
  
  a[(i-1)/8] = FALSE;
  b[((i-1)%8)+((i-1)/8)] = FALSE;
  c[7+((i-1)%8)-((i-1)/8)] = FALSE;
  d[(i-1)%8] = FALSE;
  x[(i-1)%8] = (i-1)/8;
  y[(i-1)%8] = (i-1)%8;
  gettimeofday(&start,NULL);
  queen(0, Hilo, a, b, c, d, x, y,start);
  
}

void *inic (void *threadid) {
  
  int a [8];
  int b [15];
  int c [15];
  int d [8];
  long t;
  t = (long)threadid;
  int k;

  for (k=0; k<8; k++) {
    a[k] = TRUE;
    d[k] = TRUE;
  }
  for (k=0;k<15; k++) { 
    b[k] = TRUE;
    c[k] = TRUE;
  }

  Tablero(t, a, b, c, d);
  
}

int exceptionHilo (int i) {
    
  int k = i;
  

  while(k>64 || k<=0) {
      printf("Numero invalido de hilos, reintroduzca: ");
      scanf("%d", &k);
  }
  
  return k;
  
}

int exceptionOpcion (int i) {
    
  int k = i;
  
  while (k!=0 && k!=1) {
    printf("Opcion invalida, reintroduzca: ");
    scanf("%d", &k);  
  }
  
  return k;
  
}

int main(int argc, char *argv[]) {
  
  numHilos = atoi(argv[1]);
  numHilos = numHilos*-1;
  int opcion = atoi(argv[2]);
  opcion = opcion*-1;
  numHilos = exceptionHilo(numHilos);
  opcion = exceptionOpcion(opcion);
  pthread_t thread[numHilos];	
  pthread_attr_t attr;
  void *status;
  int i, rc;
  int count [numHilos];
 
   for (i = 0; i<64; i++)
     solucion[i] = 0;
  
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
  for (i=1;i<=numHilos;i++){

    rc = pthread_create(&thread[i], &attr, inic, (void *)i);
    if (rc) {
      printf("Fallo en la creacion del hilo: %d",i);
      exit(-1);
    }
    
  } 

  pthread_attr_destroy(&attr);
  
  for (i=1;i<=numHilos;i++){
    
    rc = pthread_join(thread[i], &status);
    if (rc) {
      printf("Error en el retorno del codigo");
      exit(-1);
    }

  }
  
  if (opcion)
    pantallaH();
  
  check();
  
}