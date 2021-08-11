#include <stdio.h>
#include <stdlib.h>

//链表数据类型


typedef struct linklist
{
    int element;
    struct linklist *next;
}linklist;


/*************************************链表初始化*******************************

功能描述:初始化链表，创建用于指向一个新链表的头指针
******************************************************************************/

linklist* listInit()
{
    linklist *headNode = (linklist*)malloc(sizeof(linklist));

    if(headNode == NULL)
    {
        printf("内存不足!!!");
        return headNode;
    }

    headNode->element = 0;
    headNode->next = NULL;
    
    return headNode;
}

/******************************************************************************
函数名称：createLinkList
功能描述：创建一个链表，以数组形式赋值
参数说明：linklist *headNode 头指针 int *inData 数组数据，存入链表  
          int dataNum 数组大小

******************************************************************************/

int creatLinkList(linklist *headNode, int *inData, int dataNum)
{
    int i;
    linklist *currentNode;
    
    currentNode = (linklist*)headNode;
    for(i = 0; i < dataNum; ++i)
    {
        currentNode->element = inData[i];
        if(i < dataNum - 1)
        {
            currentNode->next = (linklist*)malloc(sizeof(linklist));
            currentNode = currentNode->next;
        }

        currentNode->next = NULL;
    }

    return 0;
}

/******************************************************************************
函数名称：insertList
功能描述：向指定位置前插入节点
参数说明：linklist *headNode 头指针 int inData 需要插入的数据  
          int locateIndex 插入节点

******************************************************************************/

int insertList(linklist *headNode,int locateIndex,int inData)

{
    int i = 1;// 由于起始结点HeadNode是头结点，所以计数从1开始

    linklist *currentNode = (linklist *) headNode;

    //将CurrentNode指向待插入位置的前一个结点（index -1）

    while(currentNode && (i < locateIndex-1))

    {
        currentNode = currentNode->next;

        i++;

    }

    linklist *nodeToInsert = (linklist*)malloc(sizeof(linklist));

    if(nodeToInsert == NULL)

    {
        printf("空间缓存不足");
        return 0;

    }

    nodeToInsert->element = inData;

    nodeToInsert->next = currentNode->next;

    currentNode->next = nodeToInsert;
    
    return 1;

}


/******************************************************************************
函数名称：deleteList
功能描述：向指定位置前插入节点
参数说明：linklist *headNode 头指针 int index 需要插入的数据  
          int locateIndex 插入节点

******************************************************************************/



int deleteList(linklist *headNode,int index, int *dataToDel)

{
    int i = 1;

    linklist *currentNode  = headNode; 

    linklist *nodeToDelete;

    //将CurrentNode指向待删除位置的前一个结点（index -1）

    while(currentNode && (i < index - 1))

    {
        currentNode = currentNode->next;

        i++;

    }

    nodeToDelete      = currentNode->next;

    *dataToDel        = nodeToDelete->element;

    currentNode->next = nodeToDelete->next;

    free(nodeToDelete);

    return 1;
}


int main()
{
    return 0;
}


