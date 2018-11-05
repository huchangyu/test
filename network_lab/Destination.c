//产生流的目的地数组 有热点的情况
//这里假设热点就是前hotN个


//目的地矩阵
struct ToRstruct{
	int** candidate;
	int number1;
	int number2;
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
	if(isAve==0)   //isave==0一定满足
	{
		int hotNum1,commonNum1,totalnumber1;//候选目的地中热点比例对应数量,普通比例对应数量和总数 
		int hotNum2,commonNum2,totalnumber2;
		
		
		
		if(hotPercentage==0)
		{
			hotNum1=1;
			commonNum1=1; 
			totalnumber1=N-1;
			hotNum2=1;
			commonNum2=1; 
			totalnumber2=N-1;
		}
		else if(hotPercentage==100)
		{
			hotNum1=1;
			commonNum1=0;
			totalnumber1=hotN;
			hotNum2=1;
			commonNum2=0;
			totalnumber2=hotN-1;
		}
		     else
		{
			int more1=(int)(100*hotPercentage/hotN);
			int less1=(int)(100*(100-hotPercentage)/(N-1-hotN));
			if(more1<=less1)
			{
		        hotNum1=1;
				commonNum1=1; 
				totalnumber1=N-1;
				hotNum2=1;
				commonNum2=1; 
				totalnumber2=N-1;
			}
			else
			{
				int diviser1=commonDivisor(more1,less1);
				if(hotN==1)
				{
					hotNum1=more1/diviser1;
					commonNum1=less1/diviser1;
					totalnumber1=hotNum1*hotN+commonNum1*(N-1-hotN);
					
					hotNum2=1;
					commonNum2=1; 
					totalnumber2=N-1;
				}
				else
				{
					int more2=(int)(100*hotPercentage/(hotN-1));
					int less2=(int)(100*(100-hotPercentage)/(N-hotN));
					int diviser2=commonDivisor(more2,less2);
					hotNum1=more1/diviser1;
					commonNum1=less1/diviser1;
					totalnumber1=hotNum1*hotN+commonNum1*(N-1-hotN); 
					
					hotNum2=more2/diviser2;
					commonNum2=less2/diviser2;
					totalnumber2=hotNum2*(hotN-1)+commonNum2*(N-hotN); 
				}
			}	
		}
		
		
		
		
		
		
		
		
		
		
		
		int count,hotCount,commonCount;
		hotCount=0;
		commonCount=0;
		ToR=(int**)malloc(N*sizeof(int*));
		
		
		
		for(i=0;i<hotN;i++)
			ToR[i]=(int*)malloc(totalnumber2*sizeof(int));
		
		
		for(i=hotN;i<N;i++)
			ToR[i]=(int*)malloc(totalnumber1*sizeof(int));
		
		
		int ToRCandidate[N][N-1];
		
		
		
		
		for(i=0;i<N;i++)
		{
			count=0;
			for(j=0;j<N-1;j++)
			{
				if(count==i)
					count++;
				ToRCandidate[i][j]=count;
				count++;
			}
		}
		
		
		
		
		for(i=0;i<hotN;i++)
		{
			count=0;
			for(j=0;j<hotN-1;j++)
			{
				while(hotCount++<hotNum2)
					ToR[i][count++]=ToRCandidate[i][j];
				hotCount=0;
			}
			for(j=hotN-1;j<N-1;j++)
			{
				while(commonCount++<commonNum2)
					ToR[i][count++]=ToRCandidate[i][j];
				commonCount=0;
			}
		}
		
		
		
		for(i=hotN;i<N;i++)
		{
			count=0;
			for(j=0;j<hotN;j++)
			{
				while(hotCount++<hotNum1)
					ToR[i][count++]=ToRCandidate[i][j];
				hotCount=0;
			}
			for(j=hotN;j<N-1;j++)
			{
				while(commonCount++<commonNum1)
					ToR[i][count++]=ToRCandidate[i][j];
				commonCount=0;
			}
		}
		
		
		
		
		
		
		
		
		struct1.candidate=ToR;
		struct1.number1=totalnumber1;
		struct1.number2=totalnumber2;
	}
	return struct1;
		
} 
