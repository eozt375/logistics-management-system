#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TABLE_SIZE     16
#define MAX_PACKAGE_ID 32
#define MAX_CITY_NAME  64
#define MAX_STATUS     32

/* Paket kaydi */
typedef struct {
    char  package_id[MAX_PACKAGE_ID]; /* Paket kimlik numarasi       */
    char  origin[MAX_CITY_NAME];      /* Cikis sehri                 */
    char  destination[MAX_CITY_NAME]; /* Varis sehri                 */
    float weight_kg;                  /* Agirlik (kg)                */
    char  status[MAX_STATUS];         /* Durum (Yolda, Teslim vb.)   */
    int   active;                     /* 1=dolu, 0=bos, -1=silinmis  */
} Package;

/* Hash tablosu yapisi (Acik Adresleme - Linear Probing) */
typedef struct {
    Package slots[TABLE_SIZE]; /* Sabit boyutlu slot dizisi */
    int     count;             /* Aktif kayit sayisi        */
} HashTable;

/*
 * Tip A Hash Fonksiyonu - Polinom Yuvarlayan Hash
 * h(key) = ( toplam: key[i] * 31^i ) mod TABLE_SIZE
 * BASE=31 secildi: asal sayi, alfanumerik anahtarlar icin dusuk cakisma
 */
unsigned int hash_type_a(const char *key);

/* Temel islemler */
void     ht_init         (HashTable *ht);
int      ht_insert       (HashTable *ht, Package pkg);
Package *ht_search       (HashTable *ht, const char *package_id);
int      ht_delete       (HashTable *ht, const char *package_id);
void     ht_print_all    (const HashTable *ht);
void     ht_memory_report(const HashTable *ht);

#endif /* HASH_TABLE_H */