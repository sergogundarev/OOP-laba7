#ifndef GAME_THREADS_H
#define GAME_THREADS_H

#include "game_manager.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

// Поток для перемещения NPC
class MovementThread {
private:
    GameManager& manager;
    std::thread thread;
    
public:
    MovementThread(GameManager& mgr) : manager(mgr) {}
    
    void start() {
        thread = std::thread([this]() { run(); });
    }
    
    void join() {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
private:
    void run() {
        while (manager.isGameRunning()) {
            // Получаем живых NPC
            auto npcs = manager.getAliveNPCs();
            
            // Перемещаем каждого NPC
            for (auto& npc : npcs) {
                if (!manager.isGameRunning()) break;
                
                // Перемещение
                npc->moveRandom();
                
                // Проверяем другие NPC в радиусе убийства
                auto stats = npc->getStats();
                
                for (auto& other : npcs) {
                    if (npc == other) continue;
                    
                    double distance = npc->distanceTo(other);
                    if (distance <= stats.killDistance) {
                        // Проверяем возможность убийства
                        if (canKill(npc->getType(), other->getType())) {
                            // Создаем задачу на бой
                            BattleTask task(npc, other);
                            manager.addBattleTask(task);
                            
                            // Логирование
                            std::stringstream ss;
                            ss << npc->getName() << " (" << npc->getTypeString()
                               << ") обнаружил " << other->getName() << " ("
                               << other->getTypeString() << ") на расстоянии "
                               << distance;
                            manager.safePrint(ss.str());
                        }
                    }
                }
            }
            
            // Задержка перед следующим циклом
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

// Поток для обработки боев
class BattleThread {
private:
    GameManager& manager;
    int id;
    std::thread thread;
    
public:
    BattleThread(GameManager& mgr, int thread_id) 
        : manager(mgr), id(thread_id) {}
    
    void start() {
        thread = std::thread([this]() { run(); });
    }
    
    void join() {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
private:
    void run() {
        while (manager.isGameRunning()) {
            // Получаем задачу на бой
            BattleTask task = manager.getBattleTask();
            
            if (!task.attacker || !task.defender) {
                continue;
            }
            
            // Проверяем, что оба NPC еще живы
            if (!task.attacker->isAlive() || !task.defender->isAlive()) {
                continue;
            }
            
            // Выполняем бой
            task.attacker->attack(task.defender);
            
            // Логирование результата
            std::stringstream ss;
            if (task.defender->isAlive()) {
                ss << "БОЙ " << id << ": " << task.attacker->getName()
                   << " не смог убить " << task.defender->getName();
            } else {
                ss << "БОЙ " << id << ": " << task.attacker->getName()
                   << " убил " << task.defender->getName();
            }
            manager.safePrint(ss.str());
        }
    }
};

// Основной поток для отображения
class DisplayThread {
private:
    GameManager& manager;
    std::thread thread;
    
public:
    DisplayThread(GameManager& mgr) : manager(mgr) {}
    
    void start() {
        thread = std::thread([this]() { run(); });
    }
    
    void join() {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
private:
    void run() {
        auto start_time = std::chrono::steady_clock::now();
        
        while (manager.isGameRunning()) {
            // Проверяем время
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                current_time - start_time).count();
            
            if (elapsed >= GAME_DURATION_SECONDS) {
                manager.stopGame();
                break;
            }
            
            // Отображаем карту
            displayMap(elapsed);
            
            // Ждем 1 секунду
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        // Финальная статистика
        displayFinalStatistics();
    }
    
    void displayMap(int elapsed_seconds) {
        auto lock = manager.getConsoleLock();
        
        auto npcs = manager.getAliveNPCs();
        
        std::cout << "\n\n=== КАРТА ИГРЫ (секунда " << elapsed_seconds 
                  << "/" << GAME_DURATION_SECONDS << ") ===" << std::endl;
        std::cout << "Живых NPC: " << npcs.size() << std::endl;
        std::cout << "Задач в очереди боев: " << (manager.hasBattleTasks() ? "Да" : "Нет") 
                  << std::endl;
        
        // Создаем карту 20x20 для отображения (центрированную)
        const int DISPLAY_WIDTH = 20;
        const int DISPLAY_HEIGHT = 20;
        
        std::vector<std::vector<char>> map(
            DISPLAY_HEIGHT, 
            std::vector<char>(DISPLAY_WIDTH, '.')
        );
        
        // Заполняем карту NPC
        for (const auto& npc : npcs) {
            int x = npc->getX();
            int y = npc->getY();
            
            // Переводим в координаты отображения (центрируем)
            int display_x = (x * DISPLAY_WIDTH) / MAP_WIDTH;
            int display_y = (y * DISPLAY_HEIGHT) / MAP_HEIGHT;
            
            if (display_x >= 0 && display_x < DISPLAY_WIDTH &&
                display_y >= 0 && display_y < DISPLAY_HEIGHT) {
                map[display_y][display_x] = npc->getSymbol();
            }
        }
        
        // Выводим карту
        std::cout << "\nЛегенда: ";
        for (const auto& pair : NPC_TABLE) {
            std::cout << pair.second.symbol << "-" << pair.second.name.substr(0, 3) << " ";
        }
        std::cout << "\n\n";
        
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                std::cout << map[y][x];
            }
            std::cout << std::endl;
        }
        
        // Выводим список NPC
        std::cout << "\nСписок NPC:" << std::endl;
        int count = 0;
        for (const auto& npc : npcs) {
            if (count++ >= 10) {  // Ограничиваем вывод
                std::cout << "... и еще " << (npcs.size() - 10) << " NPC" << std::endl;
                break;
            }
            std::cout << "- " << npc->getName() << " (" << npc->getTypeString()
                      << ") [" << npc->getX() << "," << npc->getY() << "]" << std::endl;
        }
    }
    
    void displayFinalStatistics() {
        auto lock = manager.getConsoleLock();
        
        auto npcs = manager.getAliveNPCs();
        
        std::cout << "\n\n=== ИГРА ОКОНЧЕНА ===" << std::endl;
        std::cout << "Длительность: " << GAME_DURATION_SECONDS << " секунд" << std::endl;
        std::cout << "Выжившие NPC: " << npcs.size() << std::endl;
        
        if (!npcs.empty()) {
            std::cout << "\nСписок выживших:" << std::endl;
            
            // Группируем по типам
            std::unordered_map<NPCType, int> type_counts;
            for (const auto& npc : npcs) {
                type_counts[npc->getType()]++;
            }
            
            for (const auto& pair : type_counts) {
                auto stats = NPC_TABLE.find(pair.first)->second;
                std::cout << "- " << stats.name << ": " << pair.second << std::endl;
            }
            
            std::cout << "\nПодробный список:" << std::endl;
            for (const auto& npc : npcs) {
                std::cout << "  " << npc->getName() << " ("
                          << npc->getTypeString() << ") на позиции ["
                          << npc->getX() << "," << npc->getY() << "]" << std::endl;
            }
        } else {
            std::cout << "\nВсе NPC погибли!" << std::endl;
        }
    }
};

#endif