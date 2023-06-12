//
// Created by amefodev on 08.06.2023.
//

#ifndef UNPACKING_CONNECTION_MAP_H
#define UNPACKING_CONNECTION_MAP_H
#include <fstream>
#include <map>
#include <unordered_map>

struct GlGeomPosition{
    std::string pcbPosition_;
    unsigned int x_;
    unsigned int y_;
    unsigned int z_;
};

struct GlChannelPosition{
    unsigned int crate_;
    unsigned int slot_;
    unsigned int position_;
    unsigned int ch32_;
    unsigned int ch256_;
};


std::ostream& operator << (std::ostream& os, const std::map<unsigned int, GlGeomPosition>& map) {
    for ( const auto& a : map) {
        os << '{' << a.first;
        os << ": " << a.second.pcbPosition_ << " " << a.second.x_ << " " << a.second.y_ << " " << a.second.z_ << "}\n";
    }
    return os ;
}

class Connection_Map {
public:
    Connection_Map(const std::string& map_file);
    void Init();
    const GlGeomPosition GetGlobalGeomPosition(unsigned int Feb, unsigned int ch256) const;

private:
    const std::string fileName_;
    std::map<unsigned int, GlGeomPosition> map_;
    unsigned int GetGlobalChannel(const GlChannelPosition& tempCh);
};


#endif //UNPACKING_CONNECTION_MAP_H
