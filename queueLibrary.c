//
//  Coda.c
//
//  Created by Francesco on 22/08/13.
//  Copyright (c) 2013 Francesco Longo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "queueLibrary.h"

// init queue
queue_t *queue_alloc(void) {
    // create queue and error check
    queue_t *qp;
    qp = (queue_t *) malloc(sizeof(queue_t));

    if (qp == NULL) {
        printf("Errore di allocazione di memoria\n");
        return NULL;
    }

    // initialize pointer and element number
    qp->head = NULL;
    qp->tail = NULL;
    qp->num = 0;

    return qp;
}

// destroy queue
void queue_free(queue_t *qp) {
    nodo_t *node, *head;

    if (qp == NULL) {
        return;
    }

    head = qp->head;

    while (head != NULL) {
        node = head;
        head = head->next;
        free(node);
    }

    free(qp);
}

// insert element in queue
int queue_insert(queue_t *qp, int dato) {
    // create node and error check
    nodo_t *node;

    if (qp == NULL) {
        return QUEUE_ERROR;
    }

    // allocate node and error check
    node = alloc(dato);
    if (node == NULL) {
        return QUEUE_ERROR;
    }

    // if head is null, I have to insert for the first time
    if (qp->head == NULL) {
        qp->head = node;
    } else {
        // otherwise insert to the tail
        qp->tail->next = node;
    }

    // update node number and tail pointer
    qp->tail = node;
    qp->num++;

    return !QUEUE_ERROR;
}

// take an element from queue
int queue_extract(queue_t *qp, int *dp) {
    nodo_t *node;

    if (qp == NULL) {
        return QUEUE_ERROR;
    }

    node = qp->head;

    if (node == NULL) {
        return QUEUE_ERROR;
    }

    *dp = node->val;

    qp->head = node->next;

    // last extraction from the queue
    if (qp->head == NULL) {
        qp->tail = NULL;
    }

    free(node);

    qp->num--;

    return !QUEUE_ERROR;
}

// allocate a new node with a new value
nodo_t *alloc(int val) {
    nodo_t *p;

    p = (nodo_t *) malloc(sizeof(nodo_t));

    if (p == NULL) {
        printf("Errore: memoria dinamica insufficiente\n");
        return NULL;
    }

    p->val = val;
    p->next = NULL;

    return p;
}

// return queue element number
int queue_empty(queue_t *q) {
    return q->num == 0;
}
