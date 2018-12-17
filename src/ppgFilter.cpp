/* 
 * PPG Viewer
 * Copyright (C) 2018 PPG Hou <https://github.com/ppghou>
 */
#include "ppgFilter.h"

int main(){
    vector<float> time;
    vector<float> data;
    loadCSV("../data/data_1031_0399_tRGB.csv", time, data);

    Filter filt(50);
    for (int i = 0; i < 2800; ++i){
        filt.input(data[i+10]);
    }

    return 0;
}
