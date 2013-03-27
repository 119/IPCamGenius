#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * 集合操作有 并，交，差，对称差
 *
 * 编程所实现的集合是有穷集合
 *
 * 集合元素可以为任意类型：比如数值型(整型，浮点型都看成数值型)，字符串型(字符型也可以看成字符串型来处理)
 *
 * 集合通过单链表来表示，链表为空NULL表示空集合
 *
 * 链表节点存储集合元素
 *
 *
 */

/*
 * 程序运行效果
 *
 请输入集合A：
请输入此集合元素个数:  	3
第1个元素 [类型,值] 【0为整型,1为字符串型】
0
12
第2个元素 [类型,值] 【0为整型,1为字符串型】
0
10
第3个元素 [类型,值] 【0为整型,1为字符串型】
1
lixiaocong
请输入集合B：
请输入此集合元素个数:  	3
第1个元素 [类型,值] 【0为整型,1为字符串型】
0
12
第2个元素 [类型,值] 【0为整型,1为字符串型】
1
xiaocong
第3个元素 [类型,值] 【0为整型,1为字符串型】
1
lixiaocong
请输入集合A与B运算方式: 1代表求并集，2代表交集，3代表求差集，4代表求对称差集
5

集合A :
10.000000  12.000000  'lixiaocong'
集合B :
12.000000  'lixiaocong'  'xiaocong'
集合求并 结果集合 :
10.000000  12.000000  'lixiaocong'  'xiaocong'
集合求交 结果集合 :
12.000000  'lixiaocong'
集合求差 结果集合 :
10.000000
集合求对称差 结果集合 :
10.000000  'xiaocong'
结果集合C :

是否继续集合运算，输入大于0的数，继续


 ** */
typedef enum  //集合元素类型
{
	NUMBER,	 //数值型
	STRING   //字符串型
}ElementType;

typedef union  //集合元素值，数值型统一用双精度来表示，字符串用指针来指向字符串
{
	int numberdata;
	char   *stringdata;
}ElementData;

/*
 * 链表节点类型
 */
typedef struct node
{
	ElementType type;               //元素类型

	ElementData data;

	struct node *next;
}Node;

/*
 *集合操作函数定义
 */

Node *set_union(Node *A,Node *B);  //集合A,B求并集

Node *set_intersection(Node *A,Node *B); //集合求交集

Node *set_difference(Node *A,Node *B);//集合求差集

Node *set_symmetric_difference(Node *A,Node *B);//集合求对称差集

Node *set_sort(Node *A);//对集合按元素进行排序，以便于集合操作。

/*
 * 创建集合链表
 */
Node *createSetLink(char *experssion);

void printSetLink(Node *C);//输出集合的元素

Node *nodeCopy(Node *node);

Node *linkCopy(Node *link);

void FreeLink(Node *link);

int NodeCmp(Node *node1,Node *node2);

Node *linkCopy(Node *link)
{
	Node *newlink=NULL;
	Node *node,*prenode;

	if(link == NULL)
	{
		return NULL;
	}

	while(link != NULL)
	{
		node=nodeCopy(link);
		if(newlink == NULL)
		{
			newlink=node;
		}
		else{
			prenode->next=node;
		}
		prenode=node;

		link=link->next;
	}

	return newlink;
}

Node *nodeCopy(Node *node)
{
	Node *newnode=NULL;

	if(node == NULL)
	{
		return NULL;
	}

	newnode=(Node *)calloc(1,sizeof(Node));

	newnode->type=node->type;
	newnode->next=node->next;

	if(node->type == NUMBER)
	{
		newnode->data.numberdata=node->data.numberdata;
	}
	else
	{
		newnode->data.stringdata=(char *)calloc(strlen(node->data.stringdata)+1,sizeof(char));
		strcpy(newnode->data.stringdata,node->data.stringdata);
	}

	return newnode;
}

void FreeLink(Node *link)
{
	Node *node;

	if(link == NULL)
	{
		return ;
	}

	while(link != NULL)
	{
		node=link;
		link=link->next;

		if(node->type == STRING)
		{
			if(node->data.stringdata)
			{
				free(node->data.stringdata);
			}
		}
		free(node);
	}
}

int checkRepeat(Node *link,Node *node)
{
	Node *curnode;

	curnode=link;

	while(curnode != NULL)
	{
		if(NodeCmp(curnode,node) == 0)
		{
			return 1;
		}
		curnode=curnode->next;
	}

	return 0;
}

Node *createSetLink(char *experssion)
{
	Node *link=NULL,*node,*prenode;
	int  i;
	int  type=0;//0为整形,1为字符串
	int  n=8500;


		for(i=8000;i<n;i++)
		{
			node=(Node *)calloc(1,sizeof(Node));

			node->data.numberdata=i;


			node->type=(ElementType)type;
			node->next=NULL;

			if(link == NULL)
			{
				link=node;
			}
			else
			{
				prenode->next=node;
			}
			prenode=node;

			
		}

	return link;
}

Node *insertCreateSetLink(const int *port)
{
	Node *link=NULL,*node,*prenode;
	int  i=0;
	int  type=0;//0为整形,1为字符串
	int  n=8500;

	while (port[i]!=NULL)
	{
		node=(Node *)calloc(1,sizeof(Node));

		node->data.numberdata=port[i];


		node->type=(ElementType)type;
		node->next=NULL;

		if(link == NULL)
		{
			link=node;
		}
		else
		{
			prenode->next=node;
		}
		prenode=node;

		i++;

	}


	return link;
}





void printSetLink(Node *C)
{
	Node *node=C;

	while(node != NULL)
	{
		switch(node->type)
		{
			case NUMBER:printf("%d  ",node->data.numberdata);fflush(stdout);break;
			case STRING:printf("'%s'  ",node->data.stringdata);fflush(stdout);break;
		}

		node=node->next;
	}
}

int NodeCmp(Node *node1,Node *node2)
{
	if(node1 == NULL || node2 == NULL)
	{
		if(node1 == node2)
		{
			return 0;
		}
		else if(node1 == NULL)
		{
			return -1;
		}
		else return 1;
	}

	if(node1->type != node2->type)
	{
		return node1->type-node2->type;
	}

	if(node1->type == NUMBER)
	{
		return node1->data.numberdata-node2->data.numberdata;
	}
	else if(node1->type == STRING)
	{
		return strcmp(node1->data.stringdata,node2->data.stringdata);
	}

	return 1;
}

/*
 * 对集合元素排序,对于数值型的按照升序排列，字符串型的按照strcmp比较结果升序排列
 * 采用插入法进行排序
 */
Node *set_sort(Node *A)
{
	Node *link=NULL;
	Node *linknode,*prelinknode;
	Node *node,*temp;

	if(A == NULL) return NULL;

	link=node=A;

	node=node->next;
	link->next=NULL;

	while(node != NULL)
	{
		prelinknode=linknode=link;
		temp=node->next;

		while(linknode != NULL)
		{
			if(NodeCmp(node,linknode) < 0)
			{
				break;
			}

			prelinknode=linknode;
			linknode=linknode->next;
		}

		if(prelinknode==linknode)
		{
			node->next=link;
			link=node;
		}
		else
		{
			prelinknode->next=node;
			node->next=linknode;
		}

		node=temp;
	}

	return link;
}

Node *set_union(Node *A,Node *B)
{
	Node *link=NULL,*node,*prenode=NULL;
	Node *node1,*node2;
	int cmp;

	if(A==NULL && B==NULL)
	{
		return NULL;
	}

	node1=A;
	node2=B;

	while(node1!=NULL && node2!=NULL)
	{
		cmp=NodeCmp(node1,node2);

		if(cmp < 0)
		{
			node=nodeCopy(node1);
			node1=node1->next;
		}
		else if(cmp > 0)
		{
			node=nodeCopy(node2);
			node2=node2->next;
		}
		else {
			node=nodeCopy(node1);
			node1=node1->next;
			node2=node2->next;
		}

		if(link == NULL)
		{
			link=node;
		}
		else
		{
			prenode->next=node;
		}

		prenode=node;
	}

	node=node1;
	if(node1 == NULL)
	{
		node=node2;
	}

	node=linkCopy(node);

	if(prenode == NULL)
	{
		link=node;
	}
	else
	{
		prenode->next=node;
	}

	return link;
}

Node *set_intersection(Node *A,Node *B)
{
	Node *link=NULL,*prenode=NULL,*node=NULL;
	Node *node1,*node2;
	int cmp;

	if(A == NULL || B==NULL)
	{
		return NULL;
	}

	node1=A;
	node2=B;

	while(node1 != NULL && node2 != NULL)
	{
		cmp=NodeCmp(node1,node2);

		if(cmp < 0)
		{
			node1=node1->next;
		}
		else if(cmp > 0)
		{
			node2=node2->next;
		}
		else{
			node=nodeCopy(node1);

			node1=node1->next;
			node2=node2->next;

			if(link == NULL)
			{
				link=node;
			}
			else
			{
				prenode->next=node;
			}
			prenode=node;
			node->next=NULL;
		}
	}

	return link;
}

Node *set_difference(Node *A,Node *B)
{
	Node *node1,*node2;
	Node *link=NULL,*node,*prenode=NULL;
	int cmp;

	if(B == NULL)
	{
		return linkCopy(A);
	}
	if(A == NULL)
	{
		return NULL;
	}

	node1=A;
	node2=B;

	while(node1 != NULL && node2 != NULL)
	{
		cmp=NodeCmp(node1,node2);
		if(cmp == 0)
		{
			node1=node1->next;
			node2=node2->next;
		}
		else
		{
			if(cmp < 0)
			{
				node=nodeCopy(node1);
				node1=node1->next;

				if(link == NULL)
				{
					link=node;
				}
				else
				{
					prenode->next=node;
				}

				prenode=node;
			}
			else
			{
				node2=node2->next;
			}
		}
	}

	if(node1 != NULL)
	{
		node=node1;
	}
	else{
		node=NULL;
	}
	node=linkCopy(node);

	if(link == NULL)
	{
		link=node;
	}
	else{
		prenode->next=node;
	}

	return link;
}

Node *set_symmetric_difference(Node *A,Node *B)
{
	Node *C,*D,*E=NULL;

	C=set_union(A,B);
	D=set_intersection(A,B);
	E=set_difference(C,D);

	FreeLink(C);
	FreeLink(D);

	return E;
}


void operate_all(Node *A,Node *B)
{
	Node *C;

	C=set_union(A,B);
	printf("\n集合求并 结果集合 :\n");
	printSetLink(C);
	FreeLink(C);

	C=set_intersection(A,B);
	printf("\n集合求交 结果集合 :\n");
	printSetLink(C);
	FreeLink(C);

	C=set_difference(A,B);
	printf("\n集合求差 结果集合 :\n");
	printSetLink(C);
	FreeLink(C);

	C=set_symmetric_difference(A,B);
	printf("\n集合求对称差 结果集合 :\n");
	printSetLink(C);
	FreeLink(C);
}

void intersection ()
{
	Node *A=NULL,*B=NULL,*C=NULL,*exNode,*inNode;
	int oper=3;
	int over;
	exNode=(Node *)calloc(1,sizeof(Node));
	inNode=(Node *)calloc(1,sizeof(Node));

	A=createSetLink(NULL);

	//insertCreateSetLink(exNode,8000);
	//exNode=set_sort(exNode);

	//insertCreateSetLink(inNode,10001);
	//inNode=set_sort(inNode);

	printf("请输入集合A与B运算方式: 1代表求并集，2代表交集，3代表求差集，4代表求对称差集\n");

	printf("\n集合A :\n");
	printSetLink(A);


	printf("\n集合exNode :\n");
	printSetLink(exNode);

	printf("\n集合inNode :\n");
	printSetLink(inNode);

	B=set_union(exNode,inNode);
	printf("\n集合B :\n");
	printSetLink(B);

	C=set_difference(A,B);
	printf("\n集合set_difference :\n");
	printSetLink(C);


	FreeLink(A);
	FreeLink(exNode);
	FreeLink(inNode);
	FreeLink(C);

	getchar();
}
