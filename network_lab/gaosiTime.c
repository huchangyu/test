//产生需要传输的流，一个流由三部分表示，到达的间隔时间，流的大小和流的目的地（流的源ToR由存放的数组编号表示）

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "Destination.c"

//gaosiTime的返回结构体 
struct finalData{
	double*** requestData;
	int smallest;
};

//产生[0,1]的均匀分布的随机数 
double normalRand()
{
	return (double)rand()/RAND_MAX;
}

//生成服从参数为lamda的指数分布 
double expRand(double lamda)
{
	double randTemp=0;
	while(randTemp==0 || randTemp==1)
		randTemp=normalRand();
	return (-log(1-randTemp)/lamda);	
}

//由均匀分布产生高斯分布的随机数 (-6 6)
double gaussRand()
{
	double xi[12],sum=0;
	int i;
	for(i=0;i<12;i++)
	{
		xi[i]=normalRand();
		sum+=xi[i];
	}
	return (sum-6); 
}

//随机产生大小服从高斯分布的流 
struct finalData gaosiTime(double time, double arrival, double miu, 
				double gama, int N, int hotN, double hotPercentage)
{
	//srand(time(NULL));
	struct finalData returnData;
	struct ToRstruct ansToR;
	
	
	int largest=floor(time*arrival);
	int numberTemp=largest;
	//double initialData[numberTemp][3][N];
	double*** initialData;
	
	
	int i,j,k,requestNum,destinationTemp,destinationNum;
	double timeSum,flowsize,requestTime;
	int isAve=0; //1是均匀分布
	
	//分配initialdata指针的空间
	initialData=(double***)malloc(numberTemp*sizeof(double**));
	for(i=0;i<largest;i++)
	{
		initialData[i]=(double**)malloc(3*sizeof(double*));
		for(j=0;j<3;j++)
		{
			initialData[i][j]=(double*)malloc(N*sizeof(double));
		}
	}
	
	//生成候选目的地矩阵
	ansToR=destination(isAve,N,hotN,hotPercentage);
	
	for(i=0;i<hotN;i++)
	{
		timeSum=0;
		requestNum=0;
		while(timeSum<time && requestNum<numberTemp)
		{
			requestTime=expRand(arrival);
			timeSum+=requestTime;
			flowsize=miu+gama*gaussRand();
			
			destinationTemp=floor((double)rand()/(RAND_MAX+1)*ansToR.number2);
			destinationNum=ansToR.candidate[i][destinationTemp];
			
			initialData[requestNum][0][i]=requestTime;
			initialData[requestNum][1][i]=flowsize;
			initialData[requestNum][2][i]=destinationNum;
			requestNum++;
		}
		free(ansToR.candidate[i]);
		largest=requestNum<largest?requestNum:largest;	
	}
	
	
	
	
	
	
	
	
	for(i=hotN;i<N;i++)
	{
		timeSum=0;
		requestNum=0;
		while(timeSum<time && requestNum<numberTemp)
		{
			requestTime=expRand(arrival);
			timeSum+=requestTime;
			flowsize=miu+gama*gaussRand();
			destinationTemp=floor((double)rand()/(RAND_MAX+1)*ansToR.number1);
			destinationNum=ansToR.candidate[i][destinationTemp];
			initialData[requestNum][0][i]=requestTime;
			initialData[requestNum][1][i]=flowsize;
			initialData[requestNum][2][i]=destinationNum;
			requestNum++;
		}
		free(ansToR.candidate[i]);
		largest=requestNum<largest?requestNum:largest;	
	}
	free(ansToR.candidate);
	
	
	
	for(i=largest;i<numberTemp;i++)
	{
		for(j=0;j<3;j++)
		{
			free(initialData[i][j]);
		}
		free(initialData[i]);
	}
	
	
	returnData.requestData = initialData;
	returnData.smallest = largest;
	return returnData;
	
	
	
	
	
	
	
	
	
	
	
	
	/*double*** request;
	request=(double***)malloc(largest*sizeof(double**));
	for(i=0;i<largest;i++)
	{
		request[i]=(double**)malloc(3*sizeof(double*));
		for(j=0;j<3;j++)
			request[i][j]=(double*)malloc(N*sizeof(double)); 
	}
	for(i=0;i<largest;i++)
		for(j=0;j<3;j++)
			for(k=0;k<N;k++)
				request[i][j][k]=initialData[i][j][k];*/
	
} 

/*int main()
{
	printf("hello\n");
	clock_t start,end;
	int i,j; 
    srand(time(NULL));
	struct finalData request3d;
	double time, arrival, miu, gama, hotPercentage;
	int N,hotN;
	time=10;
	arrival=100;
	miu=1000;
	gama=10;
	hotPercentage=50;
	N=6;
	hotN=1;
	int count[N];
	for(i=0;i<N;i++)
	{
		count[i]=0;
	}
	start=clock();
	request3d=gaosiTime(time,arrival,miu,gama,N,hotN,hotPercentage);
	printf("22222222222222222\n");
	end=clock();
	for(i=0;i<request3d.smallest;i++)
	{
		for(j=0;j<3;j++)
			printf("%lf  ",request3d.requestData[i][j][1]);
		count[(int)request3d.requestData[i][2][1]]++;
		printf("\n");
	}
	for(i=0;i<request3d.smallest;i++)
	{
		for(j=0;j<3;j++)
		{
			free(request3d.requestData[i][j]);
		}
		free(request3d.requestData[i]);
	}
	free(request3d.requestData);
	printf("-------------------------\n");
	for(i=0;i<N;i++)
	{
		printf("%d ",count[i]);
	}
	printf("\n");
	printf("%f\n",(double)(end-start)/CLOCKS_PER_SEC);
	return 1;
}*/

