#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// gcc -pthread -fno-stack-protector main.c -o main.o
// ./main.o arq1.dat arq2.dat arq3.dat arq4.dat arq5.dat  

pthread_t outrosTIDs[5];

//struct passado como argumento na thread
struct thread_args {
    int * arr;
    int left;
    int right;
    int first;
};

//funcao passada na thread para ordenacao dos dados
void * mergeSort(void * args){

    struct thread_args *arg = args;
    struct thread_args *argl = malloc(sizeof arg);
    struct thread_args *argr = malloc(sizeof arg);

    int right = arg->right;
    int left = arg->left;
    int * arr = arg->arr;
    int aux=0,i,j;
    int meio = left +(right-left)/2 ;
    int temp[right-left];
    i = left;
    j = meio+1;
    int p = 0;
    //troca item
    
    if(right-left==1){
        if(arr[left]>arr[right]){
            aux = arr[left];
            arr[left] = arr[right];
            arr[right]= aux;
           }
    }
    
    
    if(right-left>1){
	
        argl->arr = arr;
        argl->left=left;
        argl->right=meio;
        argl->first=0;
	
        argr->arr = arr;
        argr->left=meio+1;
        argr->right=right;
        argr->first=0;

        //compara menor com menor e maior com maior
        mergeSort(argl);
        mergeSort(argr);
	
        while((i<=meio)&&(j<=right)){
            if(arr[i]<arr[j]){
                temp[p]=arr[i];
                i++;
            }
            else{
                temp[p]=arr[j];
                j++;
            }
            p++;
        }
        //copiar restante do array
        while(i<=meio){
            temp[p]=arr[i];
            i++;
            p++;
        }

        while(j<=right){
            temp[p]=arr[j];
            j++;
            p++;
        }


        for(int k=0;left <=right;left++,k++){
            arr[left] = temp[k];
        }

    }


    if(arg->first==1){
    	pthread_exit(NULL);
        free(arg);
        free(argr);
        free(argl);
    }
}

//funcao que identifica quantidade de linhas de cada arquivo
int numLinhas(char filename[]){
	int i=0;
	char ch ;
	FILE *fp;
	char caminho[] = "./arquivos/";
	strcat(caminho,filename);

	fp = fopen(caminho, "r"); // read mode

	if (fp == NULL){
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}	
	while((ch = (char)fgetc(fp)) != EOF){
		if(ch=='\n')
			i++;
	}
	
	return i;

}

//funcao que faz a leitura do arquivo e armazena no vetor total
int lerArquivo(char file_name[],int arquivo[], int line){

    char ch ;
   FILE *fp;
    int j = line;
    int i = 0;
    int k = 0;

    char caminho[] = "./arquivos/";

    strcat(caminho,file_name);

   fp = fopen(caminho, "r"); // read mode

   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }



    i=1;
   arquivo[j] = 0;
   while((ch = (char)fgetc(fp)) != EOF){
	if(ch=='\r')
	    continue;
	

        if(ch=='\n'){
            arquivo[j+1] = 0;
            j++;
	   
        }else{
            arquivo[j] = arquivo[j]*10 + (int)(ch-'0');
        }
	

   }
   
   fclose(fp);
   return j;
 
}

//funcao que identifica menor valor entre parametros passados
int menorValor(int i, int j[],int m[], int * arr){
	int k=0;
	int min =0;
	while(k<i-1){
		if(j[k]<=m[k]){
			min = arr[j[k]];
			k=i;
		}
		k++;
	}
	k=0;
	while(k<i-1){
		//printf(" %d e %d ||",min,arr[j[k+1]]);
		if(min>=arr[j[k+1]]&&j[k+1]<=m[k+1]){
			min = arr[j[k+1]];
		}		
		k++;	
	}	
	//printf("\n");	
	return min;
}

int main(int argc, char *argv[])
{
    int * vetTotal,* vetFinal, vetFiles[argc-2][1000],tamTotal=0,size =0;
    struct timespec tinicio={0,0}, tfim={0,0};
    struct thread_args arg[atoi(argv[1])];
    int k = 0,j,m=0,line=0;

   printf("Inicio de execucao\n");
 
    int i;
    
    //tambem pega o total de numeros por arquivo
    printf("Realizando leitura dos arquivos\n");
    for(i=2;i<argc;i++){//contar qtd linhas
	size= size+numLinhas(argv[i]);

    }
    vetTotal = malloc(size*sizeof(int));
    for(i=2;i<argc;i++){//lê todos os arquivos e grava em um unico vetor
	
	line = lerArquivo(argv[i],vetTotal,line);
        
    }
    //cria o vetor final para armazenar o vetor ordenado
    vetFinal = malloc(size*sizeof(int));
    printf("\nOrdenando os dados\n");

    
    //pega data/hora atual para calcular tempo de execucao da thread
    clock_gettime(CLOCK_MONOTONIC, &tinicio);
    
    i = atoi(argv[1]);
    k=0;
    int n[i],p[i];
    //faz a criacao das threads
    while(k<i){
	arg[k].arr = vetTotal;
	arg[k].left=(size)*k/i;
	n[k]=arg[k].left;
	arg[k].first = 1;

    	arg[k].right=(size*(k+1)/i)-1;
	p[k]=arg[k].right;
	//printf("\n%d, l %d e r %d \n",k,arg[k].left,arg[k].right);
	pthread_create(&outrosTIDs[k], NULL, mergeSort, &arg[k]);
	k++;
    }

    k=0;
    while(k<i){

        pthread_join(outrosTIDs[k], NULL);
	k++;
    }
    //pega horario de final da execucao das threads e checa tempo de execucao
    clock_gettime(CLOCK_MONOTONIC, &tfim);
    printf("\nTempo de ordenacao ou thread: %f segundos \n",
           (((double)tfim.tv_sec)/1000000000+((double)tfim.tv_nsec)/1000000000)
           - (((double)tinicio.tv_sec)/1000000000 + ((double)tinicio.tv_nsec)/1000000000));
    k=0;

    /*o vetor final fica ordenado em partes iguais definidas pela quantidade de threads
	Ex: se a quantidade de threads passada no parametro eh 4, o vetor é dividido em 4 partes iguais e ordenado somente nos intervalos dessas quatro partes
	O loop abaixo junta as quatro partes em uma so.	

*/
    while(k<size){
   
	int proximoItem=0;
	
	
	proximoItem = menorValor(i,n,p,vetTotal);
	vetFinal[k]=proximoItem;


	j=0;
	while(j<i){
		if(vetTotal[n[j]]==proximoItem){
			if(n[j] <= p[j]){
				n[j]++;//comparando indice com  valor
				j = j+40;
			}
		}		
		j++;
	}

	k++;		
	}


    //faz a gravacao dos arquivos em client.data
    printf("\nGravando os dados\n");
    FILE *f = fopen("./client.dat", "w");

    for(i=0;i<size;i++){
        fprintf(f,"%d\n",vetFinal[i]);
	
    }
/*
	O codigo abaixo foi criado para gravar o tempo de execucao em um arquivo para depois analisar.
    FILE *fs = fopen("./tempos.data", "a");
    fprintf(fs,"%s;%f\n",argv[1],
           (((double)tfim.tv_sec)/1000000000+((double)tfim.tv_nsec)/1000000000)
           - (((double)tinicio.tv_sec)/1000000000 + ((double)tinicio.tv_nsec)/1000000000));
*/
    printf("\nFim da execucao\n");
 


    return 0;
}
