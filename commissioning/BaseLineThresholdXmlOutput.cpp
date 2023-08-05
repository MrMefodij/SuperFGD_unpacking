//
// Created by amefodev on 04.08.2023.
//

#include "BaseLineThresholdXmlOutput.h"
#include <boost/foreach.hpp>


bool BaseLineThresholdXmlOutput::ReadXml(const std::string& filename){
    ptree tree;
    try {
        read_xml(filename, tree);
    } catch (const std::exception& ex) {
        std::cout << "Failed to read XML file: " << ex.what() << std::endl;
        return false;
    }

    try {
        const ptree & boards = tree.get_child("FEBsList");
        BOOST_FOREACH(const ptree::value_type & f, boards){
            const ptree & attributes = f.second.get_child("<xmlattr>");
            BoardData<BaseLineThreshold> boardTemp;
            boardTemp.boardId = f.second.get<unsigned int>("<xmlattr>.id");
            const ptree & subtree = f.second;
            BOOST_FOREACH(ptree::value_type asic, subtree.get_child("")){
                BaseLineThreshold asicTemp;
                if(asic.first=="Asic"){
                    asicTemp.asicId = asic.second.get<unsigned int>("<xmlattr>.id");
//                    std::cout << "ID Asic: " << asicTemp.asicId << std::endl;
                    BOOST_FOREACH(ptree::value_type asicAttr, asic.second.get_child("<xmlattr>")){
                        if (asicAttr.first != "id") {
                            auto a = asicAttr.second.get<unsigned int>("");
                            asicTemp.thresholdForBaseline = asicAttr.second.get<unsigned int>("");
//                            std::cout << '\t' << asicAttr.first.data() << ": " << asicAttr.second.data() <<" "<<a << std::endl;
                        }
                    }
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

bool BaseLineThresholdXmlOutput::WriteXml(const std::string& filename) {
    ptree tree;

    for (const auto& boardData : boardsData_) {
        ptree& boardNode = tree.add("FEBsList.Board", "");
        boardNode.put("<xmlattr>.id", boardData.boardId);

        for (const auto& asicData : boardData.asicsData) {
            ptree& asicNode = boardNode.add("Asic", "");
            asicNode.put("<xmlattr>.id", asicData.asicId);
            asicNode.put("<xmlattr>.DAC10bTh", asicData.thresholdForBaseline);
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

void BaseLineThresholdXmlOutput::PrintXml() const {
    for (const auto& boardData : boardsData_) {
        std::cout << "Board ID: " << boardData.boardId << std::endl;

        for (const auto& asicData : boardData.asicsData) {
            std::cout << "  ASIC ID: " << asicData.asicId << std::endl;
            std::cout << "\tBaseline Threshold " << asicData.thresholdForBaseline << std::endl;
        }

        std::cout << std::endl;
    }
}

void BaseLineThresholdXmlOutput::AddBoard(BoardData<BaseLineThreshold>& boardData) {
    boardsData_.push_back(std::move(boardData));
}