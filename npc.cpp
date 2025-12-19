#include "npc.h"
#include "npc_types.h"
#include <iostream>
#include <random>

NPC::NPC(NPCType type, const std::string& name, int x, int y) 
    : type(type), name(name), x(x), y(y), alive(true),
      rng(std::random_device{}()), dice(1, 6) {
}

std::string NPC::getName() const {
    std::lock_guard<std::mutex> lock(position_mutex);
    return name;
}

NPCType NPC::getType() const {
    std::lock_guard<std::mutex> lock(position_mutex);
    return type;
}

int NPC::getX() const {
    std::lock_guard<std::mutex> lock(position_mutex);
    return x;
}

int NPC::getY() const {
    std::lock_guard<std::mutex> lock(position_mutex);
    return y;
}

bool NPC::isAlive() const {
    return alive.load();
}

void NPC::kill() {
    alive.store(false);
}

void NPC::setPosition(int new_x, int new_y) {
    std::lock_guard<std::mutex> lock(position_mutex);
    x = new_x;
    y = new_y;
}

int NPC::rollDice() const {
    return dice(rng);
}

double NPC::distanceTo(const std::shared_ptr<NPC>& other) const {
    std::lock_guard<std::mutex> lock1(position_mutex);
    std::lock_guard<std::mutex> lock2(other->position_mutex);
    
    int dx = x - other->x;
    int dy = y - other->y;
    return std::sqrt(dx*dx + dy*dy);
}

void NPC::moveRandom() {
    if (!isAlive()) return;
    
    auto stats = getStats();
    if (stats.moveDistance <= 0) return;
    
    std::lock_guard<std::mutex> lock(position_mutex);
    
    // Генерируем случайное смещение
    std::uniform_int_distribution<> dist(-stats.moveDistance, stats.moveDistance);
    int dx = dist(rng);
    int dy = dist(rng);
    
    x += dx;
    y += dy;
    
    // Проверяем границы (0-100)
    if (x < 0) x = 0;
    if (x >= 100) x = 99;
    if (y < 0) y = 0;
    if (y >= 100) y = 99;
}

std::string NPC::getTypeString() const {
    auto it = NPC_TABLE.find(type);
    if (it != NPC_TABLE.end()) {
        return it->second.name;
    }
    return "Неизвестный";
}

char NPC::getSymbol() const {
    auto it = NPC_TABLE.find(type);
    if (it != NPC_TABLE.end()) {
        return it->second.symbol;
    }
    return '?';
}

NPCData NPC::getStats() const {
    auto it = NPC_TABLE.find(type);
    if (it != NPC_TABLE.end()) {
        return it->second;
    }
    return {0, 0, "Неизвестный", '?'};
}

void NPC::attack(std::shared_ptr<NPC> defender) {
    if (!isAlive() || !defender->isAlive()) return;
    
    if (!canKill(type, defender->getType())) return;
    
    int attack_power = rollDice();
    int defense_power = defender->rollDice();
    
    if (attack_power > defense_power) {
        defender->kill();
    }
}
