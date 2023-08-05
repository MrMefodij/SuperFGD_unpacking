//
// Created by amefodev on 03.07.2023.
//
#include <iostream>
#include <fstream>
#include "ThresholdXmlOutput.h"
#include "BaseLineThresholdXmlOutput.h"

int main() {
    BaseLineThresholdXmlOutput xmlFile;
    xmlFile.ReadXml("test.xml");
    BaseLineThreshold tempData {0,134};
    std::vector<BaseLineThreshold> tempBoard;
    for (int i = 0; i <= 7; ++i) {
        unsigned int a = 10*i + 134;
        tempBoard.push_back({tempData.asicId+i, a});
    }
    BoardData<BaseLineThreshold> tempBoardData;
    tempBoardData.AddAsics(12,tempBoard);
    xmlFile.AddBoard(tempBoardData);
    xmlFile.PrintXml();
    xmlFile.WriteXml("test2.xml");
    return 0;
}