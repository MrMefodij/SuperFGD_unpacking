//
// Created by amefodev on 21.06.2023.
//

#include "XmlWriter.h"

void XmlWriter::AddElement(const std::string& name, const std::string& value) {
    ptree_.add(name, value);
}

void  XmlWriter::AddNestedElement(const std::string& parentName, const std::string& childName, const std::string& value) {
    ptree_.add(parentName + "." + childName, value);
}

void  XmlWriter::Save(const std::string& filename) {
    boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
    boost::property_tree::write_xml(filename, ptree_, std::locale(), settings);
}

void  XmlWriter::Load(const std::string& filename) {
    boost::property_tree::read_xml(filename, ptree_);
}

void  XmlWriter::Print() {
    boost::property_tree::write_xml(std::cout, ptree_);
}

std::map<std::string, std::map<std::string, std::string>>  XmlWriter::readXml(const std::string& filename) {
    std::map<std::string, std::map<std::string, std::string>> dataMap;

    boost::property_tree::ptree tree;
    try {
        boost::property_tree::read_xml(filename, tree);

        // Проход по узлам XML и извлечение данных
        for (const auto &level1: tree.get_child("root")) {
            std::string level1Name = level1.first;
            std::map<std::string, std::string> level2Data;

            // Проход по вложенным узлам второго уровня
            for (const auto &level2: level1.second) {
                std::string level2Name = level2.first;
                std::string value = level2.second.get_value<std::string>();
                level2Data[level2Name] = value;
            }

            dataMap[level1Name] = level2Data;
        }
    } catch (const boost::property_tree::xml_parser_error &e) {
        std::cout << "XML parsing error: " << e.what() << std::endl;
    } catch (const boost::property_tree::ptree_error &e) {
        std::cout << "Property tree error: " << e.what() << std::endl;
    }

    return dataMap;
}

std::map<std::string, std::map<std::string, std::string>>  XmlWriter::ptreeToMap(const boost::property_tree::ptree& tree) {
    std::map<std::string, std::map<std::string, std::string>> dataMap;
    for (const auto& node : tree) {
        if (node.second.empty()) {
            dataMap[node.first] = { { "", node.second.get_value<std::string>() } };
        } else {
            std::map<std::string, std::string> subMap;
            for (const auto& subNode : node.second) {
                subMap[subNode.first] = subNode.second.get_value<std::string>();
            }
            dataMap[node.first] = subMap;
        }
    }
    return dataMap;
}
