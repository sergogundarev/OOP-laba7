#include "game_manager.h"
#include "game_threads.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

TEST(GameManagerTest, Initialization) {
    GameManager manager;
    
    EXPECT_TRUE(manager.isGameRunning());
    EXPECT_EQ(manager.getNPCCount(), 0);
}

TEST(GameManagerTest, AddAndRemoveNPC) {
    GameManager manager;
    
    auto npc = std::make_shared<NPC>(NPCType::ORC, "Тест", 10, 10);
    manager.addNPC(npc);
    
    EXPECT_EQ(manager.getNPCCount(), 1);
    
    auto npcs = manager.getAliveNPCs();
    EXPECT_EQ(npcs.size(), 1);
    EXPECT_EQ(npcs[0]->getName(), "Тест");
}

TEST(GameManagerTest, BattleQueue) {
    GameManager manager;
    
    auto attacker = std::make_shared<NPC>(NPCType::ORC, "Атакующий", 0, 0);
    auto defender = std::make_shared<NPC>(NPCType::DRUID, "Защитник", 1, 1);
    
    BattleTask task(attacker, defender);
    manager.addBattleTask(task);
    
    EXPECT_TRUE(manager.hasBattleTasks());
    
    auto retrieved_task = manager.getBattleTask();
    EXPECT_EQ(retrieved_task.attacker->getName(), "Атакующий");
    EXPECT_EQ(retrieved_task.defender->getName(), "Защитник");
    
    EXPECT_FALSE(manager.hasBattleTasks());
}

TEST(GameManagerTest, ThreadSafety) {
    GameManager manager;
    
    // Добавляем несколько NPC из разных потоков
    std::vector<std::thread> threads;
    const int THREAD_COUNT = 10;
    const int NPC_PER_THREAD = 5;
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.emplace_back([&manager, i]() {
            for (int j = 0; j < NPC_PER_THREAD; j++) {
                auto npc = std::make_shared<NPC>(
                    NPCType::ORC, 
                    "NPC_" + std::to_string(i) + "_" + std::to_string(j),
                    i * 10, j * 10
                );
                manager.addNPC(npc);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(manager.getNPCCount(), THREAD_COUNT * NPC_PER_THREAD);
}

TEST(GameManagerTest, SafePrint) {
    GameManager manager;
    
    // Тестируем безопасный вывод из нескольких потоков
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([&manager, i]() {
            for (int j = 0; j < 10; j++) {
                manager.safePrint("Сообщение " + std::to_string(i) + "_" + std::to_string(j));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

TEST(GameManagerTest, StopGame) {
    GameManager manager;
    
    EXPECT_TRUE(manager.isGameRunning());
    
    manager.stopGame();
    
    EXPECT_FALSE(manager.isGameRunning());
}

TEST(GameManagerTest, RemoveDeadNPCs) {
    GameManager manager;
    
    // Добавляем живых и мертвых NPC
    auto alive1 = std::make_shared<NPC>(NPCType::ORC, "Живой1", 0, 0);
    auto alive2 = std::make_shared<NPC>(NPCType::DRUID, "Живой2", 1, 1);
    auto dead = std::make_shared<NPC>(NPCType::KNIGHT, "Мертвый", 2, 2);
    
    dead->kill();
    
    manager.addNPC(alive1);
    manager.addNPC(alive2);
    manager.addNPC(dead);
    
    EXPECT_EQ(manager.getNPCCount(), 3);
    
    auto alive_npcs = manager.getAliveNPCs();
    EXPECT_EQ(alive_npcs.size(), 2);
    
    manager.removeDeadNPCs();
    
    EXPECT_EQ(manager.getNPCCount(), 2);
    
    alive_npcs = manager.getAliveNPCs();
    EXPECT_EQ(alive_npcs.size(), 2);
}

TEST(GameManagerTest, InitializeNPCs) {
    GameManager manager;
    
    manager.initializeNPCs(10);
    
    EXPECT_EQ(manager.getNPCCount(), 10);
    
    auto npcs = manager.getAllNPCs();
    for (const auto& npc : npcs) {
        EXPECT_TRUE(npc->isAlive());
        EXPECT_GE(npc->getX(), 0);
        EXPECT_LT(npc->getX(), MAP_WIDTH);
        EXPECT_GE(npc->getY(), 0);
        EXPECT_LT(npc->getY(), MAP_HEIGHT);
    }
}

TEST(GameManagerTest, BattleTaskProcessing) {
    GameManager manager;
    
    auto attacker = std::make_shared<NPC>(NPCType::ORC, "Атакующий", 0, 0);
    auto defender = std::make_shared<NPC>(NPCType::DRUID, "Защитник", 1, 1);
    
    // Добавляем задачу
    BattleTask task(attacker, defender);
    manager.addBattleTask(task);
    
    // Запускаем поток обработки
    bool task_processed = false;
    std::thread worker([&manager, &task_processed]() {
        auto task = manager.getBattleTask();
        if (task.attacker && task.defender) {
            task_processed = true;
        }
    });
    
    // Даем время на обработку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    manager.stopBattleThreads();
    worker.join();
    
    EXPECT_TRUE(task_processed);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}