#pragma once

#include "data/PlayerData.h"

// SECCION: Datos locales
// Sistema auxiliar para guardar datos simples del jugador en archivo local.
class DataManager {
private:
    // Archivo local usado como respaldo; no reemplaza la API.
    const char* saveFilePath = "save_data.txt";

public:
    // FUNCION: Carga datos locales o crea valores por defecto si no existe archivo.
    PlayerData loadPlayerData();
    // FUNCION: Guarda el estado local actual del jugador.
    void savePlayerData(const PlayerData& data);

    // FUNCIONES: Puntos preparados para registrar eventos si se amplian datos locales.
    void registerGameStarted(int userId);
    void registerGameFinished(int userId, int score, int coinsCollected);
    void registerCoinCollected(int userId, int amount);
};