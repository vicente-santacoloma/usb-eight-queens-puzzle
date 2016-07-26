#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define FALSE 0
#define TRUE 1

int a [8];
int b [15];
int c [15];
int d [8];
int x [8];
int y [8];
int numHijos, Hijo;
int solucion = 0;
struct timeval start,end;
float elapse = 0.0;
FILE *fp;
  
void inic () {
  
  int k;

  for (k=0; k<8; k++) { 
    a[k] = TRUE;
    d[k] = TRUE;
  }
  
  for (k=0;k<15; k++) {  
    b[k] = TRUE;
    c[k] = TRUE;  
  }
  
}

double timeval_diff(struct timeval *a, struct timeval *b) {
  
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
    
}

void pantallaH (int matrix [numHijos][16], float tiempo [numHijos]) {
 
  int i,j;
  
  for(i = 1; i<=numHijos; i++) {
    
     printf("Resultado del hilo %d: \n",i);
     printf("\tSolucion: ");
     
     if (matrix[i-1][0] != matrix[i-1][2] ) {
    
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
  fp = fopen("ProcesoPrincipal","r");
  
  while((fscanf(fp,"%c",&c))!=EOF)
    printf("%c",c); 
    
  fclose(fp);
  
}

void escribirH () {
  
  int k;
  
  for(k=0;k<8;k++)
    fprintf(fp,"%d %d ",y[k],x[k]);
  
  fprintf(fp,"%f\n",elapse*1000.0);
  
  fclose(fp);
  exit(Hijo);
  
}

void escribirP (int matrix [numHijos][16], float tiempo [numHijos], int count [numHijos]) {
  
  int i, j;
  int k = 0;
  int aux = 1;
  int numSol = 0;
  
  for(i = 0; i<numHijos; i++) {
    
    if (matrix[i][0] != matrix[i][2]) {
      if (count[i]>1)
	numSol++;
      else
	numSol = numSol + count[i];
    }
  }
  
  fp = fopen("ProcesoPrincipal","w");
  
  fprintf(fp,"Nro. Total de soluciones diferentes: %d \n",numSol);
  
  for (i = 0; i<numHijos; i++) {
    
    if (matrix[i][0] != -1 && (matrix[i][0]!=matrix[i][2])) {

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
  
  fclose(fp);
  pantallaP();
  
}

void check (int matrix [numHijos][16], float tiempo [numHijos], int count [numHijos]) {
  
  int i, j, k, aux, tmp1, tmp2;
 
  for (i = 0; i<numHijos; i++) {
   
    for (k = i+1; k<numHijos; k++) {
    
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
  
  escribirP(matrix,tiempo,count); 
  
}

void result () {
  
  solucion = 1;
  gettimeofday(&end,NULL);
  elapse = timeval_diff(&end,&start);
  escribirH();
  
}

void queen (int i) {

  int j;

  for (j=0; j<8; j++) {
    
    if (!d[i]) {
      
      j = 8;
      queen(i+1);
      if (i==7) 
	result(); 
    }
    else if (a[j] && b[i+j] && c[i-j+7]) {
      
      x[i] = j;
      y[i] = i;
      a[j] = FALSE; //row
      b[i+j] = FALSE; //slash
      c[i-j+7] = FALSE; //backslash
      
      if (i<7) 
	queen(i+1);
      else 
	result();
      
      if (d[i]) {
	a[j] = TRUE;
	b[i+j] =TRUE;
	c[i-j+7] = TRUE;
      }
    }
  }
  
}
  
void Tablero (int i) {
  
    a[(i-1)/8] = FALSE;
    b[((i-1)%8)+((i-1)/8)] = FALSE;
    c[7+((i-1)%8)-((i-1)/8)] = FALSE;
    d[(i-1)%8] = FALSE;
    x[(i-1)%8] = (i-1)/8;
    y[(i-1)%8] = (i-1)%8;
    char s [8] = "";
    sprintf(s,"TI_%d_%d",((i-1)%8),((i-1)/8));
    fp = fopen(s,"w");
    Hijo = i;
    gettimeofday(&start,NULL);
    queen(0);
    if (!solucion) {
     
	int j;
	for (j = 0; j<8; j++) {
	  x[j] = 0;
	  y[j] = 0;
	}
	escribirH();
    }
      
      
    fclose(fp);
    exit(i);
}

void cargarArchivo (int opcion) {
  
    int matrix [numHijos][16];
    float tiempo [numHijos];
    int count [numHijos];
    int i, j, k, fila, columna;
    float t;
    j = 0;
    k = 0;
    t = 0.0;
    fila = 0;
    columna = 0;
    
    for (i=0; i<numHijos; i++)
      count[i] = 1;
    
    for (i = 1; i<=numHijos; i++) {
    
      char s [8] = "";
      sprintf(s,"TI_%d_%d",((i-1)%8),((i-1)/8));
      fp = fopen(s,"r");
      
      while((fscanf(fp,"%d",&k))!=EOF) {
	
	if (j==15) {
	  matrix[fila][columna] = k;
	  fscanf(fp,"%f",&t);
	  tiempo[fila] = t;
	  fila++;
	  j = 0;
	  columna = 0;
	}
	else {
	  matrix[fila][columna] = k;
	  columna++;
	  j++;
	}
	
      }
      fclose(fp);
      
  }
  
  if (opcion) 
    pantallaH(matrix,tiempo);
     
  check(matrix,tiempo,count);
  
}

int exceptionProceso (int i) {
    
  int k = i;
  

  while(k>64 || k<=0) {
      printf("Numero invalido de procesos, reintroduzca: ");
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
  
  numHijos = atoi(argv[1]);
  numHijos = numHijos*-1;
  int opcion = atoi(argv[2]);
  opcion = opcion*-1;
  numHijos = exceptionProceso(numHijos);
  opcion = exceptionOpcion(opcion);
  pid_t pid[numHijos];	
  int i, status;

  inic();
  
  for (i=1;i<=numHijos;i++){
    pid[i] = fork();
    if (!pid[i]) 
      Tablero(i);	
  }

  for (i = 1; i<=numHijos; i++)
    wait(&status);
  
  cargarArchivo(opcion);
  
}
