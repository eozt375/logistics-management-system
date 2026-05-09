# Lojistik Yönetim Sistemi – Öğrenci No: 64

## Teknik Seçimler (No: 64)

| Kriter | Açıklama |
|---|---|
| Hash Fonksiyonu | **Tip A** – Çift numara (64 çift) |
| Çakışma Çözümü | **Açık Adresleme** – Son rakam 4 (0-4 arası) |
| Graf Gezinme | **BFS** – Sondan 2. rakam 6 (çift) |

---

## Proje Yapısı

```
logistics_system/
├── include/
│   ├── hash_table.h    # Hash tablosu arayüzü
│   └── graph.h         # Graf arayüzü
├── src/
│   ├── hash_table.c    # Tip A hash + Açık Adresleme
│   ├── graph.c         # Komşuluk listesi + BFS
│   └── main.c          # Ana program / menü
├── Makefile
└── README.md
```

---

## Hash Fonksiyonu – Tip A

**Polynomial Rolling Hash (Polinom Yuvarlayan Hash)**

```
h(key) = ( Σ key[i] × 31^i ) mod TABLE_SIZE
```

- **BASE = 31**: Asal sayı; küçük harf alfabe + rakam kombinasyonu için düşük çakışma oranı sağlar.
- Her karakter, pozisyonuna göre farklı ağırlıkla katkı yapar → anagram sorununu önler.
- `unsigned int` taşması `mod 2^32` işlevi görür, ardından `mod TABLE_SIZE` uygulanır.

### Neden Tip A olarak bu seçim?

| Özellik | Değer |
|---|---|
| Dağılım kalitesi | Yüksek (çakışma az) |
| Hesaplama hızı | O(n) – anahtar uzunluğu |
| Anagram ayrımı | ✔ (pozisyon hassas) |
| Basitlik | ✔ (tek döngü) |

---

## Çakışma Çözümü – Açık Adresleme (Linear Probing)

Çakışma oluştuğunda:
```
probe(i) = (hash(key) + i) mod TABLE_SIZE,  i = 0, 1, 2, ...
```

- **Silme**: Lazy Deletion (active = -1) – arama zinciri korunur.
- **Yük faktörü**: count / TABLE_SIZE < 1 olduğu sürece ekleme garantili.

---

## Graf – BFS (Genişlik Öncelikli Arama)

```
1. Başlangıç düğümünü kuyruğa ekle, ziyaret et.
2. Kuyruk boş değilse:
   a. Baştan düğüm çıkar.
   b. Tüm ziyaret edilmemiş komşularını kuyruğa ekle.
3. En kısa yol: parent[] dizisiyle geri izleme.
```

Şehirler arası bağlantılar **komşuluk listesi** (linked-list) ile tutulur.

---

## Bellek İzleme Raporu

Program çalıştırıldığında seçenek 10 (Hash) ve 11 (Graf) ile gerçek RAM adresleri (`0x...`) ve pointer zinciri görüntülenir.

### Hash Tablosu Bellek Özeti

| Alan | Değer |
|---|---|
| HashTable toplam | `sizeof(HashTable)` = 3204 bayt |
| Slot başına | `sizeof(Package)` = 200 bayt |
| Slot sayısı | 16 |
| Yapı türü | Stack (statik dizi) |

### Graf Bellek Özeti

| Alan | Değer |
|---|---|
| City dizisi | `city_count × sizeof(City)` |
| Her Edge | `sizeof(Edge)` = 24 bayt, **heap** (malloc) |
| Pointer zinciri | `head → next → ... → NULL` |

---

## Derleme ve Çalıştırma

```bash
make          # Derle
make run      # Derle + çalıştır
make clean    # Temizle
```

---

## Menü Seçenekleri

| No | İşlev |
|---|---|
| 1 | Paket Ekle |
| 2 | Paket Ara |
| 3 | Paket Sil |
| 4 | Tüm Paketleri Listele |
| 5 | Şehir Ekle |
| 6 | Kenar (Bağlantı) Ekle |
| 7 | Graf Yazdır |
| 8 | BFS Gezinme |
| 9 | En Kısa Yol (BFS) |
| 10 | Hash Bellek Raporu |
| 11 | Graf Bellek Raporu |
| 0 | Çıkış |
