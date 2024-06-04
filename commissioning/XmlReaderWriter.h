#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "BoardData.h"

using boost::property_tree::ptree;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::xml_parser::trim_whitespace;

class XmlReaderWriter {
public:
    bool ReadXml(const std::string& filename);
    bool WriteXml(const std::string& filename);
    void AddBoard(BoardData<AsicData>& boardData);
    void PrintXml();
private:
    std::vector<BoardData<AsicData>> boardsData_;
};
