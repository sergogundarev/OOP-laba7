#ifndef NPC_H
#define NPC_H

#include "npc_types.h"
#include <memory>
#include <string>
#include <cmath>
#include <mutex>
#include <atomic>
#include <random>

// Базовый класс NPC
class NPC {
protected:
    std::string name;
    int x, y;
    std::atomic<bool> alive;
    NPCType type;
    mutable std::mutex position_mutex;
    
    // Для бросков кубика (у каждого NPC свой генератор)
    mutable std::mt19937 rng;
    mutable std::uniform_int_distribution<int> dice;
    
public:
    NPC(NPCType type, const std::string& name, int x, int y);
    virtual ~NPC() = default;
    
    // Геттеры с блокировкой
    std::string getName() const;
    NPCType getType() const;
    int getX() const;
    int getY() const;
    bool isAlive() const;
    
    // Методы с блокировкой
    void kill();
    void setPosition(int new_x, int new_y);
    
    // Бросок кубика (потокобезопасный)
    int rollDice() const;
    
    // Расстояние до другого NPC
    double distanceTo(const std::shared_ptr<NPC>& other) const;
    
    // Перемещение в случайном направлении
    void moveRandom();
    
    // Информация о NPC
    virtual std::string getTypeString() const;
    char getSymbol() const;
    NPCData getStats() const;
    
    // Для паттерна Visitor
    virtual void attack(std::shared_ptr<NPC> defender);
};

#endif
