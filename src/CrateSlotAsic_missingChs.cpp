//
// Created by Maria on 08.08.2023.
//

#include "CrateSlotAsic_missingChs.h"
#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>
CrateSlotAsic_missingChs::CrateSlotAsic_missingChs( const std::string& map_file) : _filename(map_file){}

void CrateSlotAsic_missingChs::Init(){
    std::ifstream infile(_filename.c_str());
    if(!infile.is_open()){
        throw std::exception();
    }
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        Mapping mapping;
        std::vector<unsigned int> chs;
        chs.clear();
        if (!(iss >> mapping._crate_id >> mapping._slot_id >> mapping._asic_id)) { break; } // error
        while (!iss.eof()) {
            unsigned int ch;
            iss >> ch;
            chs.push_back(32 * mapping._asic_id + ch);
        }
        if (chs.empty()) { break; }
        _missing_chs[mapping] = chs;
//        std::cout << mapping._crate_id <<" "<< mapping._slot_id <<" "<< mapping._asic_id<<" " << chs.size()<<std::endl;
    }

}
bool CrateSlotAsic_missingChs::Is_Missing_Chs(Mapping mapping, unsigned int ch){
    if(_missing_chs.find(mapping) != _missing_chs.end())
    {
        if(std::find(_missing_chs[mapping].begin(), _missing_chs[mapping].end(), ch) != _missing_chs[mapping].end())
            return true;
        return false;
    }
    return false;
}

bool CrateSlotAsic_missingChs::Is_Missing_FEB(unsigned int board_id){
    if(board_id == 112  || board_id == 253)
        return true;
    return false;
}