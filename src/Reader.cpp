//
// Created by Maria on 18.10.2023 kolupanova@inr.ru
//

#include "Reader.h"
#include "TH1D.h"
#include "TF1.h"

void Reader::GetValueDistribution(const int x_down, const int x_up, const int x_down_2d, const int x_up_2d){
    TH1D* hValue = new TH1D("Distribution", "Value_distribution",  1000, x_down, x_up);
    TH1D* hPedestal_2d = new TH1D("Pedestal_2d_distribution", "Pedestal_2d_distribution",  1000, x_down_2d, x_up_2d);
    for(const auto& value : _res) {
        hValue->Fill(value.second._value);
        if(value.second._pedestal_2d != BadValue_PED)
            hPedestal_2d->Fill(value.second._pedestal_2d);
    }
    TF1 * fit = new TF1("fit","gaus");
    hValue->Fit("fit","","");
    _mean_value = fit->GetParameter(1);
    _std_value = fit->GetParameter(2);
    if(hPedestal_2d->GetEntries() > 0){
        hPedestal_2d->Fit("fit","","");
        _mean_value_pedestal_2d = fit->GetParameter(1);
        _std_value_pedestal_2d = fit->GetParameter(2);
    }
    delete hValue;
    delete hPedestal_2d;
    delete fit;
}

const std::pair<double, double> Reader::GetValueAndError(const unsigned int &global_channel) const {
    if(_res.find( global_channel ) != _res.end())
        return {_res.at(global_channel)._value,_res.at(global_channel)._value_error};
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return {_mean_value,_std_value};
}

const double Reader::GetValue(const unsigned int &global_channel) const {
    if(_res.find( global_channel ) != _res.end())
        return _res.at(global_channel)._value;
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _mean_value;
}

const double Reader::GetValueError(const unsigned int &global_channel) const {
    if(_res.find( global_channel ) != _res.end())
        return _res.at(global_channel)._value_error;
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _std_value;
}

const double Reader::GetOnlyValue(const unsigned int& global_channel) const{
    if(_res.find( global_channel ) != _res.end()) {
        if (_res.at(global_channel)._value == BadValue_PED || _res.at(global_channel)._value == BadValue_HG) {
            return _mean_value;
        }
        return _res.at(global_channel)._value;
    }
//    std::cout << AS_KV(_mean_value) << std::endl;
//    std::cerr << "Unable to find global channel GetOnlyValue "<< global_channel << std::endl;
    return _mean_value;
}

const double Reader::GetOnlyValue(const unsigned int &board_id, const unsigned int &channel_id) const{
    unsigned int global_channel =  (1<<23) | (board_id << 8) | channel_id;
    if(_res.find( global_channel ) != _res.end()) {
        if (_res.at(global_channel)._value == BadValue_PED || _res.at(global_channel)._value == BadValue_HG) {
            return _mean_value;
        }
        return _res.at(global_channel)._value;
    }
    return _mean_value;
}

void GainReader::Init(){
    if(!_files.empty()) {
        for (const auto& file: _files) {
            std::ifstream calib_res_file(file.c_str());
            if (!calib_res_file.is_open()) {
                throw std::exception();
            }
            std::string line;
            while (getline(calib_res_file, line)) {
                std::istringstream iss(line);
                Results calib_data;
                unsigned int boardID, ch256;
                double peak_position;
                if (!(iss >> boardID >> ch256 >> calib_data._value >> calib_data._value_error)) { break; }
                if (calib_data._value != BadValue_HG) {
                    while (iss >> peak_position) {
                        calib_data._peaks_position.push_back(peak_position);
                    }
                }
                else {
                    ++_bad_fit;
                    unsigned int crate = boardID >> 4;
                    std::cout << "Crate "<< crate<<" FEB " << boardID<<" Channel "<<ch256<<": bad fit" <<std::endl;
                }
                unsigned int globalChannel = (1<<23)|(boardID << 8) | ch256;
                _globalChannels.insert(globalChannel);
                _res.insert({globalChannel,calib_data});
            }
        }
    }
    else{
        std::cerr << "There are no txt files with calibration in the directory" << std::endl;
    }
    std::cout << "Number of channels without calibration: " <<_bad_fit<<std::endl;
    GetValueDistribution(0,100);
}

const std::vector<double> GainReader::GetPeaks(const unsigned int& global_channel){
    if(_res.find( global_channel ) != _res.end())
        return _res.at(global_channel)._peaks_position;
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return {};
}

void PedMapReader::Init(){
    if(!_files.empty()) {
        for (const auto& file: _files) {
            std::ifstream ped_map_res_file(file.c_str());
            if (!ped_map_res_file.is_open()) {
                throw std::exception();
            }
            std::string line;
            getline(ped_map_res_file, line);
            while (getline(ped_map_res_file, line)) {
                std::istringstream iss(line);
                Results ped_map_data;
                unsigned int global_ch;
                if (!(iss >> global_ch >> ped_map_data._value >> ped_map_data._value_error>>ped_map_data._pedestal_2d>>ped_map_data._pedestal_error_2d)) { break; }
                _globalChannels.insert(global_ch);
                if(ped_map_data._value ==  BadValue_PED || ped_map_data._pedestal_2d == BadValue_PED){
                    ++_bad_fit;
//                    std::cout << "Global channel "<<global_ch<<": bad fit" <<std::endl;
                }
                _res.insert({global_ch, ped_map_data});
            }
        }
    }
    else{
        std::cerr << "There are no txt files with calibration in the directory" << std::endl;
    }
    std::cout << "Number of channels without pedestal: " <<_bad_fit<<std::endl;
    GetValueDistribution(-100,200, -100, 200);
}

const std::pair<double, double> PedMapReader::GetValueAndError2d(const unsigned int& global_channel){
    if(_res.find( global_channel ) != _res.end())
        return {_res.at(global_channel)._pedestal_2d, _res.at(global_channel)._pedestal_error_2d};
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return {_mean_value_pedestal_2d, _std_value_pedestal_2d};
}

const double PedMapReader::GetOnly2dPedestal(const unsigned int &global_channel) const{
    if(_res.find( global_channel ) != _res.end()) {
        if (_res.at(global_channel)._value == BadValue_PED) {
            return _mean_value_pedestal_2d;
        }
        return _res.at(global_channel)._pedestal_2d;
    }
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _mean_value_pedestal_2d;
}

const double PedMapReader::GetPedestal2d(const unsigned int &global_channel) const {
    if(_res.find( global_channel ) != _res.end())
        return _res.at(global_channel)._pedestal_2d;
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _mean_value_pedestal_2d;
}

const double PedMapReader::GetPedestal2dError(const unsigned int &global_channel) const {
    if(_res.find( global_channel ) != _res.end())
        return _res.at(global_channel)._pedestal_error_2d;
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _std_value_pedestal_2d;
}

void LgHgReader::Init(){
    if(!_files.empty()) {
        for (const auto& file: _files) {
            std::ifstream calib_res_file(file.c_str());
            if (!calib_res_file.is_open()) {
                throw std::exception();
            }
            std::string line;
            while (getline(calib_res_file, line)) {
                std::istringstream iss(line);
                Results calib_data;
                unsigned int boardID, ch256;
                if (!(iss >> boardID >> ch256 >> calib_data._value >> calib_data._value_error >> calib_data._pedestal_2d >> calib_data._pedestal_error_2d)) { break; }
                if(calib_data._value == BadValue_HG)
                    ++_bad_fit;
                unsigned int globalChannel = (1<<23)|(boardID << 8) | ch256;
                _globalChannels.insert(globalChannel);
                _res.insert({globalChannel,calib_data});
            }
        }
    }
    else{
        std::cerr << "There are no txt files with calibration in the directory" << std::endl;
    }
    std::cout << "Number of channels without HG/LG calibration: " <<_bad_fit<<std::endl;
    GetValueDistribution(0,0.35, - 150, 150);
}

const double LgHgReader::GetOnly2dPedestal(const unsigned int &global_channel) const{
    if(_res.find( global_channel ) != _res.end()) {
        if (_res.at(global_channel)._value == BadValue_HG) {
            return _mean_value_pedestal_2d;
        }
        return _res.at(global_channel)._pedestal_2d;
    }
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _mean_value_pedestal_2d;
}

const double LgHgReader::GetPedestal2d(const unsigned int &global_channel) const {
    if(_res.find( global_channel ) != _res.end())
        return _res.at(global_channel)._pedestal_2d;
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _mean_value_pedestal_2d;
}

const double LgHgReader::GetPedestal2dError(const unsigned int &global_channel) const {
    if(_res.find( global_channel ) != _res.end())
        return _res.at(global_channel)._pedestal_error_2d;
    std::cerr << "Unable to find global channel "<< global_channel << std::endl;
    return _std_value_pedestal_2d;
}