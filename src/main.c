#include <stdio.h>
#include <stdlib.h>

#include "rpc.h"

int main(int argc, char *argv[]) {
    struct rpc rpc;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (rpc_init(&rpc, argv[1]) == -1) {
        fprintf(stderr, "rpc_init: failed\n");
        exit(EXIT_FAILURE);
    }

    if (rpc_listen(&rpc) == -1) {
        fprintf(stderr, "rpc_listen: failed\n");
        exit(EXIT_FAILURE);
    }

    rpc_destroy(&rpc);

    return 0;
}