#include "map/map.h"
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

typedef std::map<char *, long long> Map;

unsigned long map_size(void * map) {
    Map *m = reinterpret_cast<Map *>(map);
    return m->size();
}

void *map_create() {
    return reinterpret_cast<void *>(new Map);
}

void map_put(void *map, char *k, long long v) {
    Map *m = reinterpret_cast<Map *>(map);
    m->insert(std::pair<char *, long long>(k, v));
}

long long map_get(void *map, char *k) {
    Map *m = reinterpret_cast<Map *>(map);
    return m->find(k) != m->end() ? m->find(k)->second : -1;
}

#ifdef __cplusplus
}
#endif