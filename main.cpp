#include "game_manager.h"
#include "game_threads.h"
#include <iostream>
#include <thread>
#include <vector>
#include <memory>

int main() {
    try {
        std::cout << "=== ЗАПУСК СИМУЛЯЦИИ NPC ===" << std::endl;
        std::cout << "Длительность игры: " << GAME_DURATION_SECONDS << " секунд" << std::endl;
        std::cout << "Размер карты: " << MAP_WIDTH << "x" << MAP_HEIGHT << std::endl;
        std::cout << "Начальное количество NPC: " << INITIAL_NPC_COUNT << std::endl;
        std::cout << "============================\n" << std::endl;
        
        // Создаем менеджер игры
        GameManager manager;
        
        // Инициализируем NPC
        std::cout << "Инициализация NPC..." << std::endl;
        manager.initializeNPCs(INITIAL_NPC_COUNT);
        
        // Создаем потоки
        std::cout << "Создание потоков..." << std::endl;
        
        // Поток перемещения
        MovementThread movement_thread(manager);
        
        // Несколько потоков для боев
        const int BATTLE_THREADS_COUNT = 3;
        std::vector<std::unique_ptr<BattleThread>> battle_threads;
        for (int i = 0; i < BATTLE_THREADS_COUNT; i++) {
            battle_threads.push_back(std::make_unique<BattleThread>(manager, i + 1));
        }
        
        // Поток отображения
        DisplayThread display_thread(manager);
        
        // Запускаем потоки
        std::cout << "Запуск потоков..." << std::endl;
        
        movement_thread.start();
        for (auto& thread : battle_threads) {
            thread->start();
        }
        display_thread.start();
        
        // Ждем завершения основного потока отображения
        display_thread.join();
        
        // Останавливаем и ждем остальные потоки
        std::cout << "\nЗавершение работы потоков..." << std::endl;
        manager.stopGame();
        
        movement_thread.join();
        for (auto& thread : battle_threads) {
            thread->join();
        }
        
        // Финальная очистка
        manager.removeDeadNPCs();
        
        std::cout << "\n=== СИМУЛЯЦИЯ ЗАВЕРШЕНА ===" << std::endl;
        std::cout << "Осталось NPC: " << manager.getNPCCount() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nОШИБКА: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nНЕИЗВЕСТНАЯ ОШИБКА" << std::endl;
        return 1;
    }
    
    return 0;
}