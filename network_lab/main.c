//main()函数所在

#include "gaosiTime.c"
#include "threshGaosi.c"
#include "wssEditionq1q2.c"

int main()
{
	struct dataForEdition dataRequest;    //用三级指针和整形数表示大小大于阀值的流。
	struct finalData dataGaosi;        //用三级指针和整形数表示大小服从高斯分布的流。
	int i,j,k;                         //循环变量？
	double A,wssTime;
	double*** initialData;               //干什么用的？
	double threshold,simTime,arrival;
	double miu,gama,bandwidth;
	int initialDataNum;                // 不是系统的参数，
	int N,p,qhot,qcommon,hotN;
	double hotPercentage;
	int judge=0,count=0;              //不是系统的参数
	double lastResult,resultTemp;     //未赋值
	clock_t start,end;
	
	bandwidth=1e10;  //带宽     
	A=1e-4;			//OCS重构时间								
	wssTime=1e-4;   //wss重构时间
	threshold=10;   //阀值（选择大于该值的流传输）
	simTime=1;      //时间
	arrival=300;    //到达率
	miu=9*1e7;      //流大小均值
	gama=1e7;       //流大小标准差
	N=32;           //ToR数量
	p=20;           //transceiver数量
	qhot=15;        //热点ToR上的q数量
	qcommon=10;     //非热点ToR上的q数量
	hotN=4;         //热点TOR数量
	hotPercentage=80;//热点ToR所占流量比例
	
	
	
	double*** result;                       //[p+1][qhot+1][qcommon+1]
	result=(double***)malloc((p+1)*sizeof(double**));
	for(i=0;i<p+1;i++)
	{
		result[i]=(double**)malloc((qhot+1)*sizeof(double*));
		for(j=0;j<qhot+1;j++)
			result[i][j]=(double*)malloc((qcommon+1)*sizeof(double));
	}
	
	for(i=0;i<p+1;i++)
		for(j=0;j<qhot+1;j++)
			for(k=0;k<qcommon+1;k++)
				result[i][j][k]=(double)5;          //每个result设置成5？
	
	srand(time(NULL));
	start=clock();
	
	dataGaosi=gaosiTime(simTime, arrival, miu, gama, N, hotN, hotPercentage);  //产生大小服从高斯分布的流。
	dataRequest=threshGaosi(bandwidth, dataGaosi.requestData, threshold, dataGaosi.smallest, N);
	
	
	
	for(k=1;k<qcommon+1;k++)
	{
		printf("qcommon=%d:",k);
		for(j=k;j<qhot+1;j++)
		{
			judge=0;
			lastResult=0;
			for(i=1;i<p+1;i++)
			{
				if(judge==0)
				{
					resultTemp=wssEdition(i, k, j, hotN, dataRequest.request3d, dataRequest.requestNum, N, A, wssTime);
					result[i][j][k]=resultTemp;
					if(lastResult==resultTemp)
						judge=1;
					lastResult=resultTemp;
				}
				else	
					result[i][j][k]=lastResult;
			}
			printf("%d ",j);
		}
		printf("\n");
	}
	
	
	
	
	
	
	
	
	printf("\n");
	end=clock();
	for(i=0;i<dataRequest.requestNum;i++)
	{
		for(j=0;j<3;j++)
			free(dataRequest.request3d[i][j]);
		free(dataRequest.request3d[i]);
	}
	free(dataRequest.request3d);
	/*FILE* fp; 
	if((fp=fopen("C:\\Users\\zhujie\\Desktop\\CfreeSimulation\\Simulation2\\data\\find_appropriatepq4.txt","w"))==NULL)
	{
		printf("fail\n");
	}
	else
    	printf("open\n");
	for(k=1;k<qcommon+1;k++)
	{
	    for(i=1;i<p+1;i++)
		{	
			for(j=1;j<qhot+1;j++)
				fprintf(fp,"%lf ",result[i][j][k]);
			fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
	}
	fprintf(fp," bandwidth=%lf\n wssTime=%lf\n threshold=%lf\n simTime=%lf\n arrival=%lf\n ",bandwidth,wssTime,threshold,simTime,arrival);
	fprintf(fp,"A= %lf\n miu=%lf\n gama=%lf\n N=%d\n p=%d\n qhot=%d\n qcommon=%d\n hotN=%d\n hotPercentage=%lf\n",A,miu,gama,N,p,qhot,qcommon,hotN,hotPercentage);
	fprintf(fp,"\n运行时间为%lf\n",(double)(end-start)/CLOCKS_PER_SEC);
	fclose(fp);*/
	for(k=1;k<qcommon+1;k++)
	{
		printf("qcommon:%d\n",k);
		for(j=0;j<qhot+1;j++)
			printf("%d      ",j);
		printf("\n");
		for(i=1;i<p+1;i++)
		{	
			if(i<10)
				printf("%d    ",i);
			else
				printf("%d   ",i);
			for(j=1;j<qhot+1;j++)
				printf("%.4lf ",result[i][j][k]);
			printf("\n");
		}
		printf("\n");
	}
	for(i=0;i<p+1;i++)
	{
		for(j=0;j<qhot+1;j++)
			free(result[i][j]);
		free(result[i]);
	}
	free(result);
	printf("-----------------------------------\n");
	printf("%f\n",(double)(end-start)/CLOCKS_PER_SEC);
	return 0;
}
