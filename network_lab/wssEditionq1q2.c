//��������������������ʵĺ��ĺ��� ���ȵ�

#include "wssFunctionsq1q2.c"
       //        ������   ���ȵ�tor�ϵ�q  �ȵ���    �ȵ��� һ��ʱ����������Ҫ���������   ���ĸ���      TOR��   ocstime
double wssEdition(int p, int qcommom, int qhot, int hotN, double*** request3d,         int totalNum, int N, double A, double wssTime)
{
	//���������qhot>qcommon
	int q=qhot>qcommom?qhot:qcommom;
	struct requestMin minRequest;
	struct qProcessMin minQProcess;
	struct AProcessMin minAProcess;   //��¼��ʣ����ʱ�䣬һ���µ�����ʼ���䡣
	int pSendState[N][p]; //���Ͷ�ToR��p������ռ����� 
	int pRecvState[N][p]; //���ܶ�ToR��p������ռ����� 
	int qSendState[N];    //���Ͷ�ToR��q���˿�ռ����� 
	int qRecvState[N];    //���ն�ToR��q���˿�ռ����� 
	double*** AProcess;   //�ع��׶�״̬                                      [(p+1)*q*N] 
	struct qProcessStruct*** qProcess;	//���ͽ׶�״̬                        [N*q*p]
	int qDest[N][q];             		//������ToR��q���˿�������ToR 
	int qRecvDest[N][q];         		//������ToR��q���˿�������ToR
	int accumlate[N];            		//��ToR���������ۼ�
	int block[N];                		//��ToR�������������ۼ�
	int blockPQ[2];              		//������ɵ�ԭ�����������p����q  ����p��qͬʱ
	int** processNq;                    //��¼N q�ϵ�������                                ��N*q��//һ��Q�Ͽ����ж����
	double** newRequest;         		//��һ�ν���ϵͳ������                             [N*3]
	int i,j,k,count=0;                  //ѭ��������һ��ֱ�ָһά����ά����ά 
	int wavelength,judgeSameToR,multiq; //����,�ж����� 
	int zeroTag,whichq,sourceToR,sourceq;//Ѱ�ҿ��ò���, ���е�q,�ж������Ƿ�ɲ�� 
	double diffTime,sumA,sumq;			//ʵ���ع�ʱ��,�ж������Ƿ���ʱ���� 
	double blockingProbablity;          //���յ������� 
	int* ab;                            //���������ۼӺ� 
	int qByN[N];                        //��Ÿ���ToR�ϵ�q������ 

	//�����϶��������ָ������ڴ� 
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
	
	//�����϶�������������ָ��ĳ�ʼ��
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
  			newRequest[k][j]=request3d[0][j][k];  //��0������
  			
	for(i=0;i<hotN;i++)
		qByN[i]=qhot;
	for(i=hotN;i<N;i++)
		qByN[i]=qcommom;
	
/*	for(i=0;i<N;i++)
		printf("%d ",qByN[i]);   ʮ��������
	printf("\n");              �س�����*/

	//�Ե�һ�������״̬���� 
	minRequest=findRequestMin(newRequest,N);
	accumlate[minRequest.requestminToR]++;
	
	wavelength=0;
	
	pSendState[minRequest.requestminToR][wavelength]=1;//[x][0]=1 //���Ͷ�ToR��p������ռ����� 
	pRecvState[minRequest.requestminDest][wavelength]=1;//[y][0]=1  ���ܶ�ToR��p������ռ�����
	qSendState[minRequest.requestminToR]=1;     //���Ͷ�ToR��q���˿�ռ�����
	qRecvState[minRequest.requestminDest]=1;   //���ն�ToR��q���˿�ռ�����
	
	diffTime=twoRecnTime(wssTime,A);            // Ϊʲô����0
	
	AProcess[wavelength][0][minRequest.requestminToR]=minRequest.flowtime+diffTime;//wss��Ҫ�Ķ����ع�ʱ����ϴ���ʱ�䡣
	AProcess[p][0][minRequest.requestminToR]=A; //����ɴ�����N��tor��0��p�˿ڻ���ҪAʱ���ع���
	
	qDest[minRequest.requestminToR][0]=minRequest.requestminDest;//������ToR��q���˿�������ToR 
	qRecvDest[minRequest.requestminDest][0]=minRequest.requestminToR;	//������ToR��q���˿�������ToR ÿ������tor�ϵ�Q��һ������TOR�ϵ�Q������
	
	updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									    request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
 
 	
	/*�ҳ���ʱ���ٵ������ڷ����������tor��+1
	  ����=0����Ϊ�ǵ�һ������
	  �޸�pqռ�����������p, ��Ƿ��͵ĵ�n��tor ��p������������һ��Ϊ1�����ڵ�һ��
	  ����ǵ�0������Ϊ1�� ����qֱ�Ӱѵ�n��tor ����ʹ�õ�qֵ��¼���ɡ�
	  
	  �����ع��׶�״̬����¼������ʱ����ع�ʱ�䣨���ʱ������ÿ��tor�ϵ�pq���ӣ�������ָ���ʾ����
	                ��Ҫ��¼�޹�p,ֻ��tor,q�йص��ع�ʱ�䡣
      
	  ��¼������ToR��q���˿�������ToR �� ������ToR��q���˿�������ToR �ֱ���N*q�Ķ�ά�����ʾ��

	   ���·��ͽ׶�״̬��newrequst�м��ÿ�������ʱ�䶼��ȥ�Ѿ����ĵ�ʱ�䡣װ���µ�����
	                    ���Ҷ���ÿһ��tor �ϵ�ÿһ��ʹ�õ�p, ���䡰����ʱ�䡱��ȥ�Ѿ����ĵ�ʱ�䡣*/
	
	
	
	
	
	
	
	
	
	
	
	
	//ѭ���������� 
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
	 	
	 	//�µ����������ȷ��� 
	 	if(minRequest.requestmin<=minQProcess.qMin && minRequest.requestmin<=minAProcess.AMin)
	 	{
	 		/*printf("000000000000: ");
	 		printf("time:%lf,ToR:%d\n",minRequest.requestmin,minRequest.requestminToR);*/
	 		accumlate[minRequest.requestminToR]++;
	 		//û�п��в������� ���������µ�n����������ڴ���Ĺ��̣���ȥ��һ�������ʱ�䣩������Aprocess��ÿ��TOR�ϵ�ÿ��q����ȥ�Ѿ����ŵ�ʱ�䣩
	 		if(sumMatrix(pSendState[minRequest.requestminToR],p)==p || sumMatrix(pRecvState[minRequest.requestminDest],p)==p)
	 		{
		 		block[minRequest.requestminToR]++;
		 		blockPQ[0]++;
		 		updateRequestProcess(newRequest, wavelength, qProcess, processNq, minRequest.requestminToR,
									 request3d, accumlate[minRequest.requestminToR], N, p, q, -1);
                updateAProcess(AProcess, minRequest.requestmin, p, q, N);
		 	}
		 	//û�п��е�q�˿� �����ǿ��Ը���q�����������ʧ�ܣ���������ʱ�䣬����ʱ���Aprocess)
		 	else if(qSendState[minRequest.requestminToR]==qByN[minRequest.requestminToR] || 
 					qRecvState[minRequest.requestminDest]==qByN[minRequest.requestminDest])
		 	{
	 			judgeSameToR=0;  //�����Ѿ����ڵ�����ʱ��judgesametor��Ϊ1 multiq��Ϊ�Ѿ���������״̬�ķ��Ͷˣ�q'��ֵ
			    for(i=0;i<qByN[minRequest.requestminToR];i++)
				{
					if(qDest[minRequest.requestminToR][i]==minRequest.requestminDest)
					{
						judgeSameToR=1;
						multiq=i;
						break;
					}
				}
				//�������� 
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
				//����ͬĿ�ĵص����� //��������ʱ�䣬����aprocessʱ��
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
                		processNq[minRequest.requestminToR][multiq]++;//������7��p������һ�����д���������ʹ�õ�ʱ����6���档
					}
					else //��Ȼ�ҵ���Ҫ�ع���p���������ع���
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
	 		    
			
			
			
			
			
			
			
			
			
			
	           //�����ȵ�����ֶ˿ڶ�û��ռ�� 
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
						
						//�ж��Ƿ���������Ҫ��� 
						if(qSendState[minRequest.requestminToR]==qByN[minRequest.requestminToR] || 
							qRecvState[minRequest.requestminDest]==qByN[minRequest.requestminDest])
						{
							if(qSendState[minRequest.requestminToR]==qByN[minRequest.requestminToR])
								for(i=0;i<qByN[minRequest.requestminToR];i++)
								{
									sumq=processNq[minRequest.requestminToR][i];  
									sumA=AProcess[p][i][minRequest.requestminToR];
									if(sumA==0 && sumq==0)   //��q���������Ƿ�����p���߼����ع�
									{
										qSendState[minRequest.requestminToR]--;  //�ͷ�q  ���²����ͷ����к�q���ŵ�q
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
								for(i=0;i<qByN[minRequest.requestminDest];i++)//0��(Q-1)
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
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		//�����������ȷ��� 
else if(minQProcess.qMin<minRequest.requestmin && minQProcess.qMin<minAProcess.AMin)
		{
			/*printf("111111111111: time:%lf, Nq:%d,",minQProcess.qMin,processNq[minQProcess.minTOR][minQProcess.minQ]);
			printf("ToR:%d,Q:%d\n",minQProcess.minTOR,minQProcess.minQ);*/
			pSendState[minQProcess.minTOR][minQProcess.minWavelength]=0;
			pRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]][minQProcess.minWavelength]=0; //���Ͷ˿ڵ�p����Ҫ����ܶ˿ڵ�p������ͬ��
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
            sumq=processNq[minQProcess.minTOR][minQProcess.minQ];  //�ոմ����������Ǹ����ڷ��Ͷ�tor��q�����ʣ����������
			if(sumq==0)                 //����һ��q����Ծ��֮��
			{
				if(qSendState[minQProcess.minTOR]==qByN[minQProcess.minTOR] || //������Ͷ�tor�����Ծ��q����tor�������е�q(qhot����qcommon����
					qRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]]==qByN[qDest[minQProcess.minTOR][minQProcess.minQ]])//���߽��ն�tor��������q��ռ��
				{
					qSendState[minQProcess.minTOR]--;
					qRecvState[qDest[minQProcess.minTOR][minQProcess.minQ]]--;//���Ͷ�����ն˶��ͷ�һ��q
					for(j=0;j<qByN[qDest[minQProcess.minTOR][minQProcess.minQ]];j++) //���������ϸ�����tor���������q�˿�
					{
						if(qRecvDest[qDest[minQProcess.minTOR][minQProcess.minQ]][j]==minQProcess.minTOR)//�ҵ�����tor��������ŷ���tor��q�˿�
						{
							qRecvDest[qDest[minQProcess.minTOR][minQProcess.minQ]][j]=-1; //�����q�ӱ������ӷ���tor�˿����״̬�ͷŵ���
							break;
						}
					}
					qDest[minQProcess.minTOR][minQProcess.minQ]=-1; // ������tor��������qҲ�ͷŵ���
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
		          

         else if(minAProcess.AMin<minQProcess.qMin && minAProcess.AMin<minRequest.requestmin)//����qprocess,minrequset
		{
			/*printf("333333333333333: time:%f, Nq:%d",minAProcess.AMin,processNq[minAProcess.AToR][minAProcess.Aq]);
			printf(",ToR:%d,q:%d\n",minAProcess.AToR,minAProcess.Aq);*/
			updateQProcess(newRequest, qProcess, processNq, minAProcess.AMin, N, p, q);
			for(i=0;i<p;i++)
			{
				if(AProcess[i][minAProcess.Aq][minAProcess.AToR]>0) //�½���һ��������̡�
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
			printf("�����������\n");
		}	 	
    }
    
	
	//�ͷ�������ڴ�ռ� 
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


