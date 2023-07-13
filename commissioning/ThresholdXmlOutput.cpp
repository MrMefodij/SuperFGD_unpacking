//
// Created by amefodev on 13.07.2023.
//

#include "ThresholdXmlOutput.h"
#include <boost/foreach.hpp>


bool ThresholdXmlOutput::ReadXml(const std::string& filename){
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
            BoardData<ThresholdData> boardTemp;

            boardTemp.boardId = f.second.get<unsigned int>("<xmlattr>.id");

            const ptree & subtree = f.second;
            BOOST_FOREACH(ptree::value_type asic, subtree.get_child("")){
                ThresholdData asicTemp;
                if(asic.first=="Asic"){
                    asicTemp.asicId = asic.second.get<unsigned int>("<xmlattr>.id");
//                    std::cout << "ID Asic: " << asicTemp.asicId << std::endl;
                    BOOST_FOREACH(ptree::value_type asicAttr, asic.second.get_child("<xmlattr>")){
                        if (asicAttr.first != "id") {
//                            auto a = asicAttr.second.get<unsigned int>("");
                            asicTemp.AddPeThreshold(asicAttr.second.get<unsigned int>(""));
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

bool ThresholdXmlOutput::WriteXml(const std::string& filename) {
    ptree tree;

    for (const auto& boardData : boardsData_) {
        ptree& boardNode = tree.add("FEBsList.Board", "");
        boardNode.put("<xmlattr>.id", boardData.boardId);

        for (const auto& asicData : boardData.asicsData) {
            ptree& asicNode = boardNode.add("Asic", "");
            asicNode.put("<xmlattr>.id", asicData.asicId);
            for (int i = 0; i < asicData.peThreshold.size(); ++i) {
                asicNode.put("<xmlattr>.pe"+ std::to_string(i), asicData.peThreshold.at(i));
            }
//            asicNode.put("<xmlattr>.HG", asicData.hgValue);
//            asicNode.put("<xmlattr>.LG", asicData.lgValue);
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

void ThresholdXmlOutput::PrintXml() const {
    for (const auto& boardData : boardsData_) {
        std::cout << "Board ID: " << boardData.boardId << std::endl;

        for (const auto& asicData : boardData.asicsData) {
            std::cout << "  ASIC ID: " << asicData.asicId << std::endl;
            for (int i = 0; i < asicData.peThreshold.size(); ++i) {
                std::cout << "\t Threshold "<< i+1 << ".25 pe: " << asicData.peThreshold.at(i) << std::endl;
            }
        }

        std::cout << std::endl;
    }
}

void ThresholdXmlOutput::AddBoard(BoardData<ThresholdData>& boardData){
    boardsData_.push_back(std::move(boardData));
}