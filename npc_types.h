#ifndef NPC_TYPES_H
#define NPC_TYPES_H

#include <string>
#include <unordered_map>


enum class NPCType {
    ORC,           // Орк
    SQUIRREL,      // Белка
    DRUID,         // Друид
    KNIGHT,        // Странствующий рыцарь
    ELF,           // Эльф
    DRAGON,        // Дракон
    BEAR,          // Медведь
    BANDIT,        // Разбойник
    WEREWOLF,      // Оборотень
    PRINCESS,      // Принцесса
    TOAD,          // Жаба
    SLAVER,        // Работорговец
    PEGASUS,       // Пегас
    LOUSE,         // Вышь
    DESMAN,        // Выхухоль
    BULL           // Бык
};

// Структура с данными NPC из таблицы
struct NPCData {
    int moveDistance;   // Расстояние хода
    int killDistance;   // Расстояние убийства
    std::string name;   // Русское название
    char symbol;        // Символ для карты
};

// Таблица характеристик
const std::unordered_map<NPCType, NPCData> NPC_TABLE = {
    {NPCType::ORC,       {20, 10, "Орк", 'O'}},
    {NPCType::SQUIRREL,  {5,  5,  "Белка", 'S'}},
    {NPCType::DRUID,     {10, 10, "Друид", 'D'}},
    {NPCType::KNIGHT,    {30, 10, "Рыцарь", 'K'}},
    {NPCType::ELF,       {10, 50, "Эльф", 'E'}},
    {NPCType::DRAGON,    {50, 30, "Дракон", 'R'}},
    {NPCType::BEAR,      {5,  10, "Медведь", 'B'}},
    {NPCType::BANDIT,    {10, 10, "Разбойник", 'A'}},
    {NPCType::WEREWOLF,  {40, 5,  "Оборотень", 'W'}},
    {NPCType::PRINCESS,  {1,  1,  "Принцесса", 'P'}},
    {NPCType::TOAD,      {1,  10, "Жаба", 'T'}},
    {NPCType::SLAVER,    {10, 10, "Работорговец", 'L'}},
    {NPCType::PEGASUS,   {30, 10, "Пегас", 'G'}},
    {NPCType::LOUSE,     {50, 10, "Вышь", 'V'}},
    {NPCType::DESMAN,    {5,  20, "Выхухоль", 'H'}},
    {NPCType::BULL,      {30, 10, "Бык", 'U'}}
};

// Таблица кто кого может убить (attacker -> {defenders})
const std::unordered_map<NPCType, std::vector<NPCType>> KILL_TABLE = {
    {NPCType::ORC,       {NPCType::SQUIRREL, NPCType::DRUID, NPCType::KNIGHT,
                         NPCType::ELF, NPCType::DRAGON, NPCType::BEAR,
                         NPCType::BANDIT, NPCType::WEREWOLF, NPCType::PRINCESS,
                         NPCType::TOAD, NPCType::SLAVER, NPCType::PEGASUS,
                         NPCType::LOUSE, NPCType::DESMAN, NPCType::BULL}},
    {NPCType::SLAVER,    {NPCType::DRUID}},
    {NPCType::DRAGON,    {NPCType::KNIGHT, NPCType::ELF, NPCType::BEAR,
                         NPCType::BANDIT, NPCType::WEREWOLF, NPCType::PRINCESS,
                         NPCType::TOAD, NPCType::SLAVER, NPCType::PEGASUS,
                         NPCType::LOUSE, NPCType::DESMAN, NPCType::BULL}},
};

// Функции для работы с типами
std::string NPCTypeToString(NPCType type);
NPCType stringToNPCType(const std::string& str);
bool canKill(NPCType attacker, NPCType defender);

#endif // NPC_TYPES_H