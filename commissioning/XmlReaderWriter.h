#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::xml_writer_settings;
using boost::property_tree::xml_parser::trim_whitespace;

class XmlReaderWriter {
public:
    bool ReadXml(const std::string& filename);
    bool WriteXml(const std::string& filename);
    void PrintXml();
private:

    struct AsicData {
        int asicId;
        int hgValue;
        int lgValue;
    };

    struct BoardData {
        int boardId;
        std::vector<AsicData> asicsData;
    };

    std::vector<BoardData> boardsData_;
};
