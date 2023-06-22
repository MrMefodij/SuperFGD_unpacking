#include <iostream>
#include <fstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class XmlWriter {
public:
    void AddElement(const std::string& name, const std::string& value);

    void AddNestedElement(const std::string& parentName, const std::string& childName, const std::string& value);

    void Save(const std::string& filename);

    void Load(const std::string& filename);

    void Print();

    std::map<std::string, std::map<std::string, std::string>> readXml(const std::string& filename);

    std::map<std::string, std::map<std::string, std::string>> ptreeToMap(const boost::property_tree::ptree& tree);
private:
    boost::property_tree::ptree ptree_;
};

//int main() {
//    XmlWriter writer;
//    writer.AddElement("Name", "John Doe");
//    writer.AddElement("Age", "25");
//    writer.AddNestedElement("Address", "Street", "123 Main St");
//    writer.AddNestedElement("Address", "City", "New York");
//    writer.AddNestedElement("Address", "Country", "USA");
//    writer.Save("data.xml");
//
//    XmlWriter reader;
//    reader.Load("data.xml");
//    reader.Print();
//
//    return 0;
//}
