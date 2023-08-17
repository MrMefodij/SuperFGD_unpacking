//
// Created by amefodev on 08.06.2023.
//

#include "Connection_Map.h"
#include <exception>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

Connection_Map::Connection_Map( const std::string& map_file) : fileName_(map_file){}

void Connection_Map::Init(){
    if(fileName_.find(".txt")!=std::string::npos) InitbyTXT();
    else if(fileName_.find(".csv")!=std::string::npos) InitbyCSV();
    else {
        std::cerr << "Unexpected format of connection map" << std::endl;
    }
}

void Connection_Map::InitbyTXT(){
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
        mapGeom_.insert({GetGlobalChannel(tempCh),std::move(tempGeom)});
        mapCh_.insert({GetGlobalChannel(tempCh),std::move(tempCh)});
    }
}

void Connection_Map::InitbyCSV(){
    std::ifstream map(fileName_.c_str());
    if(!map.is_open()){
        throw std::exception();
    }
    std::string line, st;
    while(getline(map,line)){
        GlChannelPosition tempCh;
        GlGeomPosition tempGeom;
        std::istringstream ist(line);
        int index=0;
        while(getline(ist,st,',')){
            if(index==0) tempGeom.pcbPosition_ = st;
            else if(index==1) tempCh.pcbid_ = st;
            else if(index==2) tempCh.pcbtype_ = stoi(st);
            else if(index==3) tempGeom.rotate_ = stoi(st);
            else if(index==4) tempCh.mppcch_ = stoi(st);
            else if(index==5) tempGeom.x_ = stoi(st);
            else if(index==6) tempGeom.y_ = stoi(st);
            else if(index==7) tempGeom.z_ = stoi(st);
            else if(index==8) tempCh.channelid_mc_ = stoi(st);
            else if(index==9) tempCh.cabletype_ = stoi(st);
            else if(index==10) tempGeom.cablelength_ = stoi(st);
            else if(index==11) tempCh.crate_     = stoi(st);
            else if(index==12) tempCh.slot_      = stoi(st);
            else if(index==13) tempCh.position_  = stoi(st);
            else if(index==14) tempCh.ch32_      = stoi(st);
            else if(index==15) tempCh.ch256_     = stoi(st);
            else if(index==16) tempCh.channelid_elec_ = stoi(st);
            else{
                std::cerr << "Unexpected cvs format" << std::endl;
                return;
            }
            index++;
        }
        //std::cout << tempGeom.pcbPosition_ << "  " << tempGeom.x_ << "  " << tempGeom.y_ << "  " << tempGeom.z_ << "  "
        //          << tempCh.crate_ << "  " <<tempCh.slot_ << "  " << tempCh.position_ << "  " << tempCh.ch32_ << "  "
        //          << tempCh.ch256_ << "  " << std::endl;
        mapGeom_.insert({GetGlobalChannel(tempCh),std::move(tempGeom)});
        mapCh_.insert({GetGlobalChannel(tempCh),std::move(tempCh)});
    }
}

const GlGeomPosition Connection_Map::GetGlobalGeomPosition(const unsigned int boardID, const unsigned int ch256) const{
    unsigned int globalChannel =  (boardID << 8) | ch256;
//    std::cout << boardID<< " " <<globalChannel << std::endl;
    return mapGeom_.at(globalChannel);
}

const GlChannelPosition Connection_Map::GetGlobalChannelPosition(const unsigned int boardID, const unsigned int ch256) const{
    unsigned int globalChannel =  (boardID << 8) | ch256;
//    std::cout << boardID<< " " <<globalChannel << std::endl;
    return mapCh_.at(globalChannel);
}

unsigned int Connection_Map::GetGlobalChannel(const GlChannelPosition& glCh){
    unsigned int boardID = (glCh.crate_ << 4) | glCh.slot_;
    unsigned int globalChannel = (boardID << 8) | glCh.ch256_;
//    std::cout << boardID<< " " <<globalChannel << std::endl;
    return globalChannel;
}
