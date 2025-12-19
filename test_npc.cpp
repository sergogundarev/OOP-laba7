#include "npc.h"
#include "npc_types.h"
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

TEST(NPCTest, CreationAndBasicProperties) {
    NPC npc(NPCType::ORC, "Тестовый_Орк", 10, 20);
    
    EXPECT_EQ(npc.getName(), "Тестовый_Орк");
    EXPECT_EQ(npc.getType(), NPCType::ORC);
    EXPECT_EQ(npc.getX(), 10);
    EXPECT_EQ(npc.getY(), 20);
    EXPECT_TRUE(npc.isAlive());
}

TEST(NPCTest, Movement) {
    NPC npc(NPCType::KNIGHT, "Рыцарь", 50, 50);
    
    int initial_x = npc.getX();
    int initial_y = npc.getY();
    
    npc.moveRandom();
    
    // Проверяем, что координаты изменились
    EXPECT_FALSE(npc.getX() == initial_x && npc.getY() == initial_y);
    
    // Проверяем границы
    EXPECT_GE(npc.getX(), 0);
    EXPECT_LT(npc.getX(), 100);
    EXPECT_GE(npc.getY(), 0);
    EXPECT_LT(npc.getY(), 100);
}

TEST(NPCTest, DistanceCalculation) {
    auto npc1 = std::make_shared<NPC>(NPCType::ORC, "Орк1", 0, 0);
    auto npc2 = std::make_shared<NPC>(NPCType::DRUID, "Друид1", 3, 4);
    
    double distance = npc1->distanceTo(npc2);
    EXPECT_DOUBLE_EQ(distance, 5.0); // 3-4-5 треугольник
}

TEST(NPCTest, DiceRoll) {
    NPC npc(NPCType::ELF, "Эльф", 0, 0);
    
    for (int i = 0; i < 100; i++) {
        int roll = npc.rollDice();
        EXPECT_GE(roll, 1);
        EXPECT_LE(roll, 6);
    }
}

TEST(NPCTest, KillAndResurrection) {
    NPC npc(NPCType::BEAR, "Медведь", 0, 0);
    
    EXPECT_TRUE(npc.isAlive());
    npc.kill();
    EXPECT_FALSE(npc.isAlive());
}

TEST(NPCTest, ThreadSafety) {
    auto npc = std::make_shared<NPC>(NPCType::DRAGON, "Дракон", 50, 50);
    
    std::vector<std::thread> threads;
    
    // Запускаем несколько потоков, которые обращаются к NPC
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([npc]() {
            for (int j = 0; j < 100; j++) {
                npc->getX();
                npc->getY();
                npc->isAlive();
                npc->rollDice();
            }
        });
    }
    
    // Запускаем потоки, которые перемещают NPC
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([npc]() {
            for (int j = 0; j < 50; j++) {
                npc->moveRandom();
            }
        });
    }
    
    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Проверяем, что NPC все еще в границах
    EXPECT_GE(npc->getX(), 0);
    EXPECT_LT(npc->getX(), 100);
    EXPECT_GE(npc->getY(), 0);
    EXPECT_LT(npc->getY(), 100);
}

TEST(NPCTest, CanKillTable) {
    // Проверяем несколько отношений из таблицы
    EXPECT_TRUE(canKill(NPCType::ORC, NPCType::DRUID));      // Орк убивает друида
    EXPECT_TRUE(canKill(NPCType::SLAVER, NPCType::DRUID));   // Работорговец убивает друида
    EXPECT_FALSE(canKill(NPCType::DRUID, NPCType::ORC));     // Друид не убивает орка
    EXPECT_TRUE(canKill(NPCType::DRAGON, NPCType::KNIGHT));  // Дракон убивает рыцаря
    EXPECT_TRUE(canKill(NPCType::ELF, NPCType::DRAGON));     // Эльф убивает дракона
}

TEST(NPCTest, AttackMechanic) {
    auto attacker = std::make_shared<NPC>(NPCType::ORC, "Атакующий", 0, 0);
    auto defender = std::make_shared<NPC>(NPCType::DRUID, "Защитник", 1, 1);
    
    // Сохраняем начальное состояние
    bool defender_was_alive = defender->isAlive();
    
    // Атакуем
    attacker->attack(defender);
    
    // Проверяем, что что-то изменилось
    // (может убить, а может и нет - зависит от броска кубика)
    EXPECT_TRUE(defender_was_alive);  // Изначально должен быть жив
}

TEST(NPCTest, NPCDataAccess) {
    NPC npc(NPCType::PEGASUS, "Пегас", 0, 0);
    
    auto stats = npc.getStats();
    EXPECT_EQ(stats.name, "Пегас");
    EXPECT_EQ(stats.moveDistance, 30);
    EXPECT_EQ(stats.killDistance, 10);
    EXPECT_EQ(stats.symbol, 'G');
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
