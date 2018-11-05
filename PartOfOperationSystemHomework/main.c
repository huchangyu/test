#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define maxsize 1000  //定义原始分配区的大小。

struct map{
    unsigned m_size;  //本表项所指空闲区的大小。
    char* m_addr;    //本表项所指空闲区的首地址。
    struct map *next,*prior;  //本表项的前后表项。
};
struct map *coremap,*start;  //coremap指向首表项，start指向当前的查找首表项。

char * mmalloc(unsigned size){  //分配空闲区的函数。
    struct map *current=start; //记录查找的起点。
    char * c;
    do{
        if(start->m_size>size){ //有足够大的空闲区，有余。
             start->m_size=start->m_size-size; //减小分配过的表项空间。
             c=start->m_addr; 
             start->m_addr+=size; //修改表项的首地址。
             return c;
        }
        else if(start->m_size==size){ //有正好大小的空闲区。
             start->next->prior=start->prior; // 从链表中删除该表项。
             start->prior->next=start->next; // 从链表中删除该表项。
             start->m_size=0;
             return start->m_addr; 
        }
        else start=start->next; //当前表项所指的空闲区不够，start指向下一个表项。
     }while(start!=current); // 一直循环查找表项，直到回到起点。
     return NULL;     //没有找到合适大小的分配区，分配失败。
}

int mfree(unsigned size,char * addr){ //释放的函数。
     struct map *current=coremap; //
     struct map *temp;
     //若释放的空间在循环链表的首表项的前头，则要插入首表项。
     if(coremap->m_addr>addr && ((coremap->m_addr-addr)>(signed) size)){
   temp=(struct map *)malloc(sizeof(struct map)); //新建首表项。
   temp->m_addr=addr;
   temp->m_size=size;
   temp->next=coremap;
   temp->prior=coremap->prior;
   coremap->prior->next=temp;//插入循环链表
   coremap->prior=temp;
   coremap=coremap->prior;
   return 1;//释放成功。
  }
     //若释放的空间紧挨着循环链表的首表项的前头，则要修改首表项。
     else if(coremap->m_addr>addr && (coremap->m_addr-addr==size)){
   coremap->m_addr=addr;//修改首表项的首地址。
   coremap->m_size+=size;
   return 1;//释放成功。
  }
     //若释放的空间在循环链表的首表项的后头，则有四种情况：
  do{
        //若释放的空间不紧挨着别的空闲区的边界。
        if((current->m_addr+current->m_size<addr )&& (current->next->m_addr>addr+size)){
             temp=current->next;
             current->next=(struct map * )malloc(sizeof (struct map));
             current->next->prior=current;
             current->next->next=temp;
             current->next->m_size=size; 
    current->next->m_addr=addr;
    temp->prior=current->next;
             return 1; //释放成功。
        }
        //若释放的空间紧挨着上边空闲区的边界，不紧挨着下边空闲区的边界。
        else if((current->m_addr+current->m_size==addr) && (current->next->m_addr>addr+size)){
             current->m_size+=size;
             return 1; //释放成功。
        }
        //若释放的空间不紧挨着上边空闲区的边界，紧挨着下边空闲区的边界。
        else if((current->m_addr+current->m_size<addr) && (current->next->m_addr==addr+size)){
             current->next->m_addr=addr;
             current->next->m_size+=size;
             return 1; //释放成功。
        }
        //若释放的空间紧挨着上边空闲区的边界，紧挨着下边空闲区的边界。
        else if((current->m_addr+current->m_size==addr) && (current->next->m_addr==addr+size)){
             current->m_size+=(size+current->next->m_size);
             current->next->next->prior=current;
             current->next=current->next->next;
             return 1; //释放成功。
        } 
        else current=current->next; //释放的空间不在当前表项和下一表项间。
     }while(current!=coremap);//一直寻找，直到回到起点。
     return 0; //没有匹配的空间可以释放，释放失败。
}

void printall(){//打印当前的所有表项。
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
 coremap=(struct map *) malloc(sizeof(struct map));//创建循环链表的首表项。
    coremap->m_addr=(char * )malloc(maxsize);//分配给首表项原始的一片空间。
    coremap->m_size=maxsize;
 coremap->prior=coremap->next=coremap;//循环关系。
 start=coremap;//设置当前指针。
    do{
    printall();
    printf("  What area do you want to malloc or free? m or f  ");
    scanf("%s",&c);   
    if(c=='m') {//分配空间。
        scanf("%u",&size);
        if(!mmalloc(size)) printf("Sorry!There's no area for this size what can be malloced.");
    }
    else if(c=='f') {//释放空间。
        scanf("%u,%u",&size,&addr);
        if(mfree(size,addr)!=1) printf("Sorry!There's no area which you can free.");
    }
    else break;//其他字母推出。
    }while(c=='m' || c=='f'); 
} 
