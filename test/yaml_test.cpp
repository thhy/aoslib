#include "yaml-cpp/yaml.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// our data types //这个例子好像是取自开源游戏引擎ogre，随便说的
struct Vec3
{ //位置坐标
    float x, y, z;
};

struct Power
{                     //招式，魔法
    std::string name; //招式名字，如 葵花宝典
    int damage;       //伤害值
};

struct Monster
{ //怪兽
    std::string name;
    Vec3 position;
    std::vector<Power> powers;
};

// now the extraction operators for these types //重载 >> 预算符。。。。
void operator>>(const YAML::Node &node, Vec3 &v)
{
    v.x = node[0].as<int>();
    v.y = node[1].as<int>();
    v.z = node[2].as<int>();
}

void operator>>(const YAML::Node &node, Power &power)
{
    power.name = std::move(node["name"].as<std::string>());
    power.damage = std::move(node["damage"].as<int>());
}

void operator>>(const YAML::Node &node, Monster &monster)
{
    monster.name = std::move(node["name"].as<std::string>());
    node["position"] >> monster.position;
    const YAML::Node &powers = node["powers"];
    for (unsigned i = 0; i < powers.size(); i++)
    {
        Power power;
        powers[i] >> power;
        monster.powers.push_back(power);
    }
}

int main() //测试程序
{
    YAML::Node doc = YAML::LoadFile("conf/conf.yaml");
    for (unsigned i = 0; i < doc.size(); i++)
    { //i的实际值是0，1，2 ；关联yaml 中三个大的struct：ogre，dragon，wizard
        Monster monster;
        doc[i] >> monster;
        std::cout << monster.name << "\n";
    }

    return 0;
}