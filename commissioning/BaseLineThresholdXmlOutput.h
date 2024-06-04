//
// Created by amefodev on 04.08.2023.
//

#ifndef UNPACKING_BASELINETHRESHOLDXMLOUTPUT_H
#define UNPACKING_BASELINETHRESHOLDXMLOUTPUT_H

#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "BoardData.h"

using boost::property_tree::ptree;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::xml_parser::trim_whitespace;

class BaseLineThresholdXmlOutput {
public:
    bool ReadXml(const std::string& filename);
    bool WriteXml(const std::string& filename);
    void AddBoard(BoardData<BaseLineThreshold>& boardData);
    void PrintXml() const;
private:
    std::vector<BoardData<BaseLineThreshold>> boardsData_;
};


#endif //UNPACKING_BASELINETHRESHOLDXMLOUTPUT_H
