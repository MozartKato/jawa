#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "vm.h"
#include "native.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage:\n  %s compile input.jw output.jbc\n  %s run output.jbc\n  %s build input.jw output_bin\n", argv[0], argv[0], argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "compile") == 0) {
        if (argc != 4) { fprintf(stderr, "compile mode needs input & output\n"); return 1; }
        compile(argv[2], argv[3]);
    } else if (strcmp(argv[1], "run") == 0) {
        run(argv[2]);
    } else if (strcmp(argv[1], "build") == 0) {
        if (argc != 4) { fprintf(stderr, "build mode needs input & output\n"); return 1; }
        int rc = build_native(argv[2], argv[3]);
        return rc;
    } else {
        fprintf(stderr, "Unknown command\n");
        return 1;
    }
    return 0;
}
