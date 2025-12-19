#include "npc_types.h"
#include <algorithm>

std::string NPCTypeToString(NPCType type) {
    auto it = NPC_TABLE.find(type);
    if (it != NPC_TABLE.end()) {
        return it->second.name;
    }
    return "Неизвестный";
}

NPCType stringToNPCType(const std::string& str) {
    if (str == "ORC") return NPCType::ORC;
    if (str == "SQUIRREL") return NPCType::SQUIRREL;
    if (str == "DRUID") return NPCType::DRUID;
    if (str == "KNIGHT") return NPCType::KNIGHT;
    if (str == "ELF") return NPCType::ELF;
    if (str == "DRAGON") return NPCType::DRAGON;
    if (str == "BEAR") return NPCType::BEAR;
    if (str == "BANDIT") return NPCType::BANDIT;
    if (str == "WEREWOLF") return NPCType::WEREWOLF;
    if (str == "PRINCESS") return NPCType::PRINCESS;
    if (str == "TOAD") return NPCType::TOAD;
    if (str == "SLAVER") return NPCType::SLAVER;
    if (str == "PEGASUS") return NPCType::PEGASUS;
    if (str == "LOUSE") return NPCType::LOUSE;
    if (str == "DESMAN") return NPCType::DESMAN;
    if (str == "BULL") return NPCType::BULL;
    
    return NPCType::ORC; // По умолчанию
}

bool canKill(NPCType attacker, NPCType defender) {
    // attacker -> может убить defender
    
    switch (attacker) {
        case NPCType::ORC:
            // Орк убивает всех
            return true;
            
        case NPCType::SQUIRREL:
            // Белка убивает только...
            return defender == NPCType::TOAD;
            
        case NPCType::DRUID:
            // Друид никого не убивает
            return false;
            
        case NPCType::KNIGHT:
            // Рыцарь убивает...
            return defender == NPCType::ORC || 
                   defender == NPCType::BEAR ||
                   defender == NPCType::BANDIT ||
                   defender == NPCType::WEREWOLF ||
                   defender == NPCType::SLAVER;
            
        case NPCType::ELF:
            // Эльф убивает...
            return defender == NPCType::DRAGON ||
                   defender == NPCType::WEREWOLF;
            
        case NPCType::DRAGON:
            // Дракон убивает...
            return defender == NPCType::KNIGHT ||
                   defender == NPCType::ELF ||
                   defender == NPCType::BEAR ||
                   defender == NPCType::BANDIT ||
                   defender == NPCType::WEREWOLF ||
                   defender == NPCType::PRINCESS ||
                   defender == NPCType::TOAD ||
                   defender == NPCType::SLAVER ||
                   defender == NPCType::PEGASUS ||
                   defender == NPCType::LOUSE ||
                   defender == NPCType::DESMAN ||
                   defender == NPCType::BULL;
            
        case NPCType::BEAR:
            // Медведь убивает...
            return defender == NPCType::SQUIRREL ||
                   defender == NPCType::PRINCESS ||
                   defender == NPCType::TOAD ||
                   defender == NPCType::LOUSE;
            
        case NPCType::BANDIT:
            // Разбойник убивает...
            return defender == NPCType::DRUID ||
                   defender == NPCType::PRINCESS ||
                   defender == NPCType::SLAVER;
            
        case NPCType::WEREWOLF:
            // Оборотень убивает...
            return defender == NPCType::SQUIRREL ||
                   defender == NPCType::DRUID ||
                   defender == NPCType::PRINCESS ||
                   defender == NPCType::TOAD ||
                   defender == NPCType::LOUSE ||
                   defender == NPCType::DESMAN;
            
        case NPCType::PRINCESS:
            // Принцесса убивает...
            return defender == NPCType::TOAD;
            
        case NPCType::TOAD:
            // Жаба убивает...
            return defender == NPCType::LOUSE ||
                   defender == NPCType::DESMAN;
            
        case NPCType::SLAVER:
            // Работорговец убивает...
            return defender == NPCType::DRUID;
            
        case NPCType::PEGASUS:
            // Пегас убивает...
            return defender == NPCType::SQUIRREL ||
                   defender == NPCType::DRUID ||
                   defender == NPCType::WEREWOLF ||
                   defender == NPCType::TOAD ||
                   defender == NPCType::LOUSE ||
                   defender == NPCType::DESMAN;
            
        case NPCType::LOUSE:
            // Вышь убивает...
            return defender == NPCType::PRINCESS ||
                   defender == NPCType::TOAD ||
                   defender == NPCType::DESMAN;
            
        case NPCType::DESMAN:
            // Выхухоль убивает...
            return defender == NPCType::SQUIRREL ||
                   defender == NPCType::TOAD ||
                   defender == NPCType::LOUSE;
            
        case NPCType::BULL:
            // Бык убивает...
            return defender == NPCType::KNIGHT ||
                   defender == NPCType::DRUID ||
                   defender == NPCType::BEAR ||
                   defender == NPCType::WEREWOLF ||
                   defender == NPCType::SLAVER;
            
        default:
            return false;
    }
}
