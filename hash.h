#ifndef _HASH_H
#define _HASH_H

struct HashTable {
	unsigned int capacity;
	unsigned int nel;
	void **data;
	int (*hash)(void *, int);
	int (*compar)(const void *, const void *);
};

struct Performance {
	unsigned int reads;
	unsigned int writes;
	unsigned int mallocs;
	unsigned int frees;
};

struct Performance *newPerformance();

struct HashTable *createTable(struct Performance *, unsigned int, int (*hash)(void *, int), int (*compar)(const void *, const void *));

void addElement (struct Performance *, struct HashTable *, void *);

int getIdx (struct Performance *, struct HashTable *, void *);

void freeTable (struct Performance *, struct HashTable *);

void *getElement (struct Performance *, struct HashTable *, void *);

void removeElement (struct Performance *, struct HashTable *, void *);

int hashAccuracy (struct HashTable *);

void rehash (struct HashTable *);
#endif
