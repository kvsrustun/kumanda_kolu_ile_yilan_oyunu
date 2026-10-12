# 🐍 Deneyap Kumanda Kolu ile Yılan Oyunu (5x7 Dot Matrix LED)

<p align="center">
  <a href="#donanim-bilesenleri"><img src="https://img.shields.io/badge/Donanım-Deneyap%20Kart%201A%20%28ESP32%29-24292e?style=flat-square&logo=espressif&logoColor=white" alt="Donanım" /></a>
  <a href="#donanim-bilesenleri"><img src="https://img.shields.io/badge/Girdi-Deneyap%20Kumanda%20Kolu%20(0x1B)-24292e?style=flat-square&logo=nintendogamecube&logoColor=white" alt="Joystick" /></a>
  <a href="#donanim-bilesenleri"><img src="https://img.shields.io/badge/Ekran-5x7%20Dot%20Matrix%20LED%20(0x0A)-24292e?style=flat-square&logo=arduino&logoColor=white" alt="LED Matris" /></a>
  <a href="#iletisim-protokolu"><img src="https://img.shields.io/badge/İletişim-I2C%20Haberleşme-24292e?style=flat-square" alt="I2C" /></a>
  <a href="#lisans"><img src="https://img.shields.io/badge/Lisans-MIT-24292e?style=flat-square&logo=open-source-initiative&logoColor=white" alt="Lisans" /></a>
</p>

<p align="center">
  <strong>Deneyap Kumanda Kolu (Joystick) ve Deneyap 5x7 Dot Matrix LED ekran modülü kullanılarak I2C veriyolu üzerinde geliştirilmiş retro yılan (Snake) oyunu projesi.</strong>
</p>

---

## 📌 İçindekiler
- [Proje Genel Bakış](#proje-genel-bakis)
- [Öne Çıkan Özellikler](#one-cikan-ozellikler)
- [Donanım Mimarisi ve I2C Bağlantıları](#donanim-bilesenleri)
- [Oyun Dinamikleri ve Kontrol Eşikleri](#oyun-dinamikleri)
- [Yazılım Mimarisi ve Çekirdek Mantık](#yazilim-mimarisi)
- [Proje Dosya Yapısı](#dosya-yapisi)
- [Kurulum ve Çalıştırma Rehberi](#kurulum-ve-calistirma)
- [Sorun Giderme (Troubleshooting)](#sorun-giderme)
- [Geliştirme ve Yol Haritası](#gelistirme-ve-yol-haritasi)
- [Lisans](#lisans)

---

<a id="proje-genel-bakis"></a>
## 📖 1. Proje Genel Bakış

Bu proje, **Deneyap Kart 1A** mikrodenetleyicisi üzerinde **Deneyap 5x7 LED Ekran** modülünü retro bir oyun ekranına, **Deneyap Kumanda Kolu** modülünü ise hassas bir oyun kumandasına dönüştürür.

Sistem, I2C seri iletişim protokolünü kullanarak minimum kablo kalabalığıyla donanımlar arası yüksek hızlı veri transferi gerçekleştirir. Oyuncu, analog kumanda kolunu 4 ana yönde (Yukarı, Aşağı, Sol, Sağ) yönlendirerek 5 sütun ve 7 satırdan oluşan LED matris üzerinde yılanı yönetir; duvara veya kendi gövdesine çarpmadan hedef noktaya ulaşmaya çalışır.

---

<a id="one-cikan-ozellikler"></a>
## ⚡ 2. Öne Çıkan Özellikler

- **Tam I2C Haberleşme Mimarisi:** Kumanda kolu (`0x1B`) ve 5x7 LED matris (`0x0A`) ortak SDA/SCL hatlarını paylaşarak GPIO tasarrufu sağlar.
- **Analog Eşik Filtreleme:** Joystick eksen değerleri (`0-1023` aralığı, merkez ~500) `470` ve `520` eşik filtreleriyle hassas ve kararlı yön girdilerine dönüştürülür.
- **Gövde & Çarpışma Fiziği:** Yılanın her adımı `Point` koordinat dizisinde kaydırılır; sınır dışına çıkma ve kendi kuyruğuna çarpma durumları anlık denetlenir.
- **Bitmap Çizim Motoru:** Ekran titreşimini önlemek için oyun alanı önce bellekteki `bitmap[35]` dizisine işlenir, ardından tek hamlede LED matrise aktarılır.
- **Oyun Sonu ve Yeniden Başlatma:** Çarpma veya hedefe ulaşma sonrasında joystick hareketiyle oyun sıfırlanarak tekrar başlatılabilir.

---

<a id="donanim-bilesenleri"></a>
## 🛠️ 3. Donanım Mimarisi ve I2C Bağlantıları

| Bileşen | Model / Modül | I2C Adresi | Görevi |
| :--- | :--- | :---: | :--- |
| **Mikrodenetleyici** | Deneyap Kart 1A (ESP32) | Master | Oyun mantığı ve grafik hesaplama |
| **Kumanda Kolu** | Deneyap Kumanda Kolu (Joystick) | `0x1B` | 2 Eksenli yön girdisi ve hareket tespiti |
| **Ekran Modülü** | Deneyap 5x7 Dot Matrix LED Ekran | `0x0A` | 35 piksellik oyun sahnesi gösterimi |
| **Bağlantı Kablosu** | I2C Haberleşme Kablosu (F/F) | - | SDA, SCL, 3.3V, GND veri/besleme hattı |

<a id="iletisim-protokolu"></a>
### 🔌 Pin Bağlantı Şeması

Tüm çevre birimleri I2C veriyolunu paylaştığından Deneyap Kart 1A üzerindeki I2C portuna zincirleme veya çoklayıcı ile bağlanır:

| Modül Pini | Deneyap Kart 1A Pini | Fonksiyon |
| :---: | :---: | :--- |
| **VCC** | `3.3V` | Modül pozitif çalışma gerilimi |
| **GND** | `GND` | Ortak toprak hattı |
| **SDA** | `SDA` (`D0`) | I2C Seri Veri İletim Hattı |
| **SCL** | `SCL` (`D1`) | I2C Seri Saat Hattı |

> 💡 **İpucu:** Deneyap modülleri I2C kabloları ile doğrudan birbirine veya kart üzerindeki I2C konnektörüne tak-çalıştır biçimde bağlanabilir.

---

<a id="oyun-dinamikleri"></a>
## 🎮 4. Oyun Dinamikleri ve Kontrol Eşikleri

### Kumanda Kolu (Joystick) Eksen Kalibrasyonu
Kumanda kolunun 10-bit analog okuması (0 - 1023) merkezde yaklaşık `495-505` bandındadır. İstenmeyen titremeleri engellemek için şu histerezis eşikleri tanımlanmıştır:

| Eksen | Okunan Değer | Algılanan Yön | Koordinat Değişimi |
| :---: | :---: | :---: | :---: |
| **X Ekseni** | `xVal > 520` | ➡️ **Sağ** | `dx = +1` |
| **X Ekseni** | `xVal < 470` | ⬅️ **Sol** | `dx = -1` |
| **Y Ekseni** | `yVal > 520` | ⬆️ **Yukarı** | `dy = -1` |
| **Y Ekseni** | `yVal < 470` | ⬇️ **Aşağı** | `dy = +1` |
| **Merkez (Ölü Bölge)** | `470 ≤ Değer ≤ 520` | ⏺️ **Sabit** | `dx = 0, dy = 0` |

### Ekran Matrisi ve Başlangıç Konumları
- **Matris Boyutu:** 5 sütun (X: 0..4) x 7 satır (Y: 0..6) = Toplam 35 LED
- **Yılan Başlangıç Boyu:** 3 parça `[(1,3), (0,3), (0,2)]`
- **Hedef / Yem Noktası:** Sabit çıkış noktası `(4, 6)`

---

<a id="yazilim-mimarisi"></a>
## 💻 5. Yazılım Mimarisi ve Çekirdek Mantık

> 📄 **Kaynak Kod:** Projenin eksiksiz Arduino/C++ kaynak koduna depodaki [`kumanda_kolu_ile_yilan_oyunu.ino`](./kumanda_kolu_ile_yilan_oyunu.ino) dosyasından ulaşabilirsiniz.

### 🎛️ Çekirdek Hareket ve Çarpışma Denetimi (Core Logic Snippet)

Joystick girdisine göre yeni koordinatların hesaplanması, sınır/kuyruk çarpışma testi ve oyun akışı:

```cpp
// Yeni kafa koordinatlarını hesapla
int newX = snake[0].x + dx;
int newY = snake[0].y + dy;

// Sınır kontrolü (5x7) ve kendi gövdesine çarpma kontrolü
if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && !isSnake(newX, newY)) {
    moveSnake(newX, newY);
    drawGame();
    lastDrawTime = millis();

    // Hedefe (çıkış noktasına) ulaşma kontrolü
    if (newX == food.x && newY == food.y) {
        Serial.println("Tebrikler, çıkış noktasına ulaştınız! Oyun bitti.");
        gameOver = true;
    }
} else {
    // Duvara veya kendi kuyruğuna çarpma
    Serial.println("Çarpma! Oyun bitti.");
    gameOver = true;
}
```

### ⚙️ Modüler Fonksiyon Analizi

| Fonksiyon | Görevi | Detay |
| :--- | :--- | :--- |
| `setup()` | `0x1B` ve `0x0A` I2C modüllerini başlatır, başlangıç pozisyonunu çizer. | Modül bulunamazsa sonsuz döngüde bekler. |
| `loop()` | Joystick okur, yön belirler, çarpışma yoksa yılanı ilerletir. | 20 ms yenileme aralığı ile çalışır. |
| `moveSnake(x, y)` | Yılanın gövde parçalarını bir gerideki konuma kaydırıp kafayı yeni noktaya koyar. | Dizi elemanlarını geriye doğru öteleme mantığı. |
| `isSnake(x, y)` | Verilen koordinatın yılan gövdesinde olup olmadığını sorgular. | Kendini ısırma (self-collision) denetimi. |
| `drawGame()` | Yem ve yılan piksellerini bellekteki bitmap'e yazar, matrise basar. | `ledMatrix.drawLedMatrix(bitmap)` kullanır. |
| `resetGame()` | Yılanı başlangıç uzunluğuna ve koordinatlarına sıfırlar. | Oyun bittikten sonra ilk harekette çağrılır. |

---

<a id="dosya-yapisi"></a>
## 📁 6. Proje Dosya Yapısı

```bash
kumanda_kolu_ile_yilan_oyunu/
│
├── kumanda_kolu_ile_yilan_oyunu.ino   # Ana Arduino/Deneyap kaynak kodu
├── .gitattributes                     # Git yapılandırma ayarları
├── README.md                           # Detaylı teknik dokümantasyon
└── LICENSE                            # MIT Açık Kaynak Lisansı
```

---

<a id="kurulum-ve-calistirma"></a>
## 🚀 7. Kurulum ve Çalıştırma Rehberi

### Gereksinimler
1. **Arduino IDE:** [arduino.cc](https://www.arduino.cc/en/software) adresinden en güncel sürümü indirin.
2. **Deneyap Geliştirme Kartı Desteği:**
   - Arduino IDE > `Dosya` > `Tercihler` > `Ek Devre Kartları Yöneticisi URL'leri` bölümüne Deneyap paket URL'sini ekleyin.
   - `Araçlar` > `Kart` > `Kart Yöneticisi` arama çubuğuna **Deneyap** yazıp kurun.
3. **Gerekli Kütüphaneler:**
   - `Deneyap_KumandaKolu` (Deneyap Kart kütüphane paketi ile birlikte gelir)
   - `Deneyap_5x7LedEkran` (Deneyap Kart kütüphane paketi ile birlikte gelir)

### Yükleme Adımları
1. Depoyu bilgisayarınıza klonlayın:
   ```bash
   git clone https://github.com/kvsrustun/kumanda_kolu_ile_yilan_oyunu.git
   ```
2. `kumanda_kolu_ile_yilan_oyunu.ino` dosyasını Arduino IDE ile açın.
3. `Araçlar` > `Kart` menüsünden **Deneyap Kart 1A** seçeneğini seçin.
4. Kartın bağlı olduğu Port'u belirleyin.
5. **Yükle** (Sağ ok butonu) ile kodu karta yükleyin.
6. 115200 baud oranında Seri Port Ekranı'nı açarak durum loglarını takip edin.

---

<a id="sorun-giderme"></a>
## 🔍 8. Sorun Giderme (Troubleshooting)

- **"Joystick başlatılamadı!" Hatası:**
  - Kumanda kolu I2C kablosunun Deneyap Kart'a sağlam takıldığını kontrol edin.
  - Adresin `0x1B` olduğundan emin olun.
- **"LED Matris başlatılamadı!" Hatası:**
  - 5x7 LED ekran modülü I2C bağlantısını ve adresini (`0x0A`) kontrol edin.
- **Yılan Kendi Kendine Hareket Ediyor:**
  - Kumanda kolu yay mekanizması merkezde kalmıyorsa kod içerisindeki `470` ve `520` eşik değerlerini daraltıp genişletebilirsiniz (Örn: `450` ve `550`).
- **LED'ler Yanmıyor:**
  - Besleme hattının `3.3V` olduğunu ve GND hattının ortaklandığını doğrulayın.

---

<a id="gelistirme-ve-yol-haritasi"></a>
## 🔮 9. Geliştirme ve Yol Haritası

- [ ] Yem yendikçe uzayan dinamik yılan mekanizması ve rastgele (`random()`) yem üretimi
- [ ] Buzzer entegrasyonu ile hareket, yem yeme ve yanma ses efektleri
- [ ] Skor sayacı ve EEPROM tabanlı en yüksek skor (High Score) kaydı
- [ ] Seviye atladıkça hızlanan kademeli zorluk sistemi

---

<a id="lisans"></a>
## 📄 10. Lisans

Bu proje **[MIT Lisansı](LICENSE)** altında lisanslanmıştır. Eğitim, hobi ve akademik projelerde serbestçe kullanılabilir.
