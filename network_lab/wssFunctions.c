//wssEdition中调用的各个函数

struct requestMin
{
	double requestmin;
	double flowtime; 
	int requestminToR;
	int requestminDest;
};
struct qProcessMin
{
	double qMin;
	int minWavelength;
	int minQ;
	int minTOR;
};
struct AProcessMin
{
	double AMin;
	int Aq;
	int AToR; 
};
struct qProcessStruct //qProcess结构
{
	double processTime;
	int wavelength;
};

//寻找下一组请求中最先到达的一个请求 
struct requestMin findRequestMin(double** request, int N)
{
	struct requestMin returnStruct;
	int i,minToR=0;
	double minTemp=request[0][0];
	for(i=1;i<N;i++)
	{
		if(request[i][0]<minTemp)
		{
			minTemp=request[i][0];
			minToR=i;
		}
	}
	returnStruct.requestmin=minTemp;
	returnStruct.requestminToR=minToR;   //int
	returnStruct.requestminDest=request[minToR][2]; //int
	returnStruct.flowtime=request[minToR][1];  
	return returnStruct; 
}

//寻找qProcess中的最小值
struct qProcessMin findQProcessMin(struct qProcessStruct*** qProcess, int** processNq, int q, int p, int N)
{
	struct qProcessMin returnStruct;
	double processmin=10^3;
	
	int minwavelength=0;
	int minq=0;
	int mintoR=0;
	
	int i,j,k;
	for(i=0;i<N;i++)
		for(j=0;j<q;j++)
			for(k=0;k<processNq[i][j];k++)
			{
					if(qProcess[i][j][k].processTime<processmin)
					{
						processmin=qProcess[i][j][k].processTime;
						minwavelength=qProcess[i][j][k].wavelength;
						minq=j;
						mintoR=i;
					}
			}
	returnStruct.qMin=processmin;    //double
	returnStruct.minWavelength=minwavelength;  //都是int
	returnStruct.minQ=minq;
	returnStruct.minTOR=mintoR;
	return returnStruct;
}

//寻找AProcess中的最小值 
struct AProcessMin findAProcessMin(double*** AProcess, int q, int p, int N)
{
	struct AProcessMin returnStruct;
	int flag=0;
	
	double Amin=10^5;
	int minq=0;
	int mintoR=0;
	
	int j,k;
	for(j=0;j<q;j++)
		for(k=0;k<N;k++)
		{
			if(flag==0)
			{
				if(AProcess[p][j][k]>0)
				{
					Amin=AProcess[p][j][k];
					minq=j;
					mintoR=k;
					flag=1;
				}
			}
			else
			{
				if(AProcess[p][j][k]>0 && AProcess[p][j][k]<Amin)
				{
					Amin=AProcess[p][j][k];
					minq=j;
					mintoR=k;
				} 
			}
		}
	returnStruct.AMin=Amin;     //double         上一个流传输完成加上重构的时间。
	returnStruct.Aq=minq;       //int
	returnStruct.AToR=mintoR;   //int
	return returnStruct;
}
//.Amin是上一个最快完成的流传输完成加上重构的时间。

//计算两个重构时间造成的实际影响，也就是当重构完ocs后还需要的更多的时间来重构wsstime。
double twoRecnTime(double wssTime, double ocsTime)
{
	double temp,diffTime;
	temp=wssTime-ocsTime;
	if(temp<0)
		diffTime=0;
	else
		diffTime=temp;
	return diffTime;	 
} 

//对流到达的情况更新newRequest和qProcess    newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
                                       //   request3d, accumlate[minRequest.requestminToR], N, p, q, -1

void updateRequestProcess(double** newRequest, int wavelength, struct qProcessStruct*** qProcess, int** Nq, 
                          int requestMinToR, double*** request3d, int accumlate, int N, int p, int q, int whichQ)
{
	int i,j,k;
	double minusTime=newRequest[requestMinToR][0];
	for(i=0;i<N;i++)
		newRequest[i][0]-=minusTime;
	newRequest[requestMinToR][0]=request3d[accumlate][0][requestMinToR];
	newRequest[requestMinToR][1]=request3d[accumlate][1][requestMinToR];
	newRequest[requestMinToR][2]=request3d[accumlate][2][requestMinToR]; 
	for(i=0;i<N;i++)
		for(j=0;j<q;j++)
			for(k=0;k<Nq[i][j];k++)
			{
					qProcess[i][j][k].processTime-=minusTime;
			}
}

//对流发送完成的情况更新newRequest和QProcess
void updateQProcess(double** newRequest, struct qProcessStruct*** qProcess, int** Nq, double minTime, int N, int p, int q)
{
	int i,j,k;
	for(i=0;i<N;i++)
	{
		newRequest[i][0]-=minTime;
	}	
	for(i=0;i<N;i++)
		for(j=0;j<q;j++)
			for(k=0;k<Nq[i][j];k++)
					qProcess[i][j][k].processTime-=minTime;   //所有n上面的所有q上面的所有正在传输的p'<p，他们的剩余传输完成时间都减去已经流逝的时间。
} 

//更新AProcess
void updateAProcess(double*** AProcess, double timePast, int p, int q, int N)
{
	int j,k;
	for(j=0;j<q;j++)
		for(k=0;k<N;k++)
		{
			if(AProcess[p][j][k]>0)
				AProcess[p][j][k]-=timePast;
		} 
}                 //每个n上面的每个q里面记录的时间都减去已经流逝的时间。

//找到accumlate数组中的最大值
int accumalateMax(int a[], int N)
{
	int maxTemp,i;
	maxTemp=a[0];
	for(i=1;i<N;i++)
	{
		if(a[i]>maxTemp)
			maxTemp=a[i];
	}
	return maxTemp;
} 

//给一维数组求和
int sumMatrix(int m[], int number)
{
	int sum=0;
	int i;
	for(i=0;i<number;i++)
	{
		sum+=m[i];
	}
	return sum;
}

//两个一维数组相加 
int* addMatrix(int a[], int b[], int number)
{
	int* ab;
	ab=(int*)malloc(number*sizeof(int));
	int i;
	for(i=0;i<number;i++)
	{
		ab[i]=a[i]+b[i];
	}
	return ab;
} 

//寻找一维数组中的最小值
int minMatrix(int a[], int number)
{
	int mintemp=a[0];
	int i;
	for(i=1;i<number;i++)
	{
		if(a[i]<mintemp)
			mintemp=a[i];
	}
	return mintemp;
} 

//寻找一维数组中第一个0的位置 0一点存在
int findZero(int a[], int number)
{
	int i, tag;
	for(i=0;i<number;i++)
	{
		if(a[i]==0)
		{
			tag=i;
			break;
		}
	}
	return tag;
} 
