/************************************************************************************************************************************************
* LINKED LIST STRUCTURE ADAPTED FOR USE WITH PATTERNS																							*
/************************************************************************************************************************************************
* IMPLEMENTED BY: MARCO ANTONIO MOREIRA DE CARVALHO (marco.opt@gmail.com)																		*
* AERONAUTICS TECHNOLOGICAL INSTITUTE - BRAZIL																									*
* DATE OF CREATION: ??/??/????																													*
* LAST MODIFICATION: 02/07/2009																													*
/************************************************************************************************************************************************
* TERMS OF USE																																	*
*																																				*			
* This code available for free public use.																										*		
* The code in this archive is licensed gratis to all third parties under the terms of this paragraph.											*
* Copying and distribution of the files in this archive is unrestricted if and only if the files are not modified.								*
* Modification of the files is encouraged, but the distribution of modifications of the files in this archive is unrestricted only				*
* if you meet the following conditions: modified files must carry a prominent notice stating (i) the author and date,							*
* (ii) the new author and the date of release of the modification, (iii) that the work is licensed at no charge to all parties.					*
*																																				*
* If you use the code extensively in your research, you are requested to provide appropriate attribution and thanks to the author of the code.  *
/************************************************************************************************************************************************
* DISCLAIMER OF WARRANTY																														*			
*																																				*	
* This source code is provided "as is" and without warranties as to performance or merchantability.												*
* The authors and/or distributors of this source code may have made statements about this source code.											*
* Any such statements do not constitute warranties and shall not be relied on by the user in deciding whether to use this source code.			*
*																																				*
* This source code is provided without any express or implied warranties whatsoever. Because of the diversity of conditions and hardware under	*
*  which this source code may be used, no warranty of fitness for a particular purpose is offered.												*
* The user is advised to test the source code thoroughly before relying on it. The user must assume the entire risk of using the source code.	*
*																																				*	
************************************************************************************************************************************************/

#include <stdio.h>
#include <cstddef>
#include <cstdlib>

typedef struct	{
				int blocked; //indicates whether the pattern has been already sequenced
				int removed; //indicates whether the pattern has been removed in the preprocessing 
				int size;    //stores the size of the pattern, in number of pieces
			}typePattern;    //pattern structure


struct cell	{
					int pattern;		//pattern structure
					struct cell *next;	//pointer to the next cell
				};						//list cell structure

typedef struct	{
					struct cell* first; //first cell of the list
					struct cell* last;  //last cell of the list
				}typeList;

// typePattern *pattern;


/*
Makes the list empty
*/
void emptyList(typeList *list)
{
	(*list).first = NULL;
	(*list).last = NULL;
}

/*
Adds the element at the end of the list
*/
void add(typeList *list, int original)
{
    struct cell *control = list->first;
    
	if ((*list).last == NULL)
	{
		(*list).last = (struct cell*)malloc(sizeof(struct cell));
		(*list).last->pattern = original;
		list->first = list->last;
		(*list->last).next = NULL;
	} 
	else 
	{			
		struct cell *control = (*list).first;			

		(*list->last).next = (struct cell*)malloc(sizeof(struct cell));
		list->last = (*list->last).next;
		(*list).last->pattern = original;
		(*list->last).next = NULL;
	}
}

/*
Counts the number of elements in a list
*/
int count(typeList *list)
{
	int size = 0;
	struct cell *control = list->first;

	while (control != NULL)
	{
		size++;
		control = control->next;
	}

	return size;
}

/*
Finalizes a list, frees all pointers
*/
void finalize(typeList *list)
{
	struct cell *control = list->first;

	while (control != NULL)
	{
		list->first = control->next;
		free(control);
		control = list->first;
	}


	(*list).first = NULL;
	(*list).last = NULL;
}

/*
Transfer a list to another
*/
void transferDominated(typeList *destiny, typeList *origin)
{
	struct cell *control = origin->first;

	if(count(origin))
	{
		while (control != NULL)
		{
			add(destiny, control->pattern);
			control = control->next;
		}
	}
}

/*
Transfer the elements of the list to the solution vector
*/
void transfer(typeList *list, int *solution, int *indice)
{
	struct cell *control = list->first;

	while (control != NULL)
	{
		solution[*indice] = control->pattern;
		(*indice)++;
		control = control->next;
	}
}

/*
Prints the patterns of a list of a block
*/
void print(typeList *list, FILE *fp)
{
	struct cell *control = list->first;

	while (control != NULL)
	{
		fprintf(fp, "%d [dominated]\n", control->pattern+1);
		control = control->next;
	}
}

void print2(typeList *list, FILE *fp, int n, int **matrix)
{
	int j;
	struct cell *control = list->first;

	while (control != NULL)
	{
		for(j=0; j<n; j++)	
			fprintf(fp, "%d ", matrix[control->pattern][j]);

		fprintf(fp, "\n");
		control = control->next;
	}
}

int insereOrdenado(typeList *list, int originalPattern, int *degrees)
{
	struct cell *control = list->first;
	struct cell *temp;
	int i;

	if(degrees[list->first->pattern] >  degrees[originalPattern])
	{
		temp = list->first;
		list->first = (struct cell*)malloc(sizeof(struct cell));
		list->first->pattern = originalPattern;
		list->first->next = temp;
	
		return 1;
	}

	for(i=1; i<count(list); i++)
	{
		if(degrees[control->next->pattern] >  degrees[originalPattern])
		{
			temp = control->next;
			control->next = (struct cell*)malloc(sizeof(struct cell));
			control->next->pattern = originalPattern;
			control->next->next = temp;
			
			return 1;
		}
		
		control = control->next;
	}

	add(list, originalPattern);

	return 0;
}
