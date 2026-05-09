#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CITIES   20
#define MAX_CITY_LEN 64
#define INF          999999

/* Kenar yapisi (komşuluk listesi icin bagli liste dugumu) */
typedef struct Edge {
    int          to;           /* Hedef sehrin indeksi */
    int          distance_km;  /* Mesafe (km)          */
    struct Edge *next;         /* Sonraki kenar        */
} Edge;

/* Sehir dugumu */
typedef struct {
    char  name[MAX_CITY_LEN]; /* Sehir adi            */
    Edge *head;               /* Komsu listesi basi   */
} City;

/* Graf yapisi - komşuluk listesi temsili */
typedef struct {
    City cities[MAX_CITIES]; /* Sehir dizisi         */
    int  city_count;         /* Toplam sehir sayisi  */
} Graph;

/* Sehir yonetimi */
int  graph_add_city      (Graph *g, const char *name);
int  graph_find_city     (const Graph *g, const char *name);
void graph_add_edge      (Graph *g, const char *from, const char *to, int dist_km);
void graph_print         (const Graph *g);
void graph_free          (Graph *g);

/* BFS - Genislik Oncelikli Arama
 * Ogrenci No 64: sondan 2. rakam = 6 (cift) -> BFS secildi */
void graph_bfs           (const Graph *g, const char *start);
int  graph_bfs_path      (const Graph *g, const char *start, const char *end,
                          int path_out[], int *path_len);

/* Bellek raporu */
void graph_memory_report (const Graph *g);

#endif /* GRAPH_H */