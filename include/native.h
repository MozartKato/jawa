#ifndef NATIVE_H
#define NATIVE_H

// Transpile a .jw source file to C and build a native binary at outPath
// Returns 0 on success, non-zero on failure.
int build_native(const char *srcPath, const char *outPath);

#endif // NATIVE_H
