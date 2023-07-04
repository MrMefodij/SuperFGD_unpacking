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
    xmlFile.WriteXml("test2.xml");
    return 0;
}