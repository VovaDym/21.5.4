#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <cctype>
#include <fstream>

const int gBgSize = 40;

enum Way
{
    WAY_TOP,
    WAY_BOTTOM,
    WAY_LEFT,
    WAY_RIGHT,
    WAY_VARIABLES
};

struct Pos
{
    int y = 0;
    int x = 0;
};

struct Character
{
    bool alive = true;
    std::string name = "none";
    int hp = 0;
    int armor = 0;
    int dmg = 0;
    Pos position;
};

void printBattleGround(std::string(*bg)[gBgSize], std::vector <Character> &players);
int enterNumber();
Character generateCharacter(int, const int, const int, const int,
    const int, const int, const int);
bool isAttack(Character& characterAttack, Character& characterDefence);
void dealDamage(Character& characterAttack, Character& characterDefence);
bool isCorrectMove(Character& character, int move);
void moveTurnBack(Character& character, int way);
void save(std::vector<Character>& players);
void load(std::vector<Character>& players);

int main()
{
    std::srand(std::time(0));
    std::string battleGround[gBgSize][gBgSize];
    const int minHP = 50;
    const int maxHP = 150;
    const int minArmor = 0;
    const int maxArmor = 50;
    const int minDmg = 15;
    const int maxDmg = 30;
    int enemyCount;

    // creating a player
    std::cout << "Enter name of your character: ";
    Character player;
    std::getline(std::cin, player.name);
    system("cls");
    std::cout << "Enter max HP of your character (average: " << (minHP + maxHP) / 2 << "): ";
    std::cin >> player.hp;
    std::cout << "Enter max Armor of your character (average: " << (minArmor + maxArmor) / 2 << "): ";
    std::cin >> player.armor;
    std::cout << "Enter damage of your character (average: " << (minDmg + maxDmg) / 2 << "): ";
    std::cin >> player.dmg;
    player.position.x = rand() % gBgSize;
    player.position.y = rand() % gBgSize;

    std::vector <Character> players;
    players.push_back(player);
    

    // creating enemies
    std::cout << "Enter count of enemys: ";
    std::cin >> enemyCount;

    for (int i = 1;i <= enemyCount; ++i)
    {
       players.push_back(generateCharacter(i, minHP, maxHP, minArmor, maxArmor, minDmg, maxDmg));
    } 
    

    // checking the uniqueness of the player's position
    bool match = true;
    while (match)
    {
        match = false;
        for (int i = 0; i < (players.size() - 1); ++i)
        {
            if ((players[i].position.x == players[i + 1].position.x) &&
                (players[i].position.y == players[i + 1].position.y))
            {
                players[i].position.x = rand() % gBgSize;
                players[i].position.y = rand() % gBgSize;
                match = true;
            }
        }
    }

    // start game
    system("cls");
    for (int i = 0; i <= enemyCount; ++i)
    {
        std::cout << players[i].name << "\t hp:" << players[i].hp << "\t armor:" << players[i].armor << "\t dmg:" << players[i].dmg << std::endl;
    }
    bool playerAlive = true;
    bool enemysAlive = true;
    while (playerAlive && enemysAlive)
    {
        printBattleGround(battleGround, players);

        // player turn
        std::string moveVariables[] = { "top", "bottom", "left", "right" };
        std::string command[] = { "save", "load" };

        std::cout << "Enter one of the way below:" << std::endl;
        for (const auto& mv : moveVariables)
            std::cout << "    " << mv << std::endl;

        std::cout << "Or command of game:" << std::endl;
        for (const auto& c : command)
            std::cout << "    " << c << std::endl;

        std::string action = "";
        int way;
        bool matchMove = false;
        bool matchCommand = false;

        while (!matchMove && !matchCommand)
        {
            std::getline(std::cin, action);
            for (int i = 0; i < action.length(); ++i)
                action[i] = std::tolower(action[i]);

            int size = sizeof(command) / sizeof(command[0]);
            for (int i = 0; !matchCommand && i < size; ++i)
                if (action == command[i])
                {
                    system("cls");
                    matchCommand = true;
                    (i == 0) ? save(players) : load(players);
                }

            size = sizeof(moveVariables) / sizeof(moveVariables[0]);
            for (int i = 0; !matchCommand && !matchMove && i < size; ++i)
                if (action == moveVariables[i])
                {
                    matchMove = true;
                    way = i;
                }
        }

        if (matchMove)
        {
            system("cls");
            

            if (isCorrectMove(players[0], way))
            {
                for (int i = 1; i < players.size(); ++i)
                    if (isAttack(players[0], players[i]))
                    {
                        dealDamage(players[0], players[i]);
                        moveTurnBack(players[0], way);
                    }
            }

            // enemies ' move
            for (int i = 1; i < players.size(); ++i)
            {
                if (players[i].alive)
                {
                    way = rand() % WAY_VARIABLES;
                    if (isCorrectMove(players[i], way))
                    {
                        if (isAttack(players[i], players[0]))
                        {
                            dealDamage(players[i], players[0]);
                            moveTurnBack(players[i], way);
                        }
                        for (int j = 1; j < players.size(); ++j)
                            if ((j != i) && isAttack(players[i], players[j]))
                                moveTurnBack(players[i], way);
                    }
                }
            }
            for (int i = 0; i <= enemyCount; ++i)
            {
                std::cout << players[i].name << "\t hp:" << players[i].hp << "\t armor:" << players[i].armor << "\t dmg:" << players[i].dmg << std::endl;
            }

            // ñheck win
            playerAlive = players[0].alive;

            enemysAlive = false;
            for (int i = 1; (i < players.size()) && !enemysAlive; ++i)
                if (players[i].alive)
                    enemysAlive = true;
        }
    }

    printBattleGround(battleGround, players);
    std::cout << ((playerAlive) ? "Player " : "Enemy ") << "win " << std::endl << std::endl;
    system("pause");
    
}

void save(std::vector<Character>& players)
{
    system("cls");
    std::cout << "Enter name of your saving file: ";
    std::string name;
    std::getline(std::cin, name);
    system("cls");

    std::ofstream file(name + ".bin", std::ios::binary);
    for (const Character& ch : players)
    {
        int len = ch.name.length();
        file.write((char*)&len, sizeof(len));
        file.write(ch.name.c_str(), len);
        file.write((char*)&ch.alive, sizeof(ch.alive));
        file.write((char*)&ch.hp, sizeof(ch.hp));
        file.write((char*)&ch.armor, sizeof(ch.armor));
        file.write((char*)&ch.dmg, sizeof(ch.dmg));
        file.write((char*)&ch.position.x, sizeof(ch.position.x));
        file.write((char*)&ch.position.y, sizeof(ch.position.y));
    }
    file.close();

    std::cout << "Game saved" << std::endl << std::endl;
}

void load(std::vector<Character>& players)
{
    system("cls");
    std::cout << "Enter name of your loading file: ";
    std::string name;
    std::getline(std::cin, name);
    system("cls");

    std::ifstream file(name + ".bin", std::ios::binary);
    if (file.is_open())
    {
        std::vector<Character>().swap(players);
        Character ch;
        int len;
        while (file.read((char*)&len, sizeof(len)))
        {
            ch.name.resize(len);
            file.read((char*)ch.name.c_str(), len);
            file.read((char*)&ch.alive, sizeof(ch.alive));
            file.read((char*)&ch.hp, sizeof(ch.hp));
            file.read((char*)&ch.armor, sizeof(ch.armor));
            file.read((char*)&ch.dmg, sizeof(ch.dmg));
            file.read((char*)&ch.position.x, sizeof(ch.position.x));
            file.read((char*)&ch.position.y, sizeof(ch.position.y));

            players.push_back(ch);
        }
        file.close();

        std::cout << "Game loaded" << std::endl << std::endl;
    }
    else
    {
        std::cout << "Save wasn't found" << std::endl;
        std::cout << "Game continues" << std::endl << std::endl;
    }
}

void moveTurnBack(Character& character, int way)
{
    switch (way)
    {
    case WAY_TOP:    ++character.position.y; break;
    case WAY_BOTTOM: --character.position.y; break;
    case WAY_LEFT:   ++character.position.x; break;
    case WAY_RIGHT:  --character.position.x; break;
    }
}

bool isCorrectMove(Character& character, int move)
{
    switch (move)
    {
    case WAY_TOP:    --character.position.y; break;
    case WAY_BOTTOM: ++character.position.y; break;
    case WAY_LEFT:   --character.position.x; break;
    case WAY_RIGHT:  ++character.position.x; break;
    }

    if (character.position.x < 0 || character.position.x >= gBgSize)
    {
        character.position.x += (move == WAY_LEFT) ? 1 : -1;
        return false;
    }
    if (character.position.y < 0 || character.position.y >= gBgSize)
    {
        character.position.y += (move == WAY_TOP) ? 1 : -1;
        return false;
    }

    return true;
}

bool isAttack(Character& characterAttack, Character& characterDefence)
{
    return (characterDefence.alive &&
        characterAttack.position.x == characterDefence.position.x &&
        characterAttack.position.y == characterDefence.position.y);
}

void dealDamage(Character& characterAttack, Character& characterDefence)
{
    
    characterDefence.armor -= characterAttack.dmg;
    if (characterDefence.armor < 0)
    {
        characterDefence.hp += characterDefence.armor;
        characterDefence.armor = 0;
    }
    if (characterDefence.hp <= 0)
    {
        characterDefence.hp = 0;
        characterDefence.alive = false;
    }
}

int enterNumber()
{
    std::string tmp;
    bool err = true;
    int number = 0;

    while (err || (number == 0))
    {
        std::cin >> tmp;

        err = false;
        for (int i = 0; i < tmp.length(); ++i)
            if (!isdigit(tmp[i]))
                err = true;

        if (!err)
            number = std::stoi(tmp);

        if (err || (number == 0))
        {
            system("cls");
            std::cout << "Incorrect number. Try again" << std::endl << std::endl;
        }
    }

    system("cls");
    return (number);
}

Character generateCharacter(int i,
    const int minHP, const int maxHP,
    const int minArmor, const int maxArmor,
    const int minDmg, const int maxDmg)
{
    Character character;
    character.name = "E" + std::to_string(i);
    character.hp = rand() % (maxHP - minHP) + minHP;
    character.armor = rand() % (maxArmor - minArmor) + minArmor;
    character.dmg = rand() % (maxDmg - minDmg) + minDmg;
    character.position.x = rand() % gBgSize;
    character.position.y = rand() % gBgSize;
    
    return character;
}

void printBattleGround(std::string (* bg)[gBgSize], std::vector <Character>& players)
{
    for (int i = 0; i < gBgSize; ++i)
    {
        for (int j = 0; j < gBgSize; ++j) 
        {
            bg[i][j] = ". ";
        }
        std::cout << std::endl ;
    }
        

    for (int i = 0; i < players.size(); ++i)
    {
        if (players[i].alive)
        {
            int x = players[i].position.x;
            int y = players[i].position.y;

            bg[y][x] = ((i == 0) ? "P " : "E" + std::to_string(i));
        }
    }

    for (int i = 0; i < gBgSize; ++i)
    {
        for (int j = 0; j < gBgSize; ++j)
            std::cout << bg[i][j] << "  ";
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}