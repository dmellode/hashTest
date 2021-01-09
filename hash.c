#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

/*
 * CIS2520 A5
 * Title: hash.c
 * Author: Dean D'Mello
 * Last Updated: 2020/11/04
 */


/****newPerformance****/
struct Performance *newPerformance(){
	//Creating and mallocing new Performance structure
	struct Performance *newPerformance;

        if ((malloc (sizeof(struct Performance)))==NULL){
		fprintf (stderr, "%s", "ERROR: FAILED TO MALLOC\n");
		exit(1);
	}

	newPerformance = malloc (sizeof(struct Performance));

	//Initializing all struct values to 0
	newPerformance->reads = 0;
	newPerformance->writes = 0;
	newPerformance->mallocs = 0;
	newPerformance->frees = 0;

	return newPerformance;
}

/****createTable****/
struct HashTable *createTable(struct Performance *performance, unsigned int capacity, int (*hash)(void *, int), int (*compar)(const void *, const void *)){
	//Declarations
	unsigned int i;

	//Malloc for HashTable
	struct HashTable *newTable;

	if (malloc (sizeof (struct HashTable))==NULL){
		fprintf (stderr, "%s", "ERROR: FAILED TO MALLOC NEW TABLE");
		exit (1);
	}
	newTable = malloc (sizeof(struct HashTable));

	//Malloc for data
	if (malloc (sizeof (void *)*capacity)==NULL){
		fprintf (stderr, "%s", "ERROR: FAILED TO MALLOC FOR DATA");
		exit (1);
	}
	newTable->data = malloc (sizeof (void *)*capacity);

	//Set value of each pointer in data array to NULL
	for (i = 0; i < capacity; i++){
		newTable->data[i] = NULL;
	}

	//Adjusting Values in Struct
	newTable->capacity = capacity;
	newTable->hash = hash;
	newTable->compar = compar;
	newTable->nel = 0;

	//Incrementing Performance
	performance->reads += capacity;
	performance->mallocs++;

	return newTable;
}

/****addElement****/
void addElement (struct Performance *performance, struct HashTable *table, void *src){
	//Declarations
	int index;
	int i = 0;

	//Checking for full table
	if (table->nel == table->capacity){
		fprintf (stderr, "%s", "ERROR: CAN'T ADD TO FULL TABLE");
		exit (1);
	}

	//Finding starting index
	index = table->hash (src, table->capacity);

	//Adding element (Linear probing if collision)
	while (i == 0){
		//Inserting if cell is empty
		if (table->data[index] == NULL){
			table->data[index] = src;
			table->nel++;
			performance->writes++;
			i = 1;	
		}
		//Incrementing if empty cell was not found
		if (i == 0){	
			index++;
			performance->reads++;
		}
		//Looping back to the beginning
		if (index == table->capacity){
			index = 0;
		}
	}	
}

/****getIdx****/
int getIdx (struct Performance *performance, struct HashTable *table, void *src){
	//Declarations
	int index, index1, comp;
	int i = 0;

	//Finding starting index
	index = table->hash (src, table->capacity);
	index1 = index;

	//Comparing each pointer's data to src
	while (i != 1){
		//Returned to original index
		if (i == 2 && index == index1){
			return (-1);
		}

		//Comparing
		if (table->data[index]!= NULL){
			comp = table->compar (src, table->data[index]);
			performance->reads++;
		}

		//Found match
		if (comp == 0){
			return index;
		}
		index++;
		//Looping back to beginning
		if (index == table->capacity){
			index = 0;
		}
		i = 2;
	}
	//Code will never reach here but to avoid compilation error
	return index;
}

/****freeTable****/
void freeTable (struct Performance *performance, struct HashTable *table){
	//Freeing data
	int i;
	//Freeing each pointer in data array
	for (i = 0; i<table->capacity; i++){
		if (table->data[i] == NULL){
			free (table->data[i]);
		}
		else {
			table->data[i] = NULL;
			free (table->data[i]);
		}
	}

	//Freeing data in structure
	free (table->data);	

	//Freeing structure
	free (table);

	//Incrementing Performance
	performance->frees++;
}

/****getElement****/
void *getElement (struct Performance *performance, struct HashTable *table, void *src){
	//Declarations
	int index;

	//Finding Index
	index = getIdx (performance, table, src);

	//Corresponding return value
	if (index == -1){
		return NULL;
	}
	else {
		return (table->data[index]);
	}
}

/****removeElement****/
void removeElement (struct Performance *performance, struct HashTable *table, void *target){
	//Declarations
	int index;

	//Finding Index
	index = getIdx (performance, table, target);

	//Removing the element
	if (index != -1){
		table->data[index] = NULL;
	}

	performance->writes++;
	table->nel--;
}

/****hashAccuracy****/
int hashAccuracy (struct HashTable *table){
	//Declarations
	int i, hashNum;
	int acc = 0;

	//Loop through table to see hash values
	for (i = 0;i < table->capacity; i++){
		if (table->data[i] != NULL){
			hashNum = table->hash (table->data[i], table->capacity);
			//Add to hash accuracy based on case
			if (i > hashNum){
				acc += i-hashNum;
			}
			else if (i < hashNum){
				acc += table->capacity - hashNum + i;
			}
		}
	}
	return acc;
}

/****rehash****/
void rehash (struct HashTable *table){
	//Declaration
	int acc, i, j, hashNum, h;

	//Adjust pointer indeces based on each hashNum calculation
	for (i = 0;i < table->capacity; i++){
		acc = 0;
		if (table->data[i] != NULL){
			hashNum = table->hash (table->data[i], table->capacity);
			//Find accuracy based on case
			if (i > hashNum){
				acc = i - hashNum;
				//Adjust pointer index where possible
				for (j=acc;j>0;j--){
					//Checking if move is valid
					if (table->data[i+j] == NULL){
						table->data[i+j] = table->data[i];
						table->data[i] = NULL;
						j = 0;
					}	
				}
			}
			else if (i < hashNum){
				acc = table->capacity - hashNum + i;
				//Adjust pointer index where possible
				for (j=acc;j>0;j--){
					h = j;
					//Checking for wrap around case
					if (i-j < 0){
						j = (table->capacity - j)*-1;
					}
					//Checking if move is valid
					if (table->data[i-j] == NULL){
						table->data[i-j] = table->data [i];
						table->data[i] = NULL;
						j = 0;
					}
					if (j != 0){
						j = h;
					}
				}
			}
		}
	}
}



