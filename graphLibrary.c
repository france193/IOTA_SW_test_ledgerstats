//
// Created by Francesco Longo on 12/06/2019.
//

#include "graphLibrary.h"

ledger_p readLedgerFromDatabase(char *filename) {
    FILE *fp;
    char line[N];
    int left_parent, right_parent, timestamp, lineNumber, num_nodes;
    ledger_p ledger = NULL;

    lineNumber = 1;
    left_parent = right_parent = timestamp = 0;

    if (DEBUG) {
        fprintf(stdout, "> Opening \"%s\"...\n", filename);
    }

    // open file
    if ((fp = fopen(filename, "rt")) == NULL) {
        error_exit(-2, "Error reading file.");
    }

    // get line from a file
    while (fgets(line, N, fp) != NULL) {
        // first line is the number of node (excluding the origin)
        if (lineNumber == 1) {
            // read num nodes withoud origin
            sscanf(line, "%d", &num_nodes);

            int all_vertices = num_nodes + 1;

            if (DEBUG) {
                fprintf(stdout, "> total nodes (origin included): %d\n", all_vertices);
            }

            ledger = createGraph(all_vertices);
        } else {
            // for each line read edges
            sscanf(line, "%d %d %d", &left_parent, &right_parent, &timestamp);

            // compute last timestamp of DAG for future statistics
            if (lineNumber == 2) {
                ledger->maxTimestamp = timestamp;
            } else {
                if (timestamp > ledger->maxTimestamp) {
                    ledger->maxTimestamp = timestamp;
                }
            }

            if (DEBUG) {
                printf(" - ID: %d -> LEFT: %d, RIGHT: %d\n", lineNumber, left_parent, right_parent);
            }

            addEdges(ledger, lineNumber, left_parent, right_parent, timestamp);
        }

        lineNumber++;
    }

    fclose(fp);

    return ledger;
}

// initialize graph structure
ledger_p createGraph(int numVertices) {
    ledger_p ledger = NULL;
    int i, j;

    // allocate a new ledger and error check
    if ((ledger = (ledger_p) malloc(sizeof(ledger))) == NULL) {
        error_exit(-3, "Unable to allocate memory for graph");
    }

    ledger->numNodes = numVertices;

    if ((ledger->graphAdjacencyMatrix = (info_dp) malloc(numVertices * sizeof(info_p))) == NULL) {
        error_exit(-4, "Unable to allocate memory for graph");
    }

    for (i = 0; i < numVertices; i++) {
        if ((ledger->graphAdjacencyMatrix[i] = (info_p) malloc(numVertices * sizeof(info))) == NULL) {
            error_exit(-5, "Unable to allocate memory for graph");
        }
    }

    for (i = 0; i < numVertices; i++) {
        for (j = 0; j < numVertices; j++) {
            ledger->graphAdjacencyMatrix[i][j].weight = -1;
            ledger->graphAdjacencyMatrix[i][j].timestamp = -1;
            ledger->graphAdjacencyMatrix[i][j].references = 0;
        }
    }

    return ledger;
}

// usefull function to display DAG as adjacency matrix visually (for DAG with numNode < 10'000)
// Each column is the considered node, on the Row, if a link exists you will find how much links there are between
// node j and node i exist. In the form: 000001.
// NB-1: the adjacency matrix is not symmetric by choice
// NB-2: maybe you need a big big screen...
void displayGraphMatrix(ledger_p ledger) {
    int i, j;

    printf("\n\nShowing DAG Adjacency Matrix:\n");

    printf("        |");
    for (i = 0; i < ledger->numNodes; i++) {
        printf(" %6.6d ", i + 1);
    }
    printf("\n");

    printf("--------");
    for (i = 0; i < ledger->numNodes; i++) {
        printf("--------");
    }
    printf("\n");

    /*
    printf("   |");
    for (i = 0; i < ledger->numNodes; i++) {
        printf("  %d ", i + 1);
    }
    printf("\n");

    printf("----");
    for (i = 0; i < ledger->numNodes; i++) {
        printf("----");
    }
    printf("\n");

    for (i = 0; i < ledger->numNodes; i++) {
        printf(" %d |", i + 1);
        for (j = 0; j < ledger->numNodes; j++) {
            if (ledger->graphAdjacencyMatrix[i][j] != -1) {
                printf("  %d ", ledger->graphAdjacencyMatrix[i][j]);
            } else {
                printf(" %d ", ledger->graphAdjacencyMatrix[i][j]);
            }
        }
        printf("\n");
    }
    */

    for (i = 0; i < ledger->numNodes; i++) {
        printf(" %6.6d |", i + 1);
        for (j = 0; j < ledger->numNodes; j++) {
            if (ledger->graphAdjacencyMatrix[i][j].timestamp != -1) {
                printf(" %6.6d ", ledger->graphAdjacencyMatrix[i][j].references);
                //} else if (i == j) {
                //    printf(" x ");
            } else {
                printf(" ------ ");
            }
        }
        printf("\n");
    }

    printf("\n");
}

// add an edge
void addEdges(ledger_p ledger, int node, int leftParent, int rightParent, int timestamp) {
    ledger->graphAdjacencyMatrix[node - 1][leftParent - 1].timestamp = timestamp;
    ledger->graphAdjacencyMatrix[node - 1][rightParent - 1].timestamp = timestamp;

    ledger->graphAdjacencyMatrix[node - 1][leftParent - 1].weight = 1;
    ledger->graphAdjacencyMatrix[node - 1][rightParent - 1].weight = 1;

    ledger->graphAdjacencyMatrix[node - 1][leftParent - 1].references++;
    ledger->graphAdjacencyMatrix[node - 1][rightParent - 1].references++;
}

// free memory
void destroyGraph(ledger_p ledger) {
    for (int i = 0; i < ledger->numNodes; i++) {
        free(ledger->graphAdjacencyMatrix[i]);
    }

    free(ledger->graphAdjacencyMatrix);
}

// Breadth First Search on the DAG
int *performBreadthFirstSearch(ledger_p ledger, int startnode) {
    int *st, i, nindex;

    st = (int *) malloc(ledger->numNodes * sizeof(int));

    for (i = 0; i < ledger->numNodes; i++) {
        st[i] = -1;
    }

    st[startnode] = startnode;
    queue_t *q = queue_alloc();
    queue_insert(q, startnode);

    while ((!queue_empty(q))) {
        queue_extract(q, &nindex);

        for (i = 0; i < ledger->numNodes; i++) {
            if (st[i] == -1 && (ledger->graphAdjacencyMatrix[i][nindex].weight != -1)) {
                queue_insert(q, i);
                st[i] = nindex;
            }
        }
    }

    queue_free(q);

    return st;
}
