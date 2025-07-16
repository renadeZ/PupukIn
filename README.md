# PupukIn

**PupukIn** adalah sistem rekomendasi pupuk presisi berbasis sensor NPK tanah, dirancang untuk membantu petani dan penyuluh mengoptimalkan dosis Urea, SP‑36, dan KCl sesuai kondisi nyata lahan.

---

## 🚀 Fitur Utama

- **Pembacaan Sensor 7‑in‑1 RS485**  
  Mengukur konsentrasi Nitrogen (N), Phosphorus (P), dan Kalium (K) dalam satuan mg/kg secara real‑time.

- **Konversi Otomatis ke Kadar Unsur (kg/ha)**  
  Menggunakan rumus standar (kedalaman 20 cm & bulk density berdasarkan jenis tanah) untuk menghitung kadar unsur hara.

- **Rule‑Based Recommendation Engine**  
  - **Kekurangan** → hitung dosis pupuk (Urea, SP‑36, KCl)  
  - **Optimal** → status “cukup”, tanpa tambahan  
  - **Kelebihan** → rekomendasi pencucian atau rotasi  

- **Dual Monitoring Interface**  
  - **OLED 128×64**: menampilkan dosis pupuk dan status  
  - **Aplikasi Mobile via Bluetooth**: menampilkan hasil rekomendasi dan notifikasi “Perlu Pemupukan”

- **Dukungan Tanah Andisol & Non‑Andisol**  
  Dataset dosis pupuk disesuaikan dengan karakteristik kedua tipe tanah berdasarkan literatur peer‑review.

- **Kinerja Ringan & Cepat**  
  - Waktu eksekusi rule & konversi: **~23 ms**  
  - Penggunaan memori pada ESP32‑WROOM DevKit V1: **12.5% SRAM** & **36.3% Flash**

---
