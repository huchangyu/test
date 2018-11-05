//处理各个流并计算阻塞率的核心函数 有热点

#include "wssFunctionsq1q2.c"
       //        波长数   非热点tor上的q  热点上    热点数 一定时间内所有需要传输的流？   流的个数      TOR数   ocstime
double wssEdition(int p, int qcommom, int qhot, int hotN, double*** request3d,         int totalNum, int N, double A, double wssTime)
{
	//正常情况下qhot>qcommon
	int q=qhot>qcommom?qhot:qcommom;
	struct requestMin minRequest;
	struct qProcessMin minQProcess;
	struct AProcessMin minAProcess;   //记录还剩多少时间，一个新的流开始传输。
	int pSendState[N][p]; //发送端ToR上p个波长占用情况 
	int pRecvState[N][p]; //接受端ToR上p个波长占用情况 
	int qSendState[N];    //发送端ToR上q个端口占用情况 
	int qRecvState[N];    //接收端ToR上q个端口占用情况 
	double*** AProcess;   //重构阶段状态                                      [(p+1)*q*N] 
	struct qProcessStruct*** qProcess;	//发送阶段状态                        [N*q*p]
	int qDest[N][q];             		//各发送ToR上q个端口相连的ToR 
	int qRecvDest[N][q];         		//各接收ToR上q个端口相连的ToR
	int accumlate[N];            		//各ToR上请求数累计
	int block[N];                		//各ToR上阻塞请求数累计
	int blockPQ[2];              		//阻塞造成的原因，三种情况，p还是q  还是p、q同时
	int** processNq;                    //记录N q上的流数量                                【N*q】//一个Q上可以有多个流
	double** newRequest;         		//下一次进入系统的请求                             [N*3]
	int i,j,k,count=0;                  //循环变量，一般分别指一维、二维、三维 
	int wavelength,judgeSameToR,multiq; //波长,判断因子 
	int zeroTag,whichq,sourceToR,sourceq;//寻找可用波长, 空闲的q,判断连接是否可拆除 
	double diffTime,sumA,sumq;			//实际重构时间,判断连接是否拆除时的量 
	double blockingProbablity;          //最终的阻塞率 
	int* ab;                            //接收数组累加和 
	int qByN[N];                        //存放各个ToR上的q的数量 

	//对以上定义的所有指针分配内存 
	AProcess=(double***)malloc((p+1)*sizeof(double**));
	qProcess=(struct qProcessStruct***)malloc(N*sizeof(struct qProcessStruct**));
	newRequest=(double**)malloc(N*sizeof(double*));
	processNq=(int**)malloc(N*sizeof(int*));
	for(i=0;i<N;i++)
	{
		newRequest[i]=(double*)malloc(3*sizeof(double));
		processNq[i]=(int*)malloc(q*sizeof(int));
	}
	for(i=0;i<p+1;i++)
	{	
		AProcess[i]=(double**)malloc(q*sizeof(double*));
		for(j=0;j<q;j++)
		{
			AProcess[i][j]=(double*)malloc(N*sizeof(double));
		}
	}
	for(i=0;i<N;i++)
	{
		qProcess[i]=(struct qProcessStruct**)malloc(q*sizeof(struct qProcessStruct*));
		for(j=0;j<q;j++)
			qProcess[i][j]=(struct qProcessStruct*)malloc(p*sizeof(struct qProcessStruct));
	} 
	
	//对以上定义的所有数组和指针的初始化
	for(i=0;i<p;i++)
		for(j=0;j<q;j++)
			for(k=0;k<N;k++)
			{
				AProcess[i][j][k]=0;
				qProcess[k][j][i].processTime=0;
				qProcess[k][j][i].wavelength=-1;
			}
	for(i=0;i<N;i++)
		for(j=0;j<q;j++)
		{
			qDest[i][j]=-1;
			qRecvDest[i][j]=-1;
			AProcess[p][j][i]=0;
			processNq[i][j]=0;
		}
	for(i=0;i<N;i++)
		for(j=0;j<p;j++)
		{
			pSendState[i][j]=0;
			pRecvState[i][j]=0;
		}
	for(i=0;i<N;i++)
	{
		accumlate[i]=0;
		block[i]=0;
		qSendState[i]=0;
		qRecvState[i]=0;
		newRequest[i][0]=0;
		newRequest[i][1]=0;
		newRequest[i][2]=0;
	}
	blockPQ[0]=0;
	blockPQ[1]=0;
	
	for(j=0;j<3;j++)
		for(k=0;k<N;k++)
  			newRequest[k][j]=request3d[0][j][k];  //第0个请求
  			
	for(i=0;i<hotN;i++)
		qByN[i]=qhot;
	for(i=hotN;i<N;i++)
		qByN[i]=qcommom;
	
/*	for(i=0;i<N;i++)
		printf("%d ",qByN[i]);   十进制整数
	printf("\n");              回车换行*/

	//对第一个请求的状态更新 
	minRequest=findRequestMin(newRequest,N);
	accumlate[minRequest.requestminToR]++;
	
	wavelength=0;
	
	pSendState[minRequest.requestminToR][wavelength]=1;//[x][0]=1 //发送端ToR上p个波长占用情况 
	pRecvState[minRequest.requestminDest][wavelength]=1;//[y][0]=1  接受端ToR上p个波长占用情况
	qSendState[minRequest.requestminToR]=1;     //发送端ToR上q个端口占用情况
	qRecvState[minRequest.requestminDest]=1;   //接收端ToR上q个端口占用情况
	
	diffTime=twoRecnTime(wssTime,A);            // 为什么会是0
	
	AProcess[wavelength][0][minRequest.requestminToR]=minRequest.flowtime+diffTime;//wss需要的额外重构时间加上传输时间。
	AProcess[p][0][minRequest.requestminToR]=A; //在完成传输后第N个tor的0号p端口还需要A时间重构。
	
	qDest[minRequest.requestminToR][0]=minRequest.requestminDest;//各发送ToR上q个端口相连的ToR 
	qRecvDest[minRequest.requestminDest][0]=minRequest.requestminToR;	//各接收ToR上q个端口相连的ToR 每个发送tor上的Q与一个接受TOR上的Q相连。
	
	updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									    request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
 
 	
	/*找出用时最少的流，在发送这个流的tor上+1
	  波长=0（因为是第一个流）
	  修改pq占用情况。对于p, 标记发送的第n个tor 的p个波长的其中一个为1（对于第一个
	  流标记第0个波长为1） 更新q直接把第n个tor 上面使用的q值记录即可。
	  
	  更新重构阶段状态。记录流传输时间和重构时间（这个时间属于每个tor上的pq连接，用三级指针表示）。
	                还要记录无关p,只和tor,q有关的重构时间。
      
	  记录各发送ToR上q个端口相连的ToR 和 各接收ToR上q个端口相连的ToR 分别用N*q的二维数组表示。

	   更新发送阶段状态。newrequst中间的每个请求的时间都减去已经消耗的时间。装载新的请求。
	                    并且对于每一个tor 上的每一个使用的p, 将其“过程时间”减去已经消耗的时间。*/
	
	
	
	
	
	
	
	
	
	
	
	
	//循环处理请求 
 	while(accumalateMax(accumlate,N)<totalNum-1)
 	{
 		//printf("%d\n",count++);
 		/*for(i=0;i<N;i++)
			for(j=0;j<q;j++)
		 		for(k=0;k<processNq[i][j];k++)
		 			printf("%lf  ", qProcess[i][j][k]);
	    printf("\n");*/
	 	minRequest=findRequestMin(newRequest,N);
	 	minQProcess=findQProcessMin(qProcess, processNq, q, p, N);
	 	minAProcess=findAProcessMin(AProcess, q, p, N);
	 	
	 	//新到的请求最先发生 
	 	if(minRequest.requestmin<=minQProcess.qMin && minRequest.requestmin<=minAProcess.AMin)
	 	{
	 		/*printf("000000000000: ");
	 		printf("time:%lf,ToR:%d\n",minRequest.requestmin,minRequest.requestminToR);*/
	 		accumlate[minRequest.requestminToR]++;
	 		//没有空闲波长可用 （更新最新的n个请求和正在传输的过程，减去上一个请求的时间）（更新Aprocess，每个TOR上的每个q都减去已经流逝的时间）
	 		if(sumMatrix(pSendState[minRequest.requestminToR],p)==p || sumMatrix(pRecvState[minRequest.requestminDest],p)==p)
	 		{
		 		block[minRequest.requestminToR]++;
		 		blockPQ[0]++;
		 		updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									 request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
                updateAProcess(AProcess, minRequest.requestmin, p, q, N);
		 	}
		 	//没有空闲的q端口 （但是可以复用q）（如果复用失败，更新请求时间，运输时间和Aprocess)
		 	else if(qSendState[minRequest.requestminToR]==qByN[minRequest.requestminToR] || 
 					qRecvState[minRequest.requestminDest]==qByN[minRequest.requestminDest])
		 	{
	 			judgeSameToR=0;  //当有已经存在的连接时，judgesametor变为1 multiq变为已经处于连接状态的发送端（q'）值
			    for(i=0;i<qByN[minRequest.requestminToR];i++)
				{
					if(qDest[minRequest.requestminToR][i]==minRequest.requestminDest)
					{
						judgeSameToR=1;
						multiq=i;
						break;
					}
				}
				//请求被阻塞 
				ab=addMatrix(pSendState[minRequest.requestminToR],pRecvState[minRequest.requestminDest],p);
                if(minMatrix(ab,p)>0 || judgeSameToR==0 )
				{
					block[minRequest.requestminToR]++;
					if(minMatrix(ab,p)>0)
						blockPQ[0]++;
					else
						blockPQ[1]++;
					updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									 request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
                	updateAProcess(AProcess, minRequest.requestmin, p, q, N);
				}
				//有相同目的地的连接 //更新请求时间，更新aprocess时间
				else
				{
					zeroTag=findZero(ab,p);
					wavelength=zeroTag;
					pSendState[minRequest.requestminToR][wavelength]=1;
					pRecvState[minRequest.requestminDest][wavelength]=1;
					if(AProcess[p][multiq][minRequest.requestminToR]==0)
					{
						qProcess[minRequest.requestminToR][multiq][   processNq[minRequest.requestminToR][multiq]  ].processTime
						                                                                       =minRequest.flowtime+wssTime;
						qProcess[minRequest.requestminToR][multiq][processNq[minRequest.requestminToR][multiq]].wavelength
						                                                                                        =wavelength;                                                                       
						updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									 request3d, accumlate[minRequest.requestminToR], N, p, q, multiq);
                		updateAProcess(AProcess, minRequest.requestmin, p, q, N);
                		processNq[minRequest.requestminToR][multiq]++;//假如有7个p，在上一步骤中带入数组中使用的时候用6代替。
					}
					else //虽然找到了要重构的p但是正在重构中
					{
						diffTime=twoRecnTime(wssTime,AProcess[p][multiq][minRequest.requestminToR]);
						AProcess[wavelength][multiq][minRequest.requestminToR]=diffTime+minRequest.flowtime;
						updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									 request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
                		updateAProcess(AProcess, minRequest.requestmin, p, q, N);
					}	
				} 
				free(ab);
	 		}
	 		    
			
			
			
			
			
			
			
			
			
			
	           //请求先到达，四种端口都没有占满 
	             else
		 	{
		 		ab=addMatrix(pSendState[minRequest.requestminToR],pRecvState[minRequest.requestminDest],p);
		 		if(minMatrix(ab,p)>0)
		 		{
		 			block[minRequest.requestminToR]++;
		 			blockPQ[0]++;
		 			updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									 request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
        			updateAProcess(AProcess, minRequest.requestmin, p, q, N);
		 		}
		 
                 
				
				else
			 	{
		 			judgeSameToR=0;
		 			for(i=0;i<qByN[minRequest.requestminToR];i++)
		 			{
			 			if(qDest[minRequest.requestminToR][i]==minRequest.requestminDest)
						{
							judgeSameToR=1;
							multiq=i;
							break;
						}
			 		}
			 		if(judgeSameToR==1)
			 		{
		 				zeroTag=findZero(ab,p);
						wavelength=zeroTag;
						pSendState[minRequest.requestminToR][wavelength]=1;
						pRecvState[minRequest.requestminDest][wavelength]=1;
						if(AProcess[p][multiq][minRequest.requestminToR]==0)
						{
							qProcess[minRequest.requestminToR][multiq][processNq[minRequest.requestminToR][multiq]].processTime
							                                                                       =minRequest.flowtime+wssTime;
       	                    qProcess[minRequest.requestminToR][multiq][processNq[minRequest.requestminToR][multiq]].wavelength
							                                                                                       =wavelength;
							updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									 request3d, accumlate[minRequest.requestminToR], N, p, q, multiq);
                			updateAProcess(AProcess, minRequest.requestmin, p, q, N);
                			processNq[minRequest.requestminToR][multiq]++;
						}
						else
						{
							diffTime=twoRecnTime(wssTime,AProcess[p][multiq][minRequest.requestminToR]);
							AProcess[wavelength][multiq][minRequest.requestminToR]=diffTime+minRequest.flowtime;
							updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
										 request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
	                		updateAProcess(AProcess, minRequest.requestmin, p, q, N);
						}
		 			}
		 			
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					else
				 	{
			 			zeroTag=findZero(ab,p);
						wavelength=zeroTag;
						pSendState[minRequest.requestminToR][wavelength]=1;
						pRecvState[minRequest.requestminDest][wavelength]=1;
						qSendState[minRequest.requestminToR]++;
						qRecvState[minRequest.requestminDest]++;
						for(i=0;i<qByN[minRequest.requestminToR];i++)
						{
							if(qDest[minRequest.requestminToR][i]==-1)
							{
								qDest[minRequest.requestminToR][i]=minRequest.requestminDest;
								whichq=i;
								break; 
							}
						}
						for(i=0;i<qByN[minRequest.requestminDest];i++)
						{
							if(qRecvDest[minRequest.requestminDest][i]==-1)
							{
								qRecvDest[minRequest.requestminDest][i]=minRequest.requestminToR;
								break;
							}
						}
						updateAProcess(AProcess, minRequest.requestmin, p, q, N);
						diffTime=twoRecnTime(wssTime,A);
						AProcess[wavelength][whichq][minRequest.requestminToR]=minRequest.flowtime+diffTime;
						AProcess[p][whichq][minRequest.requestminToR]=A;
						
						//判断是否有连接需要拆除 
						if(qSendState[minRequest.requestminToR]==qByN[minRequest.requestminToR] || 
							qRecvState[minRequest.requestminDest]==qByN[minRequest.requestminDest])
						{
							if(qSendState[minRequest.requestminToR]==qByN[minRequest.requestminToR])
								for(i=0;i<qByN[minRequest.requestminToR];i++)
								{
									sumq=processNq[minRequest.requestminToR][i];  
									sumA=AProcess[p][i][minRequest.requestminToR];
									if(sumA==0 && sumq==0)   //找q，看上面是否连着p或者即将重构
									{
										qSendState[minRequest.requestminToR]--;  //释放q  以下并且释放所有和q连着的q
										qRecvState[qDest[minRequest.requestminToR][i]]--;
										for(j=0;j<qByN[qDest[minRequest.requestminToR][i]];j++)
											if(qRecvDest[qDest[minRequest.requestminToR][i]][j]==minRequest.requestminToR)
											{
												qRecvDest[qDest[minRequest.requestminToR][i]][j]=-1;
												break;
											}
										qDest[minRequest.requestminToR][i]=-1;
										break;
									}
								}
							
							
							
							
							
							
							
							
							
							
							if(qRecvState[minRequest.requestminDest]==qByN[minRequest.requestminDest])
							{
								for(i=0;i<qByN[minRequest.requestminDest];i++)//0到(Q-1)
								{
									sourceToR=qRecvDest[minRequest.requestminDest][i];
									for(j=0;j<qByN[sourceToR];j++)
									{
										if(qDest[sourceToR][j]==minRequest.requestminDest)
										{
											sourceq=j;
											break;
										}
									}
									sumq=processNq[sourceToR][sourceq]; 
									sumA=AProcess[p][sourceq][sourceToR];
									if(sumA==0 && sumq==0)
									{
										qSendState[sourceToR]--;
										qRecvState[minRequest.requestminDest]--;
										qDest[sourceToR][sourceq]=-1;
										qRecvDest[minRequest.requestminDest][i]=-1;
										break;
									}
								}	
							}	
						}
					    
						
						
						
						
						
						updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
										 request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
			 		}			 		
		 		















               }
		 		
				
				
				
				
				
				
				
				free(ab);
		 	}
		 	
		
			
			/*for(i=0;i<N;i++)
		 		for(j=0;j<q;j++)
		 		{
	 				printf("%d,%d(%d): ",i,j,processNq[i][j]);
	 				for(k=0;k<processNq[i][j];k++)
	 				{
				 		printf("(%lf,%d) ",qProcess[i][j][k].processTime,qProcess[i][j][k].wavelength);
				 	}
				 	printf("\n");
		 		}
	 		printf("----------------\n");*/
	 	}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		//流传输完最先发生 
else if(minQProcess.qMin<minRequest.requestmin && minQProcess.qMin<minAProcess.AMin)
		{
			/*printf("111111111111: time:%lf, Nq:%d,",minQProcess.qMin,processNq[minQProcess.minTOR][minQProcess.minQ]);
			printf("ToR:%d,Q:%d\n",minQProcess.minTOR,minQProcess.minQ);*/
			pSendState[minQProcess.minTOR][minQProcess.minWavelength]=0;
			pRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]][minQProcess.minWavelength]=0; //发送端口的p波长要与接受端口的p波长相同。
			updateQProcess(newRequest, qProcess, processNq, minQProcess.qMin, N, p, q);
			processNq[minQProcess.minTOR][minQProcess.minQ]--;
			for(i=0;i<processNq[minQProcess.minTOR][minQProcess.minQ];i++)
			{
				if(qProcess[minQProcess.minTOR][minQProcess.minQ][i].processTime==0)
				{
					qProcess[minQProcess.minTOR][minQProcess.minQ][i].processTime
					=qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].processTime;
					
					qProcess[minQProcess.minTOR][minQProcess.minQ][i].wavelength
					=qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].wavelength;
					
					qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].wavelength=-1;
					
					qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].processTime=0;
					
					break;
				}
			}
            sumq=processNq[minQProcess.minTOR][minQProcess.minQ];  //刚刚传输玩流的那个处于发送端tor的q上面的剩余流数量。
			if(sumq==0)                 //当有一个q不活跃了之后
			{
				if(qSendState[minQProcess.minTOR]==qByN[minQProcess.minTOR] || //如果发送端tor上面活跃的q等于tor上面所有的q(qhot或者qcommon个）
					qRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]]==qByN[qDest[minQProcess.minTOR][minQProcess.minQ]])//或者接收端tor上面所有q被占满
				{
					qSendState[minQProcess.minTOR]--;
					qRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]]--;//发送端与接收端都释放一个q
					for(j=0;j<qByN[qDest[minQProcess.minTOR][minQProcess.minQ]];j++) //遍历接收上个流的tor上面的所有q端口
					{
						if(qRecvDest[qDest[minQProcess.minTOR][minQProcess.minQ]][j]==minQProcess.minTOR)//找到接受tor上面的连着发送tor的q端口
						{
							qRecvDest[qDest[minQProcess.minTOR][minQProcess.minQ]][j]=-1; //将这个q从本来连接发送tor端口这个状态释放掉。
							break;
						}
					}
					qDest[minQProcess.minTOR][minQProcess.minQ]=-1; // 将发送tor上面的这个q也释放掉。
				}
			}
			//updateQProcess(newRequest, qProcess, processNq, minQProcess.qMin, N, p, q);
			updateAProcess(AProcess, minQProcess.qMin, p, q, N);
			/*for(i=0;i<N;i++)
		 		for(j=0;j<q;j++)
		 		{
	 				printf("%d,%d(%d): ",i,j,processNq[i][j]);
	 				for(k=0;k<processNq[i][j];k++)
	 				{
				 		printf("(%lf,%d) ",qProcess[i][j][k].processTime,qProcess[i][j][k].wavelength);
				 	}
				 	printf("\n");
		 		}
	 		printf("----------------\n");*/
		}
		     
		
		
		
	else if(minQProcess.qMin==minAProcess.AMin)
		{
			//printf("2222222222222\n");
			pSendState[minQProcess.minTOR][minQProcess.minWavelength]=0;
			pRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]][minQProcess.minWavelength]=0;
			updateQProcess(newRequest, qProcess, processNq, minQProcess.qMin, N, p, q);
			processNq[minQProcess.minTOR][minQProcess.minQ]--;
			for(i=0;i<processNq[minQProcess.minTOR][minQProcess.minQ];i++)
			{
				if(qProcess[minQProcess.minTOR][minQProcess.minQ][i].processTime==0)
				{
					qProcess[minQProcess.minTOR][minQProcess.minQ][i].processTime
					=qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].processTime;
					qProcess[minQProcess.minTOR][minQProcess.minQ][i].wavelength
					=qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].wavelength;
					qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].wavelength=-1;
					qProcess[minQProcess.minTOR][minQProcess.minQ][processNq[minQProcess.minTOR][minQProcess.minQ]].processTime=0;
					break;
				}
			}
			
			for(i=0;i<p;i++)
			{
				if(AProcess[i][minAProcess.Aq][minAProcess.AToR]>0)
				{
					qProcess[minAProcess.AToR][minAProcess.Aq][processNq[minAProcess.AToR][minAProcess.Aq]].processTime
	                                                                     =AProcess[i][minAProcess.Aq][minAProcess.AToR];
                    qProcess[minAProcess.AToR][minAProcess.Aq][processNq[minAProcess.AToR][minAProcess.Aq]].wavelength=i;
                    processNq[minAProcess.AToR][minAProcess.Aq]++;
					AProcess[i][minAProcess.Aq][minAProcess.AToR]=0;
				}
			}
			AProcess[p][minAProcess.Aq][minAProcess.AToR]=0;
			
			sumq=processNq[minQProcess.minTOR][minQProcess.minQ];
			if(sumq==0)
			{
				if(qSendState[minQProcess.minTOR]==qByN[minQProcess.minTOR] || 
					qRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]]==qByN[qDest[minQProcess.minTOR][minQProcess.minQ]])
				{
					qSendState[minQProcess.minTOR]--;
					qRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]]--;
					for(i=0;i<qByN[qDest[minQProcess.minTOR][minQProcess.minQ]];i++)
					{
						if(qRecvDest[qDest[minQProcess.minTOR][minQProcess.minQ]][i]==minQProcess.minTOR)
						{
							qRecvDest[qDest[minQProcess.minTOR][minQProcess.minQ]][i]=-1;
							break;
						}	
					}
				}
			}
			updateAProcess(AProcess, minAProcess.AMin, p, q, N);
			/*for(i=0;i<N;i++)
		 		for(j=0;j<q;j++)
		 		{
	 				printf("%d,%d(%d): ",i,j,processNq[i][j]);
	 				for(k=0;k<processNq[i][j];k++)
	 				{
				 		printf("(%lf,%d) ",qProcess[i][j][k].processTime,qProcess[i][j][k].wavelength);
				 	}
				 	printf("\n");
		 		}
	 		printf("----------------\n");*/
		}
		          

         else if(minAProcess.AMin<minQProcess.qMin && minAProcess.AMin<minRequest.requestmin)//更新qprocess,minrequset
		{
			/*printf("333333333333333: time:%f, Nq:%d",minAProcess.AMin,processNq[minAProcess.AToR][minAProcess.Aq]);
			printf(",ToR:%d,q:%d\n",minAProcess.AToR,minAProcess.Aq);*/
			updateQProcess(newRequest, qProcess, processNq, minAProcess.AMin, N, p, q);
			for(i=0;i<p;i++)
			{
				if(AProcess[i][minAProcess.Aq][minAProcess.AToR]>0) //新建了一个传输过程。
				{
					//printf("///:%lf\n",AProcess[i][minAProcess.Aq][minAProcess.AToR]);
					qProcess[minAProcess.AToR][minAProcess.Aq][processNq[minAProcess.AToR][minAProcess.Aq]].processTime
	                                                                     =AProcess[i][minAProcess.Aq][minAProcess.AToR];
                    qProcess[minAProcess.AToR][minAProcess.Aq][processNq[minAProcess.AToR][minAProcess.Aq]].wavelength=i;
                    processNq[minAProcess.AToR][minAProcess.Aq]++;
					AProcess[i][minAProcess.Aq][minAProcess.AToR]=0;
				}
			}
			AProcess[p][minAProcess.Aq][minAProcess.AToR]=0;
			//printf("333333333333333 %d\n",processNq[minAProcess.AToR][minAProcess.Aq]);
			updateAProcess(AProcess, minAProcess.AMin, p, q, N);
			/*for(i=0;i<N;i++)
		 		for(j=0;j<q;j++)
		 		{
	 				printf("%d,%d(%d): ",i,j,processNq[i][j]);
	 				for(k=0;k<processNq[i][j];k++)
	 				{
				 		printf("(%lf,%d) ",qProcess[i][j][k].processTime,qProcess[i][j][k].wavelength);
				 	}
				 	printf("\n");
		 		}
	 		printf("----------------\n");*/	
		}
		
		 
		 else
		{
			printf("出现其他情况\n");
		}	 	
    }
    
	
	//释放申请的内存空间 
	for(i=0;i<N;i++)
	{
		free(newRequest[i]);
		free(processNq[i]);
	}
	free(newRequest);
	free(processNq);
	
	for(i=0;i<p+1;i++)
	{
		for(j=0;j<q;j++)
		{
			free(AProcess[i][j]);
		}
		free(AProcess[i]);
	} 
	free(AProcess);
	
	for(i=0;i<N;i++)
	{
		for(j=0;j<q;j++)
			free(qProcess[i][j]);
		free(qProcess[i]);
	}
	free(qProcess);
	 
	blockingProbablity=(double)sumMatrix(block,N)/sumMatrix(accumlate,N); 
	return blockingProbablity;	
}


