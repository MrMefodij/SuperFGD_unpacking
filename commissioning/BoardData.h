//
// Created by amefodev on 13.07.2023.
//

#ifndef UNPACKING_BOARDDATA_H
#define UNPACKING_BOARDDATA_H

struct AsicData {
    unsigned int asicId;
    int hgValue;
    int lgValue;
};

template<typename T>
struct BoardData {
    unsigned int boardId;
    std::vector<T> asicsData;

    void AddAsics(int id, const std::vector<T>& asData){
        boardId = id;
        asicsData =  asData;
    }
};
#endif //UNPACKING_BOARDDATA_H
