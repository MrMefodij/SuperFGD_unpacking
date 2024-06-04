//
// Created by Maria on 18.10.2023 kolupanova@inr.ru
//
#ifndef UNPACKING_READER_H
#define UNPACKING_READER_H

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

#define BadValue_PED -999.9
#define BadValue_HG 0

class Reader {
protected:
    struct Results{
        double _value,_value_error;
        double _pedestal_2d = BadValue_PED;
        double _pedestal_error_2d = BadValue_PED;
        std::vector<double> _peaks_position;
    };
public:
    Reader(const std::vector<std::string> &files) : _files(files) {};

    virtual ~Reader() = default;

    virtual void Init() = 0;

    const std::set<unsigned int> GetAllChannels(){return _globalChannels;}

    /// If value global_channel in map _res wasn't found, function returns mean value (+ std::cerr)
    const std::pair<double, double> GetValueAndError(const unsigned int &global_channel) const;

    const double GetValue(const unsigned int &global_channel) const ;

    const double GetValueError(const unsigned int &global_channel) const ;

    /// Returns gain / pedestal value from map _res by key global_channel,
    /// if value is equal to value_to_change change value to the _mean_value
    const double GetOnlyValue(const unsigned int &global_channel) const;

    const double GetOnlyValue(const unsigned int &board_id, const unsigned int &channel_id) const;

protected:
    std::set<unsigned int>  _globalChannels;
    std::map<unsigned int, Results> _res;
    std::vector<std::string> _files;
    double _mean_value, _std_value;
    double _std_value_pedestal_2d = BadValue_PED;
    double _mean_value_pedestal_2d = BadValue_PED;
    unsigned int _bad_fit = 0;
    void GetValueDistribution(const int x_down, const int x_up, const int x_down_2d = 0, const int x_up_2d = 1);

};

class GainReader : public Reader {
public:

    GainReader(const std::vector<std::string> &files) : Reader(files) {}
    ~GainReader() override = default;

    void Init() override;

    /// If value global_channel in map _res wasn't found, function returns mean value (+ std::cerr)
    const std::vector<double> GetPeaks(const unsigned int& global_channel);


};



class PedMapReader : public Reader {
public:

    PedMapReader(const std::vector<std::string> &files) : Reader(files) {}
    ~PedMapReader() override = default;

    void Init() override;

    /// If value global_channel in map _res wasn't found, function returns mean value (+ std::cerr)
    const std::pair<double, double> GetValueAndError2d(const unsigned int& global_channel);

    const double GetPedestal2d(const unsigned int &global_channel) const ;

    const double GetPedestal2dError(const unsigned int &global_channel) const ;

    /// Returns pedestal2d value from map _res by key global_channel,
    /// if value is equal to value_to_change change value to the _mean_value_pedestal_2d
    const double GetOnly2dPedestal(const unsigned int &global_channel) const;
};

class LgHgReader : public Reader {
public:

    LgHgReader(const std::vector<std::string> &files) : Reader(files) {}
    ~LgHgReader() override = default;

    void Init() override;

    /// If value global_channel in map _res wasn't found, function returns mean value (+ std::cerr)
    const std::vector<double> GetPeaks(const unsigned int& global_channel);

    const double GetPedestal2d(const unsigned int &global_channel) const ;

    const double GetPedestal2dError(const unsigned int &global_channel) const ;

    /// Returns pedestal2d value from map _res by key global_channel,
    /// if value is equal to value_to_change change value to the _mean_value_pedestal_2d
    const double GetOnly2dPedestal(const unsigned int &global_channel) const;


};

#endif //UNPACKING_READER_H
