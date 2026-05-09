/*
 * main.c - Lojistik Yonetim Sistemi
 *
 * Ogrenci No : 64
 * Hash       : Tip A  (Polinom Yuvarlayan Hash, base=31)
 * Cakisma    : Acik Adresleme - Linear Probing  (son rakam 4, 0-4 arasi)
 * Gezinme    : BFS  (sondan 2. rakam 6, cift)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash_table.h"
#include "../include/graph.h"

/* Ana menu ekrani */
static void print_menu(void)
{
    printf("\n+---------------------------------------+\n");
    printf("|  LOJISTIK YONETIM SISTEMI  (No: 64)   |\n");
    printf("+---------------------------------------+\n");
    printf("| [1]  Paket Ekle                       |\n");
    printf("| [2]  Paket Ara                        |\n");
    printf("| [3]  Paket Sil                        |\n");
    printf("| [4]  Tum Paketleri Listele            |\n");
    printf("| [5]  Sehir Ekle                       |\n");
    printf("| [6]  Baglanti (Kenar) Ekle            |\n");
    printf("| [7]  Grafi Yazdir                     |\n");
    printf("| [8]  BFS Gezinme                      |\n");
    printf("| [9]  En Kisa Yol (BFS)                |\n");
    printf("| [10] Bellek Raporu - Hash Tablosu     |\n");
    printf("| [11] Bellek Raporu - Graf             |\n");
    printf("| [0]  Cikis                            |\n");
    printf("+---------------------------------------+\n");
    printf("Seciminiz: ");
}

/*
 * Demo verileri yukle
 * Odev gereksinimine gore en az 5 paket ve sehir ekleniyor
 * Bellek raporu bu verilerle anlamli sonuc verir
 */
static void load_demo(HashTable *ht, Graph *g)
{
    printf("\n[DEMO] Ornek veriler yukleniyor...\n\n");

    /* 7 paket - 5'ten fazla kayit (bellek raporu gereksinimine gore) */
    Package paketler[] = {
        {"PKG-001", "Istanbul", "Ankara",   3.5f, "Yolda",    1},
        {"PKG-002", "Ankara",   "Izmir",    1.2f, "Bekliyor", 1},
        {"PKG-003", "Izmir",    "Bursa",    5.0f, "Teslim",   1},
        {"PKG-004", "Bursa",    "Antalya",  2.8f, "Yolda",    1},
        {"PKG-005", "Antalya",  "Istanbul", 0.9f, "Bekliyor", 1},
        {"PKG-006", "Istanbul", "Izmir",    4.1f, "Yolda",    1},
        {"PKG-007", "Ankara",   "Bursa",    3.3f, "Teslim",   1}
    };

    for (int i = 0; i < 7; i++)
        ht_insert(ht, paketler[i]);

    /* Sehirler arasi yonlu baglantillar (her yonde ayri kenar) */
    graph_add_edge(g, "Istanbul", "Ankara",   450);
    graph_add_edge(g, "Ankara",   "Istanbul", 450);
    graph_add_edge(g, "Istanbul", "Izmir",    480);
    graph_add_edge(g, "Istanbul", "Bursa",   154);
    graph_add_edge(g, "Izmir",    "Istanbul", 480);
    graph_add_edge(g, "Ankara",   "Izmir",    590);
    graph_add_edge(g, "Izmir",    "Bursa",    330);
    graph_add_edge(g, "Bursa",    "Ankara",   400);
    graph_add_edge(g, "Bursa",    "Istanbul",   154);
    graph_add_edge(g, "Bursa",    "Antalya",  620);
    graph_add_edge(g, "Antalya",  "Istanbul", 710);

    printf("[DEMO] %d paket ve %d sehir yuklendi.\n", ht->count, g->city_count);
}

int main(void)
{
    HashTable ht;
    Graph     g;

    /* Yapilari sifirla */
    ht_init(&ht);
    memset(&g, 0, sizeof(g));

    /* Demo verileri yukle */
    load_demo(&ht, &g);

    int  secim;
    char buf[128];

    while (1) {
        print_menu();
        if (!fgets(buf, sizeof(buf), stdin)) break;
        secim = atoi(buf);

        switch (secim) {

        /* --- 1. Paket Ekle --- */
        case 1: {
            Package p;
            memset(&p, 0, sizeof(p));

            printf("Paket ID   : ");
            fgets(p.package_id, sizeof(p.package_id), stdin);
            p.package_id[strcspn(p.package_id, "\n")] = 0;

            printf("Alici Sube     : ");
            fgets(p.origin, sizeof(p.origin), stdin);
            p.origin[strcspn(p.origin, "\n")] = 0;

            printf("Varis Subesi      : ");
            fgets(p.destination, sizeof(p.destination), stdin);
            p.destination[strcspn(p.destination, "\n")] = 0;

            printf("Agirlik kg : ");
            fgets(buf, sizeof(buf), stdin);
            p.weight_kg = (float)atof(buf);

            printf("Durum      : ");
            fgets(p.status, sizeof(p.status), stdin);
            p.status[strcspn(p.status, "\n")] = 0;

            ht_insert(&ht, p);
            break;
        }

        /* --- 2. Paket Ara --- */
        case 2: {
            char id[MAX_PACKAGE_ID];
            printf("Paket ID: ");
            fgets(id, sizeof(id), stdin);
            id[strcspn(id, "\n")] = 0;

            Package *bulunan = ht_search(&ht, id);
            if (bulunan) {
                printf("\n[BULUNDU]\n");
                printf("  ID         : %s\n", bulunan->package_id);
                printf("  Alici Sube     : %s\n", bulunan->origin);
                printf("  Varis Subesi      : %s\n", bulunan->destination);
                printf("  Agirlik    : %.2f kg\n", bulunan->weight_kg);
                printf("  Durum      : %s\n", bulunan->status);
            } else {
                printf("Paket bulunamadi: %s\n", id);
            }
            break;
        }

        /* --- 3. Paket Sil --- */
        case 3: {
            char id[MAX_PACKAGE_ID];
            printf("Paket ID: ");
            fgets(id, sizeof(id), stdin);
            id[strcspn(id, "\n")] = 0;
            ht_delete(&ht, id);
            break;
        }

        /* --- 4. Tum Paketleri Listele --- */
        case 4:
            ht_print_all(&ht);
            break;

        /* --- 5. Sehir Ekle --- */
        case 5: {
            char sehir[MAX_CITY_LEN];
            printf("Sehir adi: ");
            fgets(sehir, sizeof(sehir), stdin);
            sehir[strcspn(sehir, "\n")] = 0;
            int idx = graph_add_city(&g, sehir);
            printf("[Graf] %s eklendi (indeks=%d)\n", sehir, idx);
            break;
        }

        /* --- 6. Kenar (Baglanti) Ekle --- */
        case 6: {
            char from[MAX_CITY_LEN], to[MAX_CITY_LEN];
            int  dist;

            printf("Alici Sube sehir  : ");
            fgets(from, sizeof(from), stdin);
            from[strcspn(from, "\n")] = 0;

            printf("Varis Subesi sehir   : ");
            fgets(to, sizeof(to), stdin);
            to[strcspn(to, "\n")] = 0;

            printf("Mesafe (km)   : ");
            fgets(buf, sizeof(buf), stdin);
            dist = atoi(buf);

            graph_add_edge(&g, from, to, dist);
            printf("[Graf] Kenar eklendi: %s -> %s (%d km)\n", from, to, dist);
            break;
        }

        /* --- 7. Grafi Yazdir --- */
        case 7:
            graph_print(&g);
            break;

        /* --- 8. BFS Gezinme --- */
        case 8: {
            char baslangic[MAX_CITY_LEN];
            printf("Baslangic sehri: ");
            fgets(baslangic, sizeof(baslangic), stdin);
            baslangic[strcspn(baslangic, "\n")] = 0;
            graph_bfs(&g, baslangic);
            break;
        }

        /* --- 9. En Kisa Yol (BFS) --- */
        case 9: {
            char from[MAX_CITY_LEN], to[MAX_CITY_LEN];

            printf("Alici Sube sehir: ");
            fgets(from, sizeof(from), stdin);
            from[strcspn(from, "\n")] = 0;

            printf("Varis Subesi sehir : ");
            fgets(to, sizeof(to), stdin);
            to[strcspn(to, "\n")] = 0;

            int path[MAX_CITIES], plen = 0;
            if (graph_bfs_path(&g, from, to, path, &plen) == 0) {
                printf("\nEn Kisa Yol (%s -> %s):\n  ", from, to);
                for (int i = 0; i < plen; i++) {
                    printf("%s", g.cities[path[i]].name);
                    if (i < plen - 1) printf(" -> ");
                }
                printf("\n(%d durak)\n", plen - 1);
            } else {
                printf("Yol bulunamadi.\n");
            }
            break;
        }

        /* --- 10. Bellek Raporu - Hash Tablosu --- */
        case 10:
            ht_memory_report(&ht);
            break;

        /* --- 11. Bellek Raporu - Graf --- */
        case 11:
            graph_memory_report(&g);
            break;

        /* --- 0. Cikis --- */
        case 0:
            printf("Cikiliyor...\n");
            graph_free(&g); /* heap bellegi temizle */
            return 0;

        default:
            printf("Gecersiz secim.\n");
        }
    }

    graph_free(&g);
    return 0;
}