//没有热点的情况

struct ToRstruct{
	int** candidate;
	int number;
};

//求两个整数的最大公约数 
int commonDivisor(int a, int b)
{
	int bigger=a>b?a:b;
	int smaller=b<a?b:a;
	int remainder=bigger%smaller;
	while(remainder!=0)
	{
		bigger=smaller;
		smaller=remainder;
		remainder=bigger%smaller;
	}
	return smaller;
}

//生成候选目的地矩阵 
struct ToRstruct destination(int isAve, int N, int hotN, double hotPercentage)
{
	int i,j,k;
	int** ToR;
	struct ToRstruct struct1;
	if(isAve==1)
	{
		ToR=(int**)malloc(N*sizeof(int*));
		for(i=0;i<N;i++)
				ToR[i]=(int*)malloc((N-1)*sizeof(int));
		for(i=0;i<N;i++)
		{	
			k=0;
			for(j=0;j<N;j++)
			{
				if(j!=i)
					ToR[i][k++]=j;
			}
		}
		struct1.candidate=ToR;
		struct1.number=N-1;
		return struct1;
	}
	else
	{
		int hotNum,commonNum,totalnumber;//候选目的地中热点比例对应数量,普通比例对应数量和总数 
		if(hotPercentage==0)
		{
			hotNum=1;
			commonNum=1; 
			totalnumber=N-1;
		}
		else if(hotPercentage==100)
		{
			hotNum=1;
			commonNum=0;
			totalnumber=hotN;
		}
		else
		{
			int more=(int)(100*hotPercentage/hotN);
			int less=(int)(100*(100-hotPercentage)/(N-1-hotN));
			int diviser=commonDivisor(more,less);
			hotNum=more/diviser;
			commonNum=less/diviser;
			totalnumber=hotNum*hotN+commonNum*(N-1-hotN); 
		}
		if((ToR=(int**)malloc(N*sizeof(int*)))==NULL)
		{
				printf("malloc error2\n");
		};
		//热点ToR数量累计，普通ToR数量累计,候选热点目的地的比例累计 ,普通目的地比例累计 
		int count,hotCount,commonCount;
		hotCount=0;
		commonCount=0;
		for(i=0;i<N;i++)
			ToR[i]=(int*)malloc(totalnumber*sizeof(int));
		int ToRCandidate[N][N-1];
		for(i=0;i<N;i++)
			for(j=0;j<N-1;j++)
				ToRCandidate[i][j]=(i+j+1)%N;
		for(i=0;i<N;i++)
		{
			count=0;
			for(j=0;j<N-1;j++)
			{
				if(j<hotN)
				{
					while(hotCount++<hotNum)
						ToR[i][count++]=ToRCandidate[i][j];
					hotCount=0;
				}
				else
				{
					while(commonCount++<commonNum)
						ToR[i][count++]=ToRCandidate[i][j];
					commonCount=0;
				}
			}	
		}
		struct1.candidate=ToR;
		struct1.number=totalnumber;
		return struct1;
	}
		
} 
