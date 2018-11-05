#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define maxsize 1000  //����ԭʼ�������Ĵ�С��

struct map{
    unsigned m_size;  //��������ָ�������Ĵ�С��
    char* m_addr;    //��������ָ���������׵�ַ��
    struct map *next,*prior;  //�������ǰ����
};
struct map *coremap,*start;  //coremapָ���ױ��startָ��ǰ�Ĳ����ױ��

char * mmalloc(unsigned size){  //����������ĺ�����
    struct map *current=start; //��¼���ҵ���㡣
    char * c;
    do{
        if(start->m_size>size){ //���㹻��Ŀ����������ࡣ
             start->m_size=start->m_size-size; //��С������ı���ռ䡣
             c=start->m_addr; 
             start->m_addr+=size; //�޸ı�����׵�ַ��
             return c;
        }
        else if(start->m_size==size){ //�����ô�С�Ŀ�������
             start->next->prior=start->prior; // ��������ɾ���ñ��
             start->prior->next=start->next; // ��������ɾ���ñ��
             start->m_size=0;
             return start->m_addr; 
        }
        else start=start->next; //��ǰ������ָ�Ŀ�����������startָ����һ�����
     }while(start!=current); // һֱѭ�����ұ��ֱ���ص���㡣
     return NULL;     //û���ҵ����ʴ�С�ķ�����������ʧ�ܡ�
}

int mfree(unsigned size,char * addr){ //�ͷŵĺ�����
     struct map *current=coremap; //
     struct map *temp;
     //���ͷŵĿռ���ѭ��������ױ����ǰͷ����Ҫ�����ױ��
     if(coremap->m_addr>addr && ((coremap->m_addr-addr)>(signed) size)){
   temp=(struct map *)malloc(sizeof(struct map)); //�½��ױ��
   temp->m_addr=addr;
   temp->m_size=size;
   temp->next=coremap;
   temp->prior=coremap->prior;
   coremap->prior->next=temp;//����ѭ������
   coremap->prior=temp;
   coremap=coremap->prior;
   return 1;//�ͷųɹ���
  }
     //���ͷŵĿռ������ѭ��������ױ����ǰͷ����Ҫ�޸��ױ��
     else if(coremap->m_addr>addr && (coremap->m_addr-addr==size)){
   coremap->m_addr=addr;//�޸��ױ�����׵�ַ��
   coremap->m_size+=size;
   return 1;//�ͷųɹ���
  }
     //���ͷŵĿռ���ѭ��������ױ���ĺ�ͷ���������������
  do{
        //���ͷŵĿռ䲻�����ű�Ŀ������ı߽硣
        if((current->m_addr+current->m_size<addr )&& (current->next->m_addr>addr+size)){
             temp=current->next;
             current->next=(struct map * )malloc(sizeof (struct map));
             current->next->prior=current;
             current->next->next=temp;
             current->next->m_size=size; 
    current->next->m_addr=addr;
    temp->prior=current->next;
             return 1; //�ͷųɹ���
        }
        //���ͷŵĿռ�������ϱ߿������ı߽磬���������±߿������ı߽硣
        else if((current->m_addr+current->m_size==addr) && (current->next->m_addr>addr+size)){
             current->m_size+=size;
             return 1; //�ͷųɹ���
        }
        //���ͷŵĿռ䲻�������ϱ߿������ı߽磬�������±߿������ı߽硣
        else if((current->m_addr+current->m_size<addr) && (current->next->m_addr==addr+size)){
             current->next->m_addr=addr;
             current->next->m_size+=size;
             return 1; //�ͷųɹ���
        }
        //���ͷŵĿռ�������ϱ߿������ı߽磬�������±߿������ı߽硣
        else if((current->m_addr+current->m_size==addr) && (current->next->m_addr==addr+size)){
             current->m_size+=(size+current->next->m_size);
             current->next->next->prior=current;
             current->next=current->next->next;
             return 1; //�ͷųɹ���
        } 
        else current=current->next; //�ͷŵĿռ䲻�ڵ�ǰ�������һ����䡣
     }while(current!=coremap);//һֱѰ�ң�ֱ���ص���㡣
     return 0; //û��ƥ��Ŀռ�����ͷţ��ͷ�ʧ�ܡ�
}

void printall(){//��ӡ��ǰ�����б��
     struct map * temp=coremap;
  printf("/n");
     do{
        printf("%u,%u;",temp->m_addr,temp->m_size);
        temp=temp->next;
     }while(temp!=coremap);
}

main(){
    char c;
    char *addr;
    unsigned size;
    printf("Welcome to use my operation system!");
 coremap=(struct map *) malloc(sizeof(struct map));//����ѭ��������ױ��
    coremap->m_addr=(char * )malloc(maxsize);//������ױ���ԭʼ��һƬ�ռ䡣
    coremap->m_size=maxsize;
 coremap->prior=coremap->next=coremap;//ѭ����ϵ��
 start=coremap;//���õ�ǰָ�롣
    do{
    printall();
    printf("  What area do you want to malloc or free? m or f  ");
    scanf("%s",&c);   
    if(c=='m') {//����ռ䡣
        scanf("%u",&size);
        if(!mmalloc(size)) printf("Sorry!There's no area for this size what can be malloced.");
    }
    else if(c=='f') {//�ͷſռ䡣
        scanf("%u,%u",&size,&addr);
        if(mfree(size,addr)!=1) printf("Sorry!There's no area which you can free.");
    }
    else break;//������ĸ�Ƴ���
    }while(c=='m' || c=='f'); 
} 
