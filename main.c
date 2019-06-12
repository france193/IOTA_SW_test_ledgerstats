#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "graphLibrary.h"

// PROTOTYPES
void computeAverageNumberOfInReferencesPerNode(ledger_p ledger);

void computeAverageDepthOfDAG(int numNodes, int *depths);

void computeAverageNumberOfTransactionsPerDepth(int numNodes, int *depths);

void computeAverageNumberOfTransactionsPerTimestampUnit(ledger_p ledger);

void computePercentageConfirmationLevelForEachNodeReferringToTheMaximumReferencesNumberOfOrigin(ledger_p ledger);

void computeDepths(int *depths, int *bstResult, int numNodes);

// MAIN
int main(int argc, char **argv) {
    ledger_p ledger;
    int *bstResult, *depths;

    if (DEBUG) {
        fprintf(stdout, "> Program start!\n");
    }

    if (argc != 2) {
        error_exit(-1, "Expected 2 argument: <prog_name> <database_filename.txt>\n");
    }

    // another example DAG by me
    //ledger = readLedgerFromDatabase("database_1.txt");
    ledger = readLedgerFromDatabase(argv[1]);

    if (true) {
        displayGraphMatrix(ledger);
    }

    // using the result array, compute the depth for each node (minimun distance to the origin)
    if ((depths = (int *) malloc(ledger->numNodes * sizeof(int))) == NULL) {
        error_exit(-4, "Unable to allocate memory");
    };

    // compute and show statistic
    printf("\nSTATISTICS\n");

    // perform a breadth first search on the DAG starting from the origin (node with ID:1) that has index 0 on
    // the adjacency matrix (it can also be used Bellman-Ford or Dijkstra algorithm using each link with weight = 1)
    bstResult = performBreadthFirstSearch(ledger, 0);

    // having the array with bstResult it is possible to compute a depth for each node
    computeDepths(depths, bstResult, ledger->numNodes);

    /** 1 **/
    computeAverageDepthOfDAG(ledger->numNodes, depths);
    /** 2 **/
    computeAverageNumberOfTransactionsPerDepth(ledger->numNodes, depths);
    /** 3 **/
    computeAverageNumberOfInReferencesPerNode(ledger);
    /** 4 **/
    computeAverageNumberOfTransactionsPerTimestampUnit(ledger);
    /** 5 **/
    computePercentageConfirmationLevelForEachNodeReferringToTheMaximumReferencesNumberOfOrigin(ledger);

    // free memory
    destroyGraph(ledger);
    free(bstResult);
    free(depths);

    return 0;
}

// FUNCTIONS
void computeAverageDepthOfDAG(int numNodes, int *depths) {
    double avg = 0.0;
    int i;

    // compute the sum of all depths
    for (i = 0; i < numNodes; i++) {
        avg += depths[i];
        if (DEBUG) {
            printf(" From node 1 to %d, depth is: %d\n", i + 1, depths[i]);
        }
    }

    // compute average
    printf("> AVG DAG DEPTH: %.2lf\n", avg / numNodes);
}

void computeAverageNumberOfTransactionsPerDepth(int numNodes, int *depths) {
    int *depthsCounter, i, maxDepth = 0, depthCount = 0;

    // find maxDepth value
    for (i = 0; i < numNodes; i++) {
        if (depths[i] > maxDepth) {
            maxDepth = depths[i];
        }
    }

    // create array to count how many a depth occurs
    if ((depthsCounter = (int *) malloc(maxDepth * sizeof(int))) == NULL) {
        error_exit(-4, "Unable to allocate memory");
    };

    if (DEBUG) {
        printf("maxDepth: %d\n", maxDepth);

        printf("Depths:\n");

        for (i = 0; i < numNodes; i++) {
            printf(" %d ", depths[i]);
        }

        printf("\nDepths count\n");
    }

    // count how many depths I have
    for (i = 0; i < maxDepth; i++) {
        depthsCounter[i] = 0;
    }

    // count how many times a depth appears and save on my array counter
    for (i = 0; i < numNodes; i++) {
        if (depths[i] != 0) {
            depthsCounter[depths[i] - 1]++;
        }
    }

    if (DEBUG) {
        for (i = 0; i < maxDepth; i++) {
            printf(" %d ", depthsCounter[i]);
        }
        printf("\n");
    }

    // sum all depths
    for (i = 0; i < maxDepth; i++) {
        depthCount += depthsCounter[i];
    }

    free(depthsCounter);

    // compute average
    printf("> AVG TXS PER DEPTH: %.2lf\n", (double) depthCount / maxDepth);
}

void computeAverageNumberOfInReferencesPerNode(ledger_p ledger) {
    int i, j, references = 0;

    // count all references, sum all references for each node
    for (i = 0; i < ledger->numNodes; i++) {
        for (j = 0; j < ledger->numNodes; j++) {
            if (ledger->graphAdjacencyMatrix[i][j].timestamp != -1) {
                references += ledger->graphAdjacencyMatrix[i][j].references;
            }
        }
    }

    // compute average
    printf("> AVG REF: %.2lf\n", (double) references / ledger->numNodes);
}

void computeAverageNumberOfTransactionsPerTimestampUnit(ledger_p ledger) {
    // assuming that this ledger was created a timestamp 0
    int i, j, txs = 0;

    // compute total number of transactions (in-reference) and then perform an average considering timestamp
    for (i = 0; i < ledger->numNodes; i++) {
        for (j = 0; j < ledger->numNodes; j++) {
            if (ledger->graphAdjacencyMatrix[i][j].timestamp != -1) {
                txs += ledger->graphAdjacencyMatrix[i][j].references;
            }
        }
    }

    // this could be useful to have an idea of the throughput of the network. TX per seconds or depending on timestamp
    // considered unit
    printf("> (ADDITIONAL PROPOSED STATISTIC 1) AVG TXs PER SECOND: %.2lf\n", (double) txs / ledger->maxTimestamp);
}

void computePercentageConfirmationLevelForEachNodeReferringToTheMaximumReferencesNumberOfOrigin(ledger_p ledger) {
    int *nodeReferences, i, j;

    // Having in mind the idea of the tangle, each node is a TX and a link means that another TX refers to it.
    // I want to compute (in percentage) the level of confirmation (how much TXs refers to a specific TX, in this case
    // a node with a specific ID).
    // A TX can be referred directly or undirectly meaning that if A refers to B and B refers to C, A has 2 total
    // references. In fact B reference directly A and C, references undirectly A.
    // The maximum confirmation is the number of TXs that refers to the origin node. Every TXs on the tangle will refer
    // directly or undirecly to the origin.
    // A TX that refers to another TX mean that a TX confirm another TX.
    // I want to compute the level of confirmation of a TX referring to the maximum possible. In this way I can give an
    // idea of the acceptance status of a specific TX (a node inside the DAG).

    // allocate nodeReferences and error check
    if ((nodeReferences = (int *) malloc(ledger->numNodes * sizeof(int))) == NULL) {
        error_exit(-4, "Unable to allocate memory");
    };

    // initialize all references
    for (i = 0; i < ledger->numNodes; i++) {
        nodeReferences[i] = 0;
    }

    // Compute all references for each node starting from the bottom of the DAG.
    // If A refers to B and B refers to C A has 1 reference (from B) and all the reference of B, so is better to start
    // to compute B references and then use this data to compute A references.
    for (i = ledger->numNodes - 1; i >= 0; i--) {
        for (j = ledger->numNodes - 1; j >= 0; j--) {
            if (ledger->graphAdjacencyMatrix[i][j].references != 0) {
                nodeReferences[j] += ledger->graphAdjacencyMatrix[i][j].references;
                nodeReferences[j] += nodeReferences[i];
            }
        }
    }

    /*
    for (i = ledger->numNodes - 1; i >= 0; i--) {
        for (j = ledger->numNodes - 1; j >= 0; j--) {
            if (ledger->graphAdjacencyMatrix[i][j].references != 0) {
                printf("For TX %d, I need TXs of TX %d\n", j+1, i+1);
                nodeReferences[j] += nodeReferences[i];
            }
        }
    }
    */

    // compute statistics for each TXs (node)
    printf("> (ADDITIONAL PROPOSED STATISTIC 2) TX CONFIRMATION %% REFERRING TO THE OVERALL NETWORKS:\n");
    for (i = 0; i < ledger->numNodes; i++) {
        printf("  - TX with ID %d has %3d references (direct/indirect), corresponds to %6.2lf%%.\n",
               i + 1,
               nodeReferences[i],
               (((double) nodeReferences[i] / nodeReferences[0])) * 100);
    }

    free(nodeReferences);
}

void computeDepths(int *depths, int *bstResult, int numNodes) {
    int i, j, t;

    for (i = 0; i < numNodes; i++) {
        t = 0;
        j = i;

        while (j != 0) {
            t++;
            j = bstResult[j];
        }
        depths[i] = t;
    }
}
