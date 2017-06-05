/* 
Brendan Giberson
June 5 2017
Operating systems
*/
#include "pfl1.h"
#include <stdlib.h>

void coal(struct region_node *ptr);
/*get_free_node() has no input parameters. It should search through the
node array for the first node with the node_state == free and return the
address of this node. If there are no free nodes, then get_free_node()
should return a NULL pointer.*/
struct region_node *get_free_node()
{
	for(int i=0;i<NUM_NODES;i++)
	{
		if(node[i].node_state==FREE)
		{
			return &node[i];
		}
	}

		return NULL;
}

/*return_node() takes the address of a node that has been unlinked from
the memory region list as its input parameter and reinitializes the
fields of the node structure.*/
void return_node(struct region_node * n)
{
	n->node_state = FREE;
	n->region_state = FREE;
}

/*alloc() takes the size for a requested memory allocation as its input
parameter. The size must be non-negative; otherwise, alloc() immediately
returns a NULL pointer. For a positive region size, alloc() searches the
memory region list in order, looking for a free memory region that is 
equal in size or larger than the requested size.*/
unsigned char *alloc(int size)
{
	if(size <= 0) return NULL; //if less then 0 return NULL

	//search through mem regions looking for free mem region w right size
	struct region_node* reg_ptr = head;
	int runner = 0;//used to break the while loop

	while(runner == 0)
	{
		if(reg_ptr->region_state != FREE)//if it is not free make sure its not last
		{
			if(reg_ptr->next != head)
			{
				reg_ptr = reg_ptr->next;
				continue;//go to next node and iterate the while loop
			}

			
				runner = 1;
				return NULL;
			
		}

		
			if(reg_ptr->size >= size)//check if size is big enough
			{
				reg_ptr->node_state = IN_USE;

				if(reg_ptr->size == size)//perfect size
				{
					reg_ptr->region_state = IN_USE;
					runner = 1;
					return reg_ptr->start_address;
				}

				else //too big 
				{
					//find new node 
					struct region_node* free_node = get_free_node();

					/*
					split the existing memory region into an in-use memory
					region of the requested size and a new free memory region of whatever size
					is left over.		
					*/

					if(free_node != NULL)
					{ 
							free_node->node_state = IN_USE;
							free_node->region_state = FREE;
							free_node->size = reg_ptr->size - size;
							reg_ptr->size = size;
							free_node->start_address = reg_ptr->start_address + reg_ptr->size;
							free_node->prev = reg_ptr;
							reg_ptr->next->prev = free_node;
							free_node->next = reg_ptr->next;
							reg_ptr->next = free_node;
							reg_ptr->region_state = IN_USE;

							runner = 1;
							return reg_ptr->start_address;
					}

					/*
					get_free_node() returns a NULL address, alloc() should allocate the
					complete memory region, even though it is larger than the requested size.
					*/
					else 
					{
						reg_ptr->region_state = IN_USE;
						runner = 1;
						return reg_ptr->start_address;
					}
				}

			}

			else//if too small find a spot if none found return NULL use continue
			{
				if(reg_ptr->next != head)
				{
					reg_ptr = reg_ptr->next;
					continue;//go to next node and iterate the while loop
				}	

				else //possibly can take out this else statement
				{
					runner = 1;
					return NULL;
				}
			}
		
	}

	return NULL;
}

/*release() takes the address of an in-use memory region as its input
parameter and passes back a return code of 0, 1, or 2, which indicates
the outcome of the call:
  0 - the call to release() was successful
  1 - the input parameter pointed to an existing FREE region
  2 - the input parameter was either NULL or an address that does not
      correspond to the staring address of an existing memory region*/
int release(unsigned char *ptr)
{
	struct region_node* reg_ptr = head->next;
	int runner = 0;
	while(runner == 0)
	{
		if(reg_ptr != head && reg_ptr != NULL)
		{
			if(reg_ptr->start_address != ptr)
			{
				reg_ptr = reg_ptr->next;
				continue;
			}
			else
			{
				if(reg_ptr->region_state != FREE)
				{
					reg_ptr->region_state = FREE;
					//coalesce
					coal(reg_ptr);
					runner = 1;//end loop
					return 0; // call to release success
				}
				else
				{	
					//coalesce
					coal(reg_ptr);
					runner = 1;//end loop
					return 1;// input parameter pointed to existing region
				}
			}
		}
		else {runner=1;return 2;}
	}
	return 2;
}

void coal(struct region_node *ptr)
{
	int runner = 0;
	while(runner == 0)
	{
		if(ptr->next->region_state == FREE) //test if not free
		{
			return_node(ptr->next);
			ptr->size = ptr->size + ptr->next->size;
			ptr->next->next->prev = ptr;
			ptr->next = ptr->next->next;
			continue; //lets loop back through 
		}
		else if(ptr->prev->region_state == FREE)
		{
			return_node(ptr);
			ptr->prev->size = ptr->size + ptr->prev->size;
			ptr->next->prev = ptr->prev;
			ptr->prev->next = ptr->next;
			ptr = ptr->prev;
			continue;
		}

		runner=1; //end loop prob shouldve used recursion 
	}
}









