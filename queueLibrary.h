//
//  queueLibrary.h
//
//  Created by Francesco on 22/08/13.
//  Copyright (c) 2013 Francesco Longo. All rights reserved.
//
#ifndef LEDGERSTATS_QUEUELIBRARY_H
#define LEDGERSTATS_QUEUELIBRARY_H

#define QUEUE_ERROR 0

typedef struct queue queue_t;

// node struct
typedef struct nodo nodo_t;

// node
struct nodo {
    int val;
    nodo_t *next;
};

// queue struct
struct queue {
    nodo_t *head;
    nodo_t *tail;
    int num;
};

queue_t *queue_alloc(void);

void queue_free(queue_t *qp);

int queue_insert(queue_t *qp, int d);

int queue_extract(queue_t *qp, int *dp);

int queue_empty(queue_t *q);

nodo_t *alloc(int val);

#endif //LEDGERSTATS_QUEUELIBRARY_H
