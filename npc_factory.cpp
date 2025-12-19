#include "npc_factory.h"
#include <random>

std::unordered_map<std::string, NPCType> NPCFactory::string_to_type;
std::unordered_map<NPCType, std::string> NPCFactory::type_to_string;

void NPCFactory::initializeMaps() {
    static bool initialized = false;
    if (initialized) return;
    
    string_to_type = {
        {"ORC", NPCType::ORC},
        {"SQUIRREL", NPCType::SQUIRREL},
        {"DRUID", NPCType::DRUID},
        {"KNIGHT", NPCType::KNIGHT},
        {"ELF", NPCType::ELF},
        {"DRAGON", NPCType::DRAGON},
        {"BEAR", NPCType::BEAR},
        {"BANDIT", NPCType::BANDIT},
        {"WEREWOLF", NPCType::WEREWOLF},
        {"PRINCESS", NPCType::PRINCESS},
        {"TOAD", NPCType::TOAD},
        {"SLAVER", NPCType::SLAVER},
        {"PEGASUS", NPCType::PEGASUS},
        {"LOUSE", NPCType::LOUSE},
        {"DESMAN", NPCType::DESMAN},
        {"BULL", NPCType::BULL}
    };
    
    for (const auto& pair : string_to_type) {
        type_to_string[pair.second] = pair.first;
    }
    
    initialized = true;
}

std::shared_ptr<NPC> NPCFactory::createNPC(NPCType type, 
                                         const std::string& name, 
                                         int x, int y) {
    return std::make_shared<NPC>(type, name, x, y);
}

std::shared_ptr<NPC> NPCFactory::loadFromString(const std::string& data) {
    std::stringstream ss(data);
    std::string type_str, name;
    int x, y;
    
    ss >> type_str >> name >> x >> y;
    
    initializeMaps();
    auto it = string_to_type.find(type_str);
    if (it == string_to_type.end()) {
        return nullptr;
    }
    
    return createNPC(it->second, name, x, y);
}

std::string NPCFactory::saveToString(const std::shared_ptr<NPC>& npc) {
    initializeMaps();
    
    auto it = type_to_string.find(npc->getType());
    if (it == type_to_string.end()) {
        return "";
    }
    
    std::stringstream ss;
    ss << it->second << " "
       << npc->getName() << " "
       << npc->getX() << " "
       << npc->getY();
    
    return ss.str();
}

std::shared_ptr<NPC> NPCFactory::createRandomNPC(const std::string& base_name, 
                                                int x, int y) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    static const std::vector<NPCType> all_types = {
        NPCType::ORC, NPCType::SQUIRREL, NPCType::DRUID,
        NPCType::KNIGHT, NPCType::ELF, NPCType::DRAGON,
        NPCType::BEAR, NPCType::BANDIT, NPCType::WEREWOLF,
        NPCType::PRINCESS, NPCType::TOAD, NPCType::SLAVER,
        NPCType::PEGASUS, NPCType::LOUSE, NPCType::DESMAN,
        NPCType::BULL
    };
    
    std::uniform_int_distribution<> dist(0, all_types.size() - 1);
    NPCType type = all_types[dist(gen)];
    
    return createNPC(type, base_name, x, y);
}
