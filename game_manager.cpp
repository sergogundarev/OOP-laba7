#include "game_manager.h"
#include <iostream>
#include <algorithm>
#include <sstream>

GameManager::GameManager() : rng(rd()) {
    npcs.reserve(INITIAL_NPC_COUNT * 2);
}

GameManager::~GameManager() {
    cleanup();
}

void GameManager::addNPC(std::shared_ptr<NPC> npc) {
    std::unique_lock<std::shared_mutex> lock(npcs_mutex);
    npcs.push_back(npc);
}

void GameManager::removeDeadNPCs() {
    std::unique_lock<std::shared_mutex> lock(npcs_mutex);
    auto it = std::remove_if(npcs.begin(), npcs.end(),
        [](const std::shared_ptr<NPC>& npc) {
            return !npc->isAlive();
        });
    npcs.erase(it, npcs.end());
}

std::vector<std::shared_ptr<NPC>> GameManager::getAliveNPCs() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex);
    std::vector<std::shared_ptr<NPC>> alive_npcs;
    
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            alive_npcs.push_back(npc);
        }
    }
    
    return alive_npcs;
}

std::vector<std::shared_ptr<NPC>> GameManager::getAllNPCs() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex);
    return npcs;
}

size_t GameManager::getNPCCount() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex);
    return npcs.size();
}

void GameManager::addBattleTask(const BattleTask& task) {
    {
        std::lock_guard<std::mutex> lock(battle_queue_mutex);
        battle_queue.push(task);
    }
    battle_queue_cv.notify_one();
}

BattleTask GameManager::getBattleTask() {
    std::unique_lock<std::mutex> lock(battle_queue_mutex);
    
    battle_queue_cv.wait(lock, [this]() {
        return !battle_queue.empty() || !battle_threads_running;
    });
    
    if (!battle_threads_running && battle_queue.empty()) {
        return BattleTask(nullptr, nullptr);
    }
    
    BattleTask task = battle_queue.front();
    battle_queue.pop();
    return task;
}

bool GameManager::hasBattleTasks() const {
    std::lock_guard<std::mutex> lock(battle_queue_mutex);
    return !battle_queue.empty();
}

void GameManager::stopBattleThreads() {
    battle_threads_running = false;
    battle_queue_cv.notify_all();
}

bool GameManager::isGameRunning() const {
    return game_running;
}

void GameManager::stopGame() {
    game_running = false;
    stopBattleThreads();
}

void GameManager::safePrint(const std::string& message) {
    std::lock_guard<std::mutex> lock(console_mutex);
    std::cout << message << std::endl;
}

std::shared_ptr<NPC> GameManager::createRandomNPC(int id) {
    // Случайный тип NPC
    static const std::vector<NPCType> all_types = {
        NPCType::ORC, NPCType::SQUIRREL, NPCType::DRUID,
        NPCType::KNIGHT, NPCType::ELF, NPCType::DRAGON,
        NPCType::BEAR, NPCType::BANDIT, NPCType::WEREWOLF,
        NPCType::PRINCESS, NPCType::TOAD, NPCType::SLAVER,
        NPCType::PEGASUS, NPCType::LOUSE, NPCType::DESMAN,
        NPCType::BULL
    };
    
    std::uniform_int_distribution<> type_dist(0, all_types.size() - 1);
    NPCType type = all_types[type_dist(rng)];
    
    // Случайное имя
    static const std::vector<std::string> names = {
        "Арагорн", "Гендальф", "Леголас", "Гимли", "Фродо",
        "Сэм", "Мерри", "Пиппин", "Боромир", "Фарамир",
        "Эомер", "Теоден", "Эовин", "Галадриэль", "Элронд",
        "Саурон", "Саруман", "Голлум", "Билбо", "Торин"
    };
    
    std::uniform_int_distribution<> name_dist(0, names.size() - 1);
    std::string name = names[name_dist(rng)] + "_" + std::to_string(id);
    
    // Случайные координаты
    std::uniform_int_distribution<> coord_dist(0, MAP_WIDTH - 1);
    int x = coord_dist(rng);
    int y = coord_dist(rng);
    
    return std::make_shared<NPC>(type, name, x, y);
}

void GameManager::initializeNPCs(int count) {
    for (int i = 0; i < count; i++) {
        auto npc = createRandomNPC(i);
        addNPC(npc);
    }
    
    std::stringstream ss;
    ss << "Создано " << count << " NPC";
    safePrint(ss.str());
}

std::unique_lock<std::mutex> GameManager::getConsoleLock() {
    return std::unique_lock<std::mutex>(console_mutex);
}

int GameManager::getRandomInt(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    return dist(rng);
}

void GameManager::cleanup() {
    stopGame();
    
    // Очищаем очереди
    {
        std::lock_guard<std::mutex> lock(battle_queue_mutex);
        while (!battle_queue.empty()) {
            battle_queue.pop();
        }
    }
    
    // Очищаем NPC
    {
        std::unique_lock<std::shared_mutex> lock(npcs_mutex);
        npcs.clear();
    }
}
