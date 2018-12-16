#ifndef __UTILS__
#define __UTILS__ 0x0101

// FUNC: load data from CSV
inline int loadCSV(const char* fileName, vector<float> &time, vector<float> &data) {
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
inline void clearVector(vector<T>& v){
    vector<T>().swap(v);
}

// FUNC: pop last element from vector
template <typename T>
inline T pop_last(vector<T>& v){
    if (v.size() == 0)
        return 0;
    v.pop_back();
}

#endif