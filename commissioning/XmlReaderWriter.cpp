#include "XmlReaderWriter.h"
#include <boost/foreach.hpp>

const ptree& empty_ptree(){
    static ptree t;
    return t;
}


bool XmlReaderWriter::ReadXml(const std::string& filename){
    ptree tree;
    try {
        read_xml(filename, tree);
    } catch (const std::exception& ex) {
        std::cout << "Failed to read XML file: " << ex.what() << std::endl;
        return false;
    }

    try {
        const ptree & boards = tree.get_child("FEBsList", empty_ptree());
        BOOST_FOREACH(const ptree::value_type & f, boards){
            const ptree & attributes = f.second.get_child("<xmlattr>", empty_ptree());
            BoardData boardTemp;

            boardTemp.boardId = f.second.get<unsigned int>("<xmlattr>.id");

//            BOOST_FOREACH(const ptree::value_type &v, attributes){
//                if(v.first=="id") {
//                    std::cout << "Board_" << v.first.data() << " " << v.second.data() << std::endl;
//                }
//            }

            const ptree & subtree = f.second;
            BOOST_FOREACH(ptree::value_type asic, subtree.get_child("")){
                AsicData asicTemp;

                if(asic.first=="Asic"){
                    asicTemp.asicId = asic.second.get<unsigned int>("<xmlattr>.id");
                    asicTemp.hgValue = asic.second.get<unsigned int>("<xmlattr>.HG");
                    asicTemp.lgValue = asic.second.get<unsigned int>("<xmlattr>.LG");
//                    BOOST_FOREACH(ptree::value_type asicAttr, asic.second.get_child("<xmlattr>")){
//                        std::cout << '\t'<< asicAttr.first.data() << ": " << asicAttr.second.data() << std::endl;
//                    }
//                    std::cout << std::endl;
                    boardTemp.asicsData.push_back(std::move(asicTemp));
                }
            }
            boardsData_.push_back(std::move(boardTemp));

        }
    } catch (const std::exception& ex) {
        std::cout << "Error parsing XML: " << ex.what() << std::endl;
        return false;
    }
    return true;
};

bool XmlReaderWriter::WriteXml(const std::string& filename) {
    ptree tree;

    for (const auto& boardData : boardsData_) {
        ptree& boardNode = tree.add("FEBsList.Board", "");
        boardNode.put("<xmlattr>.id", boardData.boardId);

        for (const auto& asicData : boardData.asicsData) {
            ptree& asicNode = boardNode.add("Asic", "");
            asicNode.put("<xmlattr>.id", asicData.asicId);
            asicNode.put("<xmlattr>.HG", asicData.hgValue);
            asicNode.put("<xmlattr>.LG", asicData.lgValue);
        }
    }

    try {
        write_xml(filename, tree, std::locale(), xml_writer_settings<std::string>('\t', 1));
    } catch (const std::exception& ex) {
        std::cout << "Failed to write XML file: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

void XmlReaderWriter::PrintXml() {
    for (const auto& boardData : boardsData_) {
        std::cout << "Board ID: " << boardData.boardId << std::endl;

        for (const auto& asicData : boardData.asicsData) {
            std::cout << "  ASIC ID: " << asicData.asicId << std::endl;
            std::cout << "    HG Value: " << asicData.hgValue << std::endl;
            std::cout << "    LG Value: " << asicData.lgValue << std::endl;
        }

        std::cout << std::endl;
    }
}

void XmlReaderWriter::AddBoard(BoardData& boardData){
    boardsData_.push_back(std::move(boardData));
}