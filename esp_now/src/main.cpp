#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>

typedef esp_now_peer_info_t esp_peer;

struct package {
    int value = 0;
};

esp_peer config_peer(const uint8_t* mac) {
    esp_now_peer_info_t peerInfo;
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    memcpy(peerInfo.peer_addr, mac, 6);
    return peerInfo;
}

void on_data_send(const uint8_t* mac, esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS) {                                 
        Serial.println("FAILED TO SENT\n");
    } 
}

const uint8_t peer_mac[] = {0xC4, 0x4F, 0x33, 0x24, 0x4F, 0x49};
//const uint8_t peer_mac[] = {0x08, 0xD1, 0xF9, 0xE1, 0xF1, 0x24};

esp_peer peer = config_peer(peer_mac);

void on_data_rcv(const uint8_t *mac, const uint8_t *data_rcv, int len) {
    if (memcmp(mac, peer_mac, 6) != 0) return;

    package* data = (package*)data_rcv;
    // trata dados
    Serial.print("DATA RECEIVED: ");
    Serial.println(data->value);
}

void setup() {
    Serial.begin(115200);
    // disable wifi
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);

    // init esp now
    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro na inicializacao do ESP-NOW");
        delay(2000);
        ESP.restart();
    }

    esp_now_register_send_cb(on_data_send);
    esp_now_register_recv_cb(on_data_rcv);

    if (esp_now_add_peer(&peer) != ESP_OK) {
        Serial.print("Falha ao adicionar peer");
        delay(2500);
        ESP.restart();
    }
}

void send_data(package data) {
    esp_now_send(peer_mac, (uint8_t*)&data, sizeof(package));
}

void loop() { 
    package dado;
    dado.value = 5;
    send_data(dado);
    delay(1000);
}