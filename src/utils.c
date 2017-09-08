#include "jmap.h"

void wipe_tile(e *E){

    int i,j;

    // fill image data - transparent
    for(j=0;j<E->height;j++){
        for(i=0;i<E->width;i++){
        E->image[ (j * E->width  + i) * 4 + 0 ] = 255;
        E->image[ (j * E->width  + i) * 4 + 1] = 255;
        E->image[ (j * E->width  + i) * 4 + 2] = 255;
        E->image[ (j * E->width  + i) * 4 + 3] = 0;
        }
    }
    return;
}

#define PATH_MAX 2048

void mkdirRecursive(const char *path, mode_t mode) {
    char opath[PATH_MAX];
    char *p;
    size_t len;

    strncpy(opath, path, sizeof(opath));
    opath[sizeof(opath) - 1] = '\0';
    len = strlen(opath);
    if (len == 0)
        return;
    else if (opath[len - 1] == '/')
        opath[len - 1] = '\0';
    for(p = opath; *p; p++)
        if (*p == '/') {
            *p = '\0';
            if (access(opath, F_OK))
                mkdir(opath, mode);
            *p = '/';
        }
    if (access(opath, F_OK))         /* if path is not terminated with / */
        mkdir(opath, mode);
}

