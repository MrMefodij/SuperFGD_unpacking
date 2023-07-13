//
// Created by amefodev on 03.07.2023.
//
#include <iostream>
#include <fstream>
#include "ThresholdXmlOutput.h"

int main() {
    ThresholdXmlOutput xmlFile;
    xmlFile.ReadXml("test.xml");
    ThresholdData tempData {0,{134,154,278}};
    std::vector<ThresholdData> tempBoard;
    for (int i = 0; i <= 7; ++i) {
        for (auto& a :tempData.peThreshold) {
            a=a+10*i;
        }
        tempBoard.push_back({tempData.asicId+i,tempData.peThreshold});
    }
    BoardData<ThresholdData> tempBoardData;
    tempBoardData.AddAsics(12,tempBoard);
    xmlFile.AddBoard(tempBoardData);
    xmlFile.PrintXml();
    xmlFile.WriteXml("test2.xml");
    return 0;
}