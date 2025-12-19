#ifndef NPC_FACTORY_H
#define NPC_FACTORY_H

#include "npc.h"
#include "npc_types.h"
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>

class NPCFactory {
public:
    // Создание NPC по типу
    static std::shared_ptr<NPC> createNPC(NPCType type, 
                                         const std::string& name, 
                                         int x, int y);
    
    // Загрузка из строки
    static std::shared_ptr<NPC> loadFromString(const std::string& data);
    
    // Сохранение в строку
    static std::string saveToString(const std::shared_ptr<NPC>& npc);
    
    // Создание случайного NPC
    static std::shared_ptr<NPC> createRandomNPC(const std::string& base_name, 
                                               int x, int y);
    
private:
    static std::unordered_map<std::string, NPCType> string_to_type;
    static std::unordered_map<NPCType, std::string> type_to_string;
    
    static void initializeMaps();
};

#endif