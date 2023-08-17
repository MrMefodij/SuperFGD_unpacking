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
    unsigned int rotate_;
    unsigned int cablelength_;
};

struct GlChannelPosition{
    std::string pcbid_;
    unsigned int pcbtype_;
    unsigned int mppcch_;
    unsigned int channelid_mc_;
    unsigned int cabletype_;
    unsigned int crate_;
    unsigned int slot_;
    unsigned int position_;
    unsigned int ch32_;
    unsigned int ch256_;
    unsigned int channelid_elec_;
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
    const GlGeomPosition GetGlobalGeomPosition(const unsigned int Feb, const unsigned int ch256) const;
    const GlChannelPosition GetGlobalChannelPosition(const unsigned int Feb, const unsigned int ch256) const;
    unsigned int GetGlobalChannel(const GlChannelPosition& tempCh);
    unsigned int GetAsic(unsigned int globalChannel);
private:
    void InitbyCSV();
    void InitbyTXT();
    const std::string fileName_;
    std::map<unsigned int, GlGeomPosition> mapGeom_;
    std::map<unsigned int, GlChannelPosition> mapCh_;
};


#endif //UNPACKING_CONNECTION_MAP_H
