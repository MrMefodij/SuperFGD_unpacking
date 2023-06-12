//
// Created by amefodev on 08.06.2023.
//

#include "Connection_Map.h"
#include <exception>
#include <iostream>
#include <algorithm>

Connection_Map::Connection_Map( const std::string& map_file) : fileName_(map_file){}

void Connection_Map::Init(){
    std::ifstream map(fileName_.c_str());
    if(!map.is_open()){
        throw std::exception();
    }
    while (!map.eof()){
        GlChannelPosition tempCh;
        GlGeomPosition tempGeom;
//        PCBPosition	x	y	z	Crate	Slot	Position	CITIROCCh32	CITIROCCh256
        map >> tempGeom.pcbPosition_ >> tempGeom.x_ >> tempGeom.y_ >> tempGeom.z_ >>
        tempCh.crate_ >>tempCh.slot_ >> tempCh.position_ >> tempCh.ch32_ >> tempCh.ch256_;
        map_.insert({GetGlobalChannel(tempCh),std::move(tempGeom)});
    }
    std::cout << map_ << map_.size();
}

const GlGeomPosition Connection_Map::GetGlobalGeomPosition(unsigned int Feb, unsigned int ch256) const{
    return map_.at(Feb*256 * ch256);
}

unsigned int Connection_Map::GetGlobalChannel(const GlChannelPosition& GlCh){
    return (GlCh.crate_*14 + GlCh.slot_)*256 + GlCh.ch256_;
}