#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::xml_parser::trim_whitespace;

struct AsicData {
    unsigned int asicId;
    int hgValue;
    int lgValue;
};

struct BoardData {
    unsigned int boardId;
    std::vector<AsicData> asicsData;

    void AddAsics(int id, const std::vector<AsicData>& asData){
        boardId = id;
        asicsData =  asData;
    }
};

class XmlReaderWriter {
public:
    bool ReadXml(const std::string& filename);
    bool WriteXml(const std::string& filename);
    void AddBoard(BoardData& boardData);
    void PrintXml();
private:
    std::vector<BoardData> boardsData_;
};
