//
// Created by amefodev on 13.07.2023.
//

#ifndef UNPACKING_THESHOLDXMLOUTPUT_H
#define UNPACKING_THESHOLDXMLOUTPUT_H

#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "BoardData.h"

using boost::property_tree::ptree;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::xml_parser::trim_whitespace;

class ThresholdXmlOutput {
public:
    bool ReadXml(const std::string& filename);
    bool WriteXml(const std::string& filename);
    void AddBoard(BoardData<ThresholdData>& boardData);
    void PrintXml() const;
private:
    std::vector<BoardData<ThresholdData>> boardsData_;
};


#endif //UNPACKING_THESHOLDXMLOUTPUT_H
