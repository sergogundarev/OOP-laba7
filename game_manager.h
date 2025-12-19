#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "npc.h"
#include "npc_types.h"
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <random>

// Константы игры
const int MAP_WIDTH = 100;
const int MAP_HEIGHT = 100;
const int GAME_DURATION_SECONDS = 30;
const int INITIAL_NPC_COUNT = 50;

// Задача на бой
struct BattleTask {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
    
    BattleTask(std::shared_ptr<NPC> a, std::shared_ptr<NPC> d) 
        : attacker(a), defender(d) {}
};

// Менеджер игры
class GameManager {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    mutable std::shared_mutex npcs_mutex;
    
    std::queue<BattleTask> battle_queue;
    std::mutex battle_queue_mutex;
    std::condition_variable battle_queue_cv;
    
    std::mutex console_mutex;
    
    std::atomic<bool> game_running{true};
    std::atomic<bool> battle_threads_running{true};
    
    std::random_device rd;
    std::mt19937 rng;
    
public:
    GameManager();
    ~GameManager();
    
    // Управление NPC
    void addNPC(std::shared_ptr<NPC> npc);
    void removeDeadNPCs();
    std::vector<std::shared_ptr<NPC>> getAliveNPCs() const;
    std::vector<std::shared_ptr<NPC>> getAllNPCs() const;
    size_t getNPCCount() const;
    
    // Управление очередью боев
    void addBattleTask(const BattleTask& task);
    BattleTask getBattleTask();
    bool hasBattleTasks() const;
    void stopBattleThreads();
    
    // Управление игрой
    bool isGameRunning() const;
    void stopGame();
    
    // Безопасный вывод
    void safePrint(const std::string& message);
    
    // Создание NPC
    std::shared_ptr<NPC> createRandomNPC(int id);
    void initializeNPCs(int count);
    
    // Утилиты
    std::unique_lock<std::mutex> getConsoleLock();
    int getRandomInt(int min, int max);
    
private:
    void cleanup();
};

#endif 
