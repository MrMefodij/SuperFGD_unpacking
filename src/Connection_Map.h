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
    std::string lgpposition_;
    unsigned int lgpposition_id_;
};

struct GlChannelPosition{
    std::string pcbid_;
    unsigned int readoutID_;
    unsigned int massTest_;
    unsigned int pcbtype_;
    unsigned int mppcch_;
    unsigned int channelid_mc_;
    unsigned long channelid_mc_global_;
    unsigned int cabletype_;
    unsigned int crate_;
    unsigned int slot_;
    unsigned int position_;
    unsigned int ch32_;
    unsigned int ch256_;
    unsigned int channelid_elec_;
    unsigned int channelid_elec_global_;
    unsigned int asic_;
    unsigned int led_driver_id_;
    unsigned int led_driver_ch_;
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
    Connection_Map(const std::string& map_file, const unsigned int verbose=0);
    void Init();
    

    // These two methods is not support exception for the missing slot,
    // but are left for EventDisplay.
    const GlGeomPosition GetGlobalGeomPosition(const unsigned int Feb, const unsigned int ch256) const;
    const GlChannelPosition GetGlobalChannelPosition(const unsigned int Feb, const unsigned int ch256) const;
    
    // PositionPtr is recommened.
    const GlGeomPosition* GetGlobalGeomPositionPtr(const unsigned int Feb, const unsigned int ch256) const;
    const GlGeomPosition* GetGlobalGeomPositionPtr(const unsigned int globalChannel) const;
    const GlChannelPosition* GetGlobalChannelPositionPtr(const unsigned int Feb, const unsigned int ch256) const;
    const GlChannelPosition* GetGlobalChannelPositionPtr(const unsigned int globalChannel) const ;

    unsigned int GetGlobalChannel(const GlChannelPosition& tempCh) const;
    unsigned int GetGlobalChannel(unsigned int Feb, unsigned int ch256) const;
    unsigned int GetGlobalPedestalChannel(unsigned int Feb, unsigned int ch256) const;
    std::string GetPcbPosition(const unsigned int Feb) const;
    int GetGlobalChannel(const unsigned int x, const unsigned int y, const unsigned int z) const;
    unsigned int GetAsic(unsigned int globalChannel);

private:
    void InitbyCSV();
    void InitbyTXT();
    const std::string fileName_;
    std::map<unsigned int, GlGeomPosition> mapGeom_;
    std::map<unsigned int, GlChannelPosition> mapCh_;
    unsigned int _verbose;
};


#endif //UNPACKING_CONNECTION_MAP_H
