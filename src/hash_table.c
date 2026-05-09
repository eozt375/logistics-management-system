#include "../include/hash_table.h"

/*
 * TIP A - Polinom Yuvarlayan Hash Fonksiyonu
 *
 * Formul: h(key) = ( toplam key[i] * 31^i ) mod TABLE_SIZE
 *
 * Neden BASE = 31?
 *   - Asal sayi oldugu icin mod islemi sonrasi duzgun dagilim saglar
 *   - Kucuk harf + rakam kombinasyonlari icin cakisma orani dusuktur
 *   - Pozisyon hassasiyeti: "abc" ve "cba" farkli hash uretir (anagram sorunu yok)
 *   - Java String.hashCode() ile ayni temel mantik, endustriyel standart
 */
unsigned int hash_type_a(const char *key)
{
    unsigned int hash  = 0;
    unsigned int base  = 31;
    unsigned int power = 1; /* 31^0 = 1 */

    for (int i = 0; key[i] != '\0'; i++) {
        hash  += (unsigned char)key[i] * power;
        power *= base;
        /* unsigned int tasimasi otomatik mod 2^32 islevi gorur */
    }
    return hash % TABLE_SIZE;
}

/* Hash tablosunu sifirla - tum slotlari bos olarak isaretler */
void ht_init(HashTable *ht)
{
    memset(ht, 0, sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++)
        ht->slots[i].active = 0;
}

/*
 * Ekleme - Acik Adresleme / Linear Probing
 *
 * Cakisma durumunda:
 *   probe(i) = (hash(key) + i) mod TABLE_SIZE,  i = 0, 1, 2, ...
 *
 * active == -1 olan slotlar (lazy delete) yeni ekleme icin kullanilabilir
 */
int ht_insert(HashTable *ht, Package pkg)
{
    if (ht->count >= TABLE_SIZE) {
        fprintf(stderr, "[HT] Tablo dolu!\n");
        return -1;
    }

    unsigned int idx   = hash_type_a(pkg.package_id);
    int          start = (int)idx;

    for (int i = 0; i < TABLE_SIZE; i++) {
        int probe = (start + i) % TABLE_SIZE; /* dogrusal adresleme */

        if (ht->slots[probe].active <= 0) {   /* bos veya silinmis slot */
            pkg.active           = 1;
            ht->slots[probe]     = pkg;
            ht->count++;
            printf("[HT] Eklendi: %s -> slot[%d] (probe adimi=%d)\n",
                   pkg.package_id, probe, i);
            return probe;
        }

        /* Ayni ID varsa guncelle */
        if (strcmp(ht->slots[probe].package_id, pkg.package_id) == 0) {
            pkg.active       = 1;
            ht->slots[probe] = pkg;
            printf("[HT] Guncellendi: %s -> slot[%d]\n", pkg.package_id, probe);
            return probe;
        }
    }

    fprintf(stderr, "[HT] Yerlestirilemedi: %s\n", pkg.package_id);
    return -1;
}

/*
 * Arama - Linear Probing ile ayni zinciri takip eder
 * active==0 gorulurse arama kesinlikle sonlanir (bos slot = zincir kopmus)
 * active==-1 gorulurse (lazy deleted) aramaya devam edilir
 */
Package *ht_search(HashTable *ht, const char *package_id)
{
    unsigned int idx   = hash_type_a(package_id);
    int          start = (int)idx;

    for (int i = 0; i < TABLE_SIZE; i++) {
        int probe = (start + i) % TABLE_SIZE;

        if (ht->slots[probe].active == 0)  /* bos slot - kayit yok */
            break;

        if (ht->slots[probe].active == 1 &&
            strcmp(ht->slots[probe].package_id, package_id) == 0)
            return &ht->slots[probe];
        /* active==-1 ise lazy deleted, aramaya devam */
    }
    return NULL;
}

/*
 * Silme - Lazy Deletion
 * Fiziksel silme yerine active=-1 yapilir
 * Boylece var olan probe zincirleri korunmus olur
 */
int ht_delete(HashTable *ht, const char *package_id)
{
    unsigned int idx   = hash_type_a(package_id);
    int          start = (int)idx;

    for (int i = 0; i < TABLE_SIZE; i++) {
        int probe = (start + i) % TABLE_SIZE;

        if (ht->slots[probe].active == 0) break;

        if (ht->slots[probe].active == 1 &&
            strcmp(ht->slots[probe].package_id, package_id) == 0) {
            ht->slots[probe].active = -1; /* lazy delete */
            ht->count--;
            printf("[HT] Silindi: %s (slot[%d])\n", package_id, probe);
            return 1;
        }
    }
    printf("[HT] Bulunamadi: %s\n", package_id);
    return 0;
}

/* Tum aktif kayitlari tablo formatinda yazdir */
void ht_print_all(const HashTable *ht)
{
    printf("\n+----+--------------+--------------+--------------+------------+\n");
    printf("| %-2s | %-12s | %-12s | %-12s | %-10s |\n",
           "No", "Paket ID", "Alici Sube", "Varis Subesi", "Durum");
    printf("+----+--------------+--------------+--------------+------------+\n");

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (ht->slots[i].active == 1) {
            printf("| %2d | %-12s | %-12s | %-12s | %-10s |\n",
                   i,
                   ht->slots[i].package_id,
                   ht->slots[i].origin,
                   ht->slots[i].destination,
                   ht->slots[i].status);
        } else {
            printf("| %2d | %-12s | %-12s | %-12s | %-10s |\n",
                   i, "---", "---", "---",
                   ht->slots[i].active == -1 ? "[SILINDI]" : "[BOS]");
        }
    }

    printf("+----+--------------+--------------+--------------+------------+\n");
    printf("Aktif kayit: %d / %d\n\n", ht->count, TABLE_SIZE);
}

/*
 * Bellek Izleme Raporu - Hash Tablosu
 * Her slotun RAM adresi (0x...), paket ID'si, durumu ve hash degeri gosterilir
 * En az 5 veri girisi sonrasi cagrilmali (odev gereksinimine gore)
 */
void ht_memory_report(const HashTable *ht)
{
    printf("\n=== HASH TABLOSU - BELLEK IZLEME RAPORU ===\n");
    printf("+----+--------------------+--------------+----------+------------------+\n");
    printf("| %-2s | %-18s | %-12s | %-8s | %-16s |\n",
           "No", "Bellek Adresi", "Paket ID", "Durum", "Hash(ID) mod 16");
    printf("+----+--------------------+--------------+----------+------------------+\n");

    for (int i = 0; i < TABLE_SIZE; i++) {
        const Package *p = &ht->slots[i];
        unsigned int   h = (p->active == 1) ? hash_type_a(p->package_id) : 0;

        printf("| %2d | %18p | %-12s | %-8s | %16u |\n",
               i,
               (void *)p,
               (p->active == 1)  ? p->package_id : "---",
               (p->active == 1)  ? "AKTIF"   :
               (p->active == -1) ? "SILINDI" : "BOS",
               (p->active == 1)  ? h : 0u);
    }

    printf("+----+--------------------+--------------+----------+------------------+\n");
    printf("HashTable adresi  : %p\n",   (void *)ht);
    printf("Toplam boyut      : %zu bayt\n", sizeof(HashTable));
    printf("Slot basina boyut : %zu bayt\n", sizeof(Package));
    printf("Aktif kayit       : %d / %d\n\n", ht->count, TABLE_SIZE);
}