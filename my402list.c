//Author: Aditya Barawkar
//barawkar@usc.edu
//CSCI402: warmup1
#include<stdio.h>
#include<string.h>
#include "my402list.h"
#include<stdlib.h>
#define MAX_LINE 1024
#ifdef DEBUG
#	define dbg(x) printf("DEBUG:%s",x)
#else
#	define dbg(x) {}//No Debug
#endif
int errno;
char dbgMsg[1024];
extern int  My402ListLength(My402List* list)
{
	return(list->num_members);
}
extern int  My402ListEmpty(My402List* list)
{
	if (list->num_members == 0)
		return(1); //TRUE
	else
		return(0); //FALSE
}
extern int  My402ListAppend(My402List* list, void* data)
{
//Add obj after Last(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. 
	My402ListElem *last;
	My402ListElem *p = (My402ListElem *)malloc(sizeof(My402ListElem));
	if ( p == NULL)
	{
		fprintf(stderr,"ERROR:In function: %s at line %d. %s \n",__func__,__LINE__,strerror(errno));
		return(0); //FALSE
	}
	p->obj = data;
	if ( My402ListEmpty(list) )
	{
		list->anchor.next = list->anchor.prev = p;
		p->next = &(list->anchor);
		p->prev = &(list->anchor);
		list->num_members = 1;
		return(1); //TRUE
	}
	else
	{
		last = My402ListLast(list);
		last->next = p;
		p->next = &(list->anchor);
		p->prev = last;
		list->anchor.prev  = p;
		list->num_members = (list->num_members) + 1;
		return(1); //TRUE
	}
	//fprintf(stderr,"Some thing unusual in %s at line %d. No condition in if was satisifed.Returning FALSE \n",__func__,__LINE__);
	//return(0); //FALSE
}
extern int  My402ListPrepend(My402List* list, void* data)
{
//Add obj before First(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. 
	My402ListElem *first;
	My402ListElem *p = (My402ListElem *)malloc(sizeof(My402ListElem));
	if (p == NULL)
	{
		fprintf(stderr,"ERROR: In function: %s at line %d %s \n",__func__,__LINE__,strerror(errno));
		return(0); //FALSE 
	}
	p->obj = data;
	if ( My402ListEmpty(list) )
	{
		list->anchor.next = list->anchor.prev = p;
		p->next = &(list->anchor);
		p->prev = &(list->anchor);
		list->num_members = 1;
		return(1); //TRUE
	}
	else
	{
		first = My402ListFirst(list);
		list->anchor.next = p;
		p->prev = &(list->anchor);
		p->next = first;
		first->prev = p;
		list->num_members = (list->num_members) + 1;
		return(1); //TRUE
	}
	//sprintf(dbgMsg,"Some thing unusual in %s at line %d. No condition in if was satisifed.Returning FALSE \n",__func__,__LINE__);
        //dbg(dbgMsg);
        //return(0); //FALSE
}
extern void My402ListUnlink(My402List* list, My402ListElem* p)
{
	//Unlink and delete elem from the list. Please do not delete the object pointed to by elem and do not check if elem is on the list. 	
	//Assume the element is present in list. Still will check a trivila thing that if there is atleast one element in the list.
	if (list->num_members == 0)
	{
		fprintf(stderr,"In function %s and line %d. \nThere are no elements in the list and still trying to delete.Exiting..\n",__func__,__LINE__);
		exit(1);
	}
	(p->prev)->next = p->next;
	(p->next)->prev = p->prev;
	list->num_members = (list->num_members) - 1;
	free(p);
}
extern void My402ListUnlinkAll(My402List* list)
{
//Unlink and delete all elements from the list and make the list empty. Please do not delete the objects pointed to be the list elements. 
	while ( list->num_members > 0)
	{
		My402ListUnlink(list, list->anchor.next);
	}
}
extern int  My402ListInsertAfter(My402List* list, void* data, My402ListElem* elem)
{
//Insert obj between elem and elem->next. If elem is NULL, then this is the same as Append(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. Please do not check if elem is on the list. 
	My402ListElem *p;
	if (elem == NULL)
	{
		if (My402ListAppend(list,data))
			return(1); // TRUE
		else
			return(0); //FALSE
	}
	else
	{
		p = (My402ListElem *)malloc(sizeof(My402ListElem));
		if (p == NULL)
         	{
       		         fprintf(stderr,"ERROR: In function: %s at line %d %s \n",__func__,__LINE__,strerror(errno));
      			 return(0); //FALSE 
       		}
		p->obj = data;
		p->next = elem->next;
		p->prev = elem;
		elem->next= p;
		(p->next)->prev = p;	
		list->num_members = (list->num_members) + 1;
		return(1); //TRUE
	}	
}
extern int  My402ListInsertBefore(My402List* list, void* data, My402ListElem* elem)
{
//Insert obj between elem and elem->prev. If elem is NULL, then this is the same as Prepend(). This function returns TRUE if the operation is performed successfully and returns FALSE otherwise. Please do not check if elem is on the list. 
	My402ListElem *p;
	if (elem == NULL)
	{
		if(My402ListPrepend(list,data))
			return(1); //TRUE
		else
			return(0); //FALSE
	}
	else
	{
		p = (My402ListElem *)malloc(sizeof(My402ListElem));
		if (p == NULL)
                {
                         fprintf(stderr,"ERROR: In function: %s at line %d %s \n",__func__,__LINE__,strerror(errno));
                         return(0); //FALSE 
                }
		p->obj = data;
		p->prev = elem->prev;
		p->next = elem;
		elem->prev = p;
		(p->prev)->next = p;
		list->num_members = (list->num_members) + 1;
		return(1); //TRUE
	}
}
extern My402ListElem *My402ListFirst(My402List* list)
{
//Returns the first list element or NULL if the list is empty.
	if (My402ListEmpty(list))
		return(NULL);
	else
		return(list->anchor.next); //First element
}
extern My402ListElem *My402ListLast(My402List* list)
{
//Returns the last list element or NULL if the list is empty.
	if (My402ListEmpty(list))
		return(NULL);
	else
		return(list->anchor.prev); //Last element
}
extern My402ListElem *My402ListNext(My402List* list, My402ListElem* elem)
{
//Returns elem->next or NULL if elem is the last item on the list. Please do not check if elem is on the list. 
	My402ListElem *last;
	last = My402ListLast(list);
	if (last == elem)
		return(NULL);
	else
		return(elem->next);
}
extern My402ListElem *My402ListPrev(My402List* list, My402ListElem* elem)
{
//Returns elem->prev or NULL if elem is the first item on the list. Please do not check if elem is on the list. 
	My402ListElem *first;
	first = My402ListFirst(list);
	if (first == elem)
		return(NULL);
	else
		return(elem->prev);
}
extern My402ListElem *My402ListFind(My402List* list, void* data)
{
//Returns the list element elem such that elem->obj == obj. Returns NULL if no such element can be found.
	My402ListElem *first, *last, *p;
	first = My402ListFirst(list);
	last = My402ListLast(list);
	if (NULL == first)
	{	
		sprintf(dbgMsg,"In Find: List is empty. Returning NULL\n");
		dbg(dbgMsg);
		return(NULL); //The list is empty
	}	
	else
	{
		p = first;
		while ( p != last)
		{
			if (p->obj == data)
			{
				sprintf(dbgMsg,"In Find: Found the element\n");
				dbg(dbgMsg);
				return(p); //Found Object
			}
			else
			{
				p = p->next;
			}
		}
		if (p == last)
		{
			if (p->obj == data)
			{
				sprintf(dbgMsg,"In Find: Found the element as the last element\n");
                                dbg(dbgMsg);
                                return(p); //Found Object
			}
			else
			{
				sprintf(dbgMsg,"In Find: Element not found. Returning NULL\n");
				dbg(dbgMsg);
				return(NULL); //Element not found
			}
		}
		else
		{
			sprintf(dbgMsg,"In function %s line #%d. pointer p traveresed whole list but did not reach last. Seems like a logical flaw in the the program. Returning NULL\n",__func__,__LINE__);
			dbg(dbgMsg);
			return(NULL);
		}
	}
}
extern int My402ListInit(My402List* list)
{
//Initialize the list into an empty list. Returns TRUE if all is well and returns FALSE if there is an error initializing the list.
	if (list->num_members == 0)
	{
		list->anchor.next = &(list->anchor);
		list->anchor.prev = &(list->anchor);
		list->anchor.obj = NULL;
		//return(1); //TRUE
	}
	else
	{
		My402ListUnlinkAll(list);
		//return(1); //TRUE
	}
	if (list->num_members != 0)
	{
		sprintf(dbgMsg, "In function %s line #%d. Seems like unlink all dint work. Returning False as UnlinkAll did not leave the list with zero elements\n",__func__,__LINE__);
		dbg(dbgMsg);
		return(0); //FALSE
	}
	else
	{
		return(1); //TRUE
	}
}
