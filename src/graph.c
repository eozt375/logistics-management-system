#include "../include/graph.h"

/* BFS icin basit kuyruk yapisi (dizi tabanli, dairesel) */
typedef struct {
    int data[MAX_CITIES];
    int front, rear;
} Queue;

static void q_init (Queue *q)           { q->front = q->rear = 0; }
static int  q_empty(const Queue *q)     { return q->front == q->rear; }
static void q_push (Queue *q, int val)  { q->data[q->rear++ % MAX_CITIES] = val; }
static int  q_pop  (Queue *q)           { return q->data[q->front++ % MAX_CITIES]; }

/* Grafa yeni sehir ekle; zaten varsa mevcut indeksi dondur */
int graph_add_city(Graph *g, const char *name)
{
    if (g->city_count >= MAX_CITIES) return -1;

    int idx = graph_find_city(g, name);
    if (idx >= 0) return idx; /* sehir zaten var */

    idx = g->city_count++;
    strncpy(g->cities[idx].name, name, MAX_CITY_LEN - 1);
    g->cities[idx].head = NULL;
    return idx;
}

/* Sehir adina gore indeks ara; bulamazsa -1 dondur */
int graph_find_city(const Graph *g, const char *name)
{
    for (int i = 0; i < g->city_count; i++)
        if (strcmp(g->cities[i].name, name) == 0) return i;
    return -1;
}

/*
 * Yonlu kenar ekle (from -> to, dist_km)
 * Kenar, komsu listesinin basina eklenir (O(1) ekleme)
 * Bellek: her kenar icin heap'ten malloc ile alan alinir
 */
void graph_add_edge(Graph *g, const char *from, const char *to, int dist_km)
{
    int f = graph_add_city(g, from);
    int t = graph_add_city(g, to);

    Edge *e        = (Edge *)malloc(sizeof(Edge));
    e->to          = t;
    e->distance_km = dist_km;
    e->next        = g->cities[f].head; /* mevcut listenin onune ekle */
    g->cities[f].head = e;
}

/* Graf icerigini komsu listesi formatinda yazdir */
void graph_print(const Graph *g)
{
    printf("\n=== SEHIRLER ARASI BAGLANTI GRAFI ===\n");
    for (int i = 0; i < g->city_count; i++) {
        printf("  [%s]\n", g->cities[i].name);
        for (Edge *e = g->cities[i].head; e; e = e->next)
            printf("      --> %s  (%d km)\n",
                   g->cities[e->to].name, e->distance_km);
    }
    printf("\n");
}

/*
 * BFS - Genislik Oncelikli Arama (Breadth-First Search)
 *
 * Algoritma:
 *   1. Baslangic dugumunu kuyruga ekle, ziyaret et
 *   2. Kuyruk bos olana kadar:
 *      a. Kuyruktan bir dugum cek
 *      b. Ziyaret edilmemis tum komşularini kuyruga ekle
 *
 * Neden BFS? Ogrenci No 64: sondan 2. rakam 6 (cift) -> BFS
 * Avantaj: En az hop sayisina sahip yolu garantiler
 */
void graph_bfs(const Graph *g, const char *start)
{
    int src = graph_find_city(g, start);
    if (src < 0) {
        printf("[BFS] Sehir bulunamadi: %s\n", start);
        return;
    }

    int   visited[MAX_CITIES] = {0};
    Queue q;
    q_init(&q);

    visited[src] = 1;
    q_push(&q, src);

    printf("\n--- BFS Gezinme (Baslangic: %s) ---\n", start);
    int step = 0;

    while (!q_empty(&q)) {
        int cur = q_pop(&q);
        printf("  Adim %2d -> %s\n", ++step, g->cities[cur].name);

        /* Tum komşulari kuyruga ekle (ziyaret edilmemisse) */
        for (Edge *e = g->cities[cur].head; e; e = e->next) {
            if (!visited[e->to]) {
                visited[e->to] = 1;
                q_push(&q, e->to);
            }
        }
    }
    printf("\n");
}

/*
 * BFS tabanli en kisa yol (hop sayisi olarak)
 * parent[] dizisi ile geri izleme yapilir
 * Donus: 0 = yol bulundu, -1 = yol yok
 */
int graph_bfs_path(const Graph *g, const char *start, const char *end,
                   int path_out[], int *path_len)
{
    int src = graph_find_city(g, start);
    int dst = graph_find_city(g, end);
    if (src < 0 || dst < 0) return -1;
    if (src == dst) { path_out[0] = src; *path_len = 1; return 0; }

    int visited[MAX_CITIES] = {0};
    int parent [MAX_CITIES];
    for (int i = 0; i < MAX_CITIES; i++) parent[i] = -1;

    Queue q; q_init(&q);
    visited[src] = 1;
    q_push(&q, src);

    while (!q_empty(&q)) {
        int cur = q_pop(&q);
        if (cur == dst) break;
        for (Edge *e = g->cities[cur].head; e; e = e->next) {
            if (!visited[e->to]) {
                visited[e->to] = 1;
                parent[e->to]  = cur;
                q_push(&q, e->to);
            }
        }
    }

    if (parent[dst] == -1) return -1; /* Varis Subesie ulasilamiyor */

    /* Yolu ters siradan duzelt */
    int tmp[MAX_CITIES], tlen = 0;
    for (int v = dst; v != -1; v = parent[v]) tmp[tlen++] = v;

    *path_len = tlen;
    for (int i = 0; i < tlen; i++) path_out[i] = tmp[tlen - 1 - i];
    return 0;
}

/* Tum edge'lerin heap bellegini serbest birak */
void graph_free(Graph *g)
{
    for (int i = 0; i < g->city_count; i++) {
        Edge *e = g->cities[i].head;
        while (e) {
            Edge *next = e->next;
            free(e);
            e = next;
        }
        g->cities[i].head = NULL;
    }
    g->city_count = 0;
}

/*
 * Bellek Izleme Raporu - Graf
 * Her sehir dugumunun RAM adresi ve edge pointer zinciri gosterilir
 * Edge'ler heap'te (malloc) olustugu icin adresler dagitik olabilir
 */
void graph_memory_report(const Graph *g)
{
    printf("\n=== GRAF - BELLEK IZLEME RAPORU ===\n");
    printf("+----+--------------------+------------------+------------------------------+\n");
    printf("| %-2s | %-18s | %-16s | %-28s |\n",
           "No", "Dugum Adresi", "Sehir Adi", "Kenar Adresleri (ptr zinciri)");
    printf("+----+--------------------+------------------+------------------------------+\n");

    size_t toplam_edge = 0;

    for (int i = 0; i < g->city_count; i++) {
        const City *c = &g->cities[i];

        /* Ilk kenar adresini goster */
        char ilk_kenar[64] = "NULL";
        if (c->head)
            snprintf(ilk_kenar, sizeof(ilk_kenar), "%p", (void *)c->head);

        printf("| %2d | %18p | %-16s | %-28s |\n",
               i, (void *)c, c->name, ilk_kenar);

        /* Zincirdeki tum kenarlari listele */
        int eidx = 0;
        for (Edge *e = c->head; e; e = e->next) {
            toplam_edge += sizeof(Edge);
            char next_str[32];
            if (e->next)
                snprintf(next_str, sizeof(next_str), "%p", (void *)e->next);
            else
                strcpy(next_str, "NULL");

            printf("|    |                    | Kenar[%d]->%-6s | next: %-22s |\n",
                   eidx++, g->cities[e->to].name, next_str);
        }
    }

    printf("+----+--------------------+------------------+------------------------------+\n");
    printf("Graf adresi        : %p\n",   (void *)g);
    printf("City dizi boyutu   : %zu bayt (%d sehir x %zu)\n",
           sizeof(City) * (size_t)g->city_count, g->city_count, sizeof(City));
    printf("Toplam Edge bellek : %zu bayt (dinamik heap - malloc)\n\n", toplam_edge);
}