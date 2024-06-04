//
// Created by amefodev on 14.09.2023.
//

#ifndef UNPACKING_EVENTDISPLAY_H
#define UNPACKING_EVENTDISPLAY_H

#include <string>
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include <tuple>
#include "TGraph.h"
#include "Connection_Map.h"
#include "TSFGDigit.hxx"
#include "SFGD_defines.h"
#include "Reader.h"

struct CrateGates{
    unsigned int crateNumber_;
    std::vector<unsigned int> availableFebs_;
    unsigned int nEntries_;
};


struct DataFile{
    ~DataFile() {
        delete AllEvents_;
        FileInput_->Close();
    }
    std::string fileName_;
    TTree * AllEvents_;
    TFile * FileInput_;
    CrateGates availableCrates_;
};

struct FebData{
    unsigned int febNumber_;
    std::vector<TSFGDigit> eventsHits;
};

struct Position{
    unsigned int _par_1;
    unsigned int _par_2;
    bool operator<(const Position &other) const {
        if(_par_1 == other._par_1){
            return _par_2 < other._par_2;
        }
        return _par_1 < other._par_1;
    }

    bool const operator==(const Position &other) const {
        return  _par_1 == other._par_1 && _par_2 == other._par_2;
    }
};

struct event_position_horizontal{
    int position_[Y_SIZE] = {0};
    Double_t max_LY_[Y_SIZE] = {0};
    Double_t LY_summ_[Y_SIZE] = {0};
    bool existY_[Y_SIZE] = {false};
    bool existX_[X_SIZE] = {false};
};

struct event_position_vertical{
    int positionX_[Z_SIZE] = {0};
    Double_t maxX_LY_[Z_SIZE] = {0};
    Double_t LY_summX_[Z_SIZE] = {0};
    bool existX_[Z_SIZE] = {false};
    unsigned int countX_ = 0;

    int positionZ_[X_SIZE] = {0};
    Double_t maxZ_LY_[X_SIZE] = {0};
    Double_t LY_summZ_[X_SIZE] = {0};
    bool existZ_[X_SIZE] = {false};
    unsigned int countZ_ = 0;
};

event_position_horizontal max_in_vector(const std::vector<std::vector<Double_t>>* event_map_XY, unsigned int x_, unsigned int y_){
    event_position_horizontal ev;
    for (int y = 0; y < y_; ++y) {
        for (int x = 0; x < x_; ++x) {
            if (event_map_XY->at(x).at(y)>0) {
                if (ev.max_LY_[y] <= event_map_XY->at(x).at(y)) {
                    ev.position_[y] = x;
                    ev.max_LY_[y] = event_map_XY->at(x).at(y);
                }
                ev.LY_summ_[y] += event_map_XY->at(x).at(y);
                ev.existY_[y] = true;
                ev.existX_[x] = true;
            }
        }
    }
    return ev;
}

event_position_vertical max_in_vector_vertical(const std::vector<std::vector<Double_t>>* event_map_XY, unsigned int x_, unsigned int z_){
    event_position_vertical ev;
    for (int z = 0; z < z_; ++z) {
        for (int x = 0; x < x_; ++x) {
            if (event_map_XY->at(x).at(z)>0) {
                if (ev.maxX_LY_[z] <= event_map_XY->at(x).at(z)) {
                    ev.positionX_[z] = x;
                    ev.maxX_LY_[z] = event_map_XY->at(x).at(z);
                }
                ev.LY_summX_[z] += event_map_XY->at(x).at(z);
                ev.existX_[z] = true;
            }
        }
        if(ev.existX_[z]){
            ++ev.countX_;
        }
    }
    for (int x = 0; x < x_; ++x) {
        for (int z = 0; z < z_; ++z) {
            if (event_map_XY->at(x).at(z)>0) {
                if (ev.maxZ_LY_[x] <= event_map_XY->at(x).at(z)) {
                    ev.positionZ_[x] = z;
                    ev.maxZ_LY_[x] = event_map_XY->at(x).at(z);
                }
                ev.LY_summZ_[x] += event_map_XY->at(x).at(z);
                ev.existZ_[x] = true;
            }
        }
        if (ev.existZ_[x]){
            ++ev.countZ_;
        }
    }
    return ev;
}

void GetEventsAmpl(TFile *fileInput, const TObject *obj, const TObject* file, const std::string& str, const std::string& eventNum,
                    std::vector<std::vector<Double_t>>& event_map_XY, std::vector<std::vector<Double_t>>& event_map_YZ,
                    std::vector<std::vector<Double_t>>& event_map_XZ, unsigned int cut){
    if (str.find("XY") != str.npos) {
//        std::cout << "GetFile: " << file->GetName() << " " << str.find("Event_HG_") << std::endl;
        TH2F *event;
        std::string path(obj->GetName());
        path += "/";
        path += file->GetName();
//        std::cout << path << std::endl;
        fileInput->GetObject(path.c_str(), event);
//        std::cout << path << " " << AS_KV(event->GetEntries()) <<std::endl;
//        if (event->GetEntries() > cut) {
            for (int x = 0; x < X_SIZE; ++x) {
                for (int y = 0; y < Y_SIZE; ++y) {
                    event_map_XY[x][y] = event->GetBinContent(x, y);
                }
            }
//        }
        event->Delete();
    }
    if (str.find("YZ") != str.npos) {
//        std::cout << "GetFile: " << file->GetName() << " " << str.find("Event_HG_") << std::endl;
        TH2F *event;
        std::string path(obj->GetName());
        path += "/";
        path += file->GetName();
        fileInput->GetObject(path.c_str(), event);
//        std::cout << path << " " << AS_KV(event->GetEntries()) <<std::endl;
        if (event->GetEntries() > cut) {
            for (int z = 0; z < Z_SIZE; ++z) {
                for (int y = 0; y < Y_SIZE; ++y) {
                    event_map_YZ[z][y] = event->GetBinContent(z, y);
                }
            }
        }
        event->Delete();
    }
    if (str.find("XZ") != str.npos) {
//        std::cout << "GetFile: " << file->GetName() << " " << str.find("Event_HG_") << std::endl;
        TH2F *event;
        std::string path(obj->GetName());
        path += "/";
        path += file->GetName();
        fileInput->GetObject(path.c_str(), event);
//        std::cout << path << " " << AS_KV(event->GetEntries()) <<std::endl;
        if (event->GetEntries() > cut) {
            for (int x = 0; x < X_SIZE; ++x) {
                for (int z = 0; z < Z_SIZE; ++z) {
                    event_map_XZ[x][z] = event->GetBinContent(x, z);
                }
            }
        }
        event->Delete();
    }
}
void GetEventsAmpl(TFile *fileInput, const TObject *obj, const TObject* file, const std::string& str,
                   TGraph& event_map_YZ_gr, TGraph& event_map_XZ_gr, std::map<Position, double>& event_map_YZ,
                   std::map<Position, double>& event_map_XZ){
    if (str.find("YZ") != str.npos) {
        TH2F *event;
        std::string path(obj->GetName());
        path += "/";
        path += file->GetName();
        fileInput->GetObject(path.c_str(), event);
        for (unsigned int z = 0; z < Z_SIZE; ++z) {
            for (unsigned int y = 0; y < Y_SIZE; ++y) {
                if(event->GetBinContent(z, y) > 0) {
                    event_map_YZ_gr.AddPoint(z, y);
                    event_map_YZ.insert({{z,y},event->GetBinContent(z, y)});
                }

            }
        }
        event->Delete();
    }
    if (str.find("XZ") != str.npos) {
        TH2F *event;
        std::string path(obj->GetName());
        path += "/";
        path += file->GetName();
        fileInput->GetObject(path.c_str(), event);
        for (unsigned int z = 0; z < X_SIZE; ++z) {
            for (unsigned int x = 0; x < Z_SIZE; ++x) {
                if(event->GetBinContent(x, z) > 0) {
                    event_map_XZ.insert({{z, x}, event->GetBinContent(x, z)});
                    event_map_XZ_gr.AddPoint(z, x);
                }
            }
        }
        event->Delete();
    }
}


typedef std::tuple<TH1F*,TH1F*,bool> EventTime ;
typedef std::tuple<TH2F*,TH2F*,TH2F *> EventMap3D ;

class EventDisplay {
public:
    EventDisplay(const std::string& evNumString);
    void AddEvents(const std::vector<FebData>& crateData, const Connection_Map& connectionMap, const Reader& gainResults, const Reader& pedResults, TH1F & bunchStructure);
    EventTime GetPictures();
    EventMap3D GetPictures(const std::string& ampl);
    ~EventDisplay();
private:
    bool eventExist_ = false;
    const std::string evNumString_;
    TH2F eventXY = TH2F(("Event_" + evNumString_ + "_XY").c_str(), ("Event_" + evNumString_ + "_XY").c_str(),
                        192, 0, 192, 56, 0, 56);
    TH2F eventYZ = TH2F(("Event_" + evNumString_ + "_YZ").c_str(), ("Event_" + evNumString_ + "_YZ").c_str(),
                        182, 0, 182, 56, 0, 56);
    TH2F eventXZ = TH2F(("Event_" + evNumString_ + "_XZ").c_str(), ("Event_" + evNumString_ + "_XZ").c_str(),
                        192, 0, 192, 182, 0, 182);

    TH2F eventHG_XY = TH2F(("Event_HG_" + evNumString_ + "_XY").c_str(), ("Event_HG_" + evNumString_ + "_XY").c_str(),
                           192, 0, 192, 56, 0, 56);
    TH2F eventHG_YZ = TH2F(("Event_HG_" + evNumString_ + "_YZ").c_str(), ("Event_HG_" + evNumString_ + "_YZ").c_str(),
                           182, 0, 182, 56, 0, 56);
    TH2F eventHG_XZ = TH2F(("Event_HG_" + evNumString_ + "_XZ").c_str(), ("Event_HG_" + evNumString_ + "_XZ").c_str(),
                           192, 0, 192, 182, 0, 182);

    TH2F eventLG_XY = TH2F(("Event_LG_" + evNumString_ + "_XY").c_str(), ("Event_LG_" + evNumString_ + "_XY").c_str(),
                           192, 0, 192, 56, 0, 56);
    TH2F eventLG_YZ = TH2F(("Event_LG_" + evNumString_ + "_YZ").c_str(), ("Event_LG_" + evNumString_ + "_YZ").c_str(),
                           182, 0, 182, 56, 0, 56);
    TH2F eventLG_XZ = TH2F(("Event_LG_" + evNumString_ + "_XZ").c_str(), ("Event_LG_" + evNumString_ + "_XZ").c_str(),
                           192, 0, 192, 182, 0, 182);

    TH2F eventXY_pe = TH2F(("Event_pe_" + evNumString_ + "_XY").c_str(), ("Event_pe_" + evNumString_ + "_XY").c_str(),
                        192, 0, 192, 56, 0, 56);
    TH2F eventYZ_pe = TH2F(("Event_pe_" + evNumString_ + "_YZ").c_str(), ("Event_pe_" + evNumString_ + "_YZ").c_str(),
                        182, 0, 182, 56, 0, 56);
    TH2F eventXZ_pe = TH2F(("Event_pe_" + evNumString_ + "_XZ").c_str(), ("Event_pe_" + evNumString_ + "_XZ").c_str(),
                        192, 0, 192, 182, 0, 182);

    TH2F eventXY_time = TH2F(("Event_time_" + evNumString_ + "_XY").c_str(), ("Event_time_" + evNumString_ + "_XY").c_str(),
                           192, 0, 192, 56, 0, 56);
    TH2F eventYZ_time = TH2F(("Event_time_" + evNumString_ + "_YZ").c_str(), ("Event_time_" + evNumString_ + "_YZ").c_str(),
                           182, 0, 182, 56, 0, 56);
    TH2F eventXZ_time = TH2F(("Event_time_" + evNumString_ + "_XZ").c_str(), ("Event_time_" + evNumString_ + "_XZ").c_str(),
                           192, 0, 192, 182, 0, 182);

    TH1F gtsSlot = TH1F (("GtsSlot_" + evNumString_ ).c_str(),("GtsSlot_" + evNumString_ ).c_str(),7,0,7 );
    TH1F eventTime = TH1F (("EventTime_" + evNumString_).c_str(), ("EventTime_" + evNumString_).c_str(),8000, 0 ,8000);
};


#endif //UNPACKING_EVENTDISPLAY_H
