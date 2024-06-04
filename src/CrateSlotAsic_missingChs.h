//
// Created by Maria on 08.08.2023.
//

#ifndef UNPACKING_CRATESLOTASIC_MISSINGCHS_H
#define UNPACKING_CRATESLOTASIC_MISSINGCHS_H
#include <map>
#include <vector>
#include <iomanip>

struct Mapping{
    unsigned int _crate_id;
    unsigned int _slot_id;
    unsigned int _asic_id;
    bool operator < (const Mapping& other) const
    {
        if (_crate_id == other._crate_id) {
           if(_slot_id == other._slot_id)
               return _asic_id < other._asic_id;
           return _slot_id < other._slot_id;
        }
        return _crate_id < other._crate_id;
    }
    bool const operator == (const Mapping& other) const
    {
        return _crate_id == other._crate_id && _slot_id == other._slot_id && _asic_id == other._asic_id;
    }
};

class CrateSlotAsic_missingChs {
public:
    CrateSlotAsic_missingChs( const std::string& map_file);
    ~CrateSlotAsic_missingChs() = default;
    void Init();
    bool Is_Missing_Chs(Mapping connection, unsigned int ch);
    bool Is_Missing_FEB(unsigned int board_id);
    bool Is_Missing_ASIC(Mapping connection);

private:
   std::map<Mapping,std::vector<unsigned int>> _missing_chs;
   std::string _filename;
};


#endif //UNPACKING_CRATESLOTASIC_MISSINGCHS_H
