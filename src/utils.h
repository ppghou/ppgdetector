/* 
 * PPG Viewer
 * Copyright (C) 2018 PPG Hou <https://github.com/ppghou>
 */
#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// FUNC: load data from CSV
inline int loadCSV(const char* fileName, std::vector<float> &time, std::vector<float> &data) {
    float t, R, G, B;
    string line; char c;
    ifstream file(fileName);
    while (getline(file, line)) {
        stringstream strm(line);
        strm >> t >> c >> R >> c >> G >> c >> B;
        time.push_back(t);
        data.push_back(G);
    }
    file.close();
    return 0;
}

// FUNC: release vector content
template <typename T>
inline void clearVector(std::vector<T>& v){
    std::vector<T>().swap(v);
}

// FUNC: pop last element from vector
template <typename T>
inline T pop_last(std::vector<T>& v){
    if (v.size() == 0)
        return 0;
    v.pop_back();
}

#endif // __UTILS_H__