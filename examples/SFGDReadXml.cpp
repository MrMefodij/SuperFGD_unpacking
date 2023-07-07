//
// Created by amefodev on 03.07.2023.
//
#include <iostream>
#include <fstream>
#include "XmlReaderWriter.h"

int main() {
    XmlReaderWriter xmlFile;
    xmlFile.ReadXml("test1.xml");
    xmlFile.PrintXml();
    AsicData tempAsic{0,100,200};
    std::vector<AsicData> tempBoard;
    for (int i = 0; i < 7; ++i) {
        tempBoard.push_back({tempAsic.asicId+i,tempAsic.hgValue + i, tempAsic.lgValue +10*i});
    }
    BoardData tempData;
    tempData.AddAsics(12,tempBoard);
    xmlFile.AddBoard(tempData);
    xmlFile.WriteXml("test2.xml");
    return 0;
}