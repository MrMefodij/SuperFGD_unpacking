//
// Created by amefodev on 21.06.2023.
//

#ifndef UNPACKING_XMLWRITER_H
#define UNPACKING_XMLWRITER_H

#include <iostream>
#include <fstream>
#include <vector>
//#include <rapidxml/rapidxml.hpp>
//#include <rapidxml/rapidxml_print.hpp>


class XmlWriter {
public:
    XmlWriter() {
        doc = new xml_document<>;
    }

    ~XmlWriter() {
        delete doc;
    }

    void AddElement(const std::string& name, const std::string& value) {
        xml_node<>* node = doc->allocate_node(node_element, doc->allocate_string(name.c_str()), doc->allocate_string(value.c_str()));
        currentNode->append_node(node);
    }

    void AddNestedElement(const std::string& parentName, const std::string& childName, const std::string& value) {
        xml_node<>* parentNode = doc->allocate_node(node_element, doc->allocate_string(parentName.c_str()));
        xml_node<>* childNode = doc->allocate_node(node_element, doc->allocate_string(childName.c_str()), doc->allocate_string(value.c_str()));
        parentNode->append_node(childNode);
        doc->append_node(parentNode);
        currentNode = parentNode;
    }

    void Save(const std::string& filename) {
        std::ofstream file(filename);
        file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        file << *doc;
        file.close();
    }

private:
    xml_document<>* doc;
    xml_node<>* currentNode = doc;

};


#endif //UNPACKING_XMLWRITER_H
