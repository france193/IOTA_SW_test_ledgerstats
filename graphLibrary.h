//
// Created by Francesco Longo on 12/06/2019.
//

#ifndef LEDGERSTATS_GRAPHLIBRARY_H
#define LEDGERSTATS_GRAPHLIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "functions.h"
#include "queueLibrary.h"

#define N 100

typedef struct {
    int weight;
    int timestamp;
    int references;
} info, *info_p, **info_dp;

typedef struct graph {
    int numNodes;
    int maxTimestamp;
    info **graphAdjacencyMatrix;
} *ledger_p;

ledger_p readLedgerFromDatabase(char *filename);

ledger_p createGraph(int numVertices);

void displayGraphMatrix(ledger_p ledger);

void addEdges(ledger_p ledger, int node, int leftParent, int rightParent, int timestamp);

void destroyGraph(ledger_p ledger);

int *performBreadthFirstSearch(ledger_p ledger, int startnode);

#endif //LEDGERSTATS_GRAPHLIBRARY_H
