//筛选出大小大于阀值的流

struct dataForEdition
{
	double*** request3d;
	int requestNum;	
};

struct dataForEdition threshGaosi(double bandwidth,double*** initialData, double threshold, int num, int N)
{
	struct dataForEdition returnRequest; 
	double*** requestTemp;
	int smallest=num;
	int i,j,k,requestNum;
	double requestTime;
	
	requestTemp=(double***)malloc(num*sizeof(double**));
	
	
	for(i=0;i<num;i++)
	{
		requestTemp[i]=(double**)malloc(3*sizeof(double*));
		for(j=0;j<3;j++)
		{
			requestTemp[i][j]=(double*)malloc(N*sizeof(double));
		}
	}
	
	
	for(k=0;k<N;k++)
	{
		requestNum=0;
		requestTime=0;
		for(i=0;i<num;i++)
		{
			if(initialData[i][1][k]>threshold)
			{
				requestTemp[requestNum][0][k]=requestTime+initialData[i][0][k];
				requestTemp[requestNum][1][k]=initialData[i][1][k]/bandwidth;
				requestTemp[requestNum][2][k]=initialData[i][2][k];
				requestNum++;
				requestTime=0;
			}
			else
				requestTime+=initialData[i][0][k];
		}
		if(requestNum<smallest)
			smallest=requestNum;
	}
	
	
	for(i=0;i<num;i++)
	{
		for(j=0;j<3;j++)
			free(initialData[i][j]);
		free(initialData[i]);
	}
	
	free(initialData);
	
	
	for(i=smallest;i<num;i++)
	{
		for(j=0;j<3;j++)
			free(requestTemp[i][j]);
		free(requestTemp[i]);
	}
	
	
	
	returnRequest.request3d=requestTemp;
	returnRequest.requestNum=smallest;
	return returnRequest;			
}
