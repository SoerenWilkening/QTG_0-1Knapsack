#ifndef MAP_H
#define MAP_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned long map_size(void *map);

void *map_create();

void map_put(void *map, char *k, long long v);

long long map_get(void *map, char *k);

#ifdef __cplusplus
}
#endif

#endif