#ifndef _CMHEAP_H
#define _CMHEAP_H
#include "../common/param.h"
#include "../common/BOBHash32.h"
using namespace std;

class CMHeap{
private:
    int *bucket;
    int row;
    int width;
    int Threshold;
    map<uint32_t,int> heap;
    BOBHash32 *hash[ROW_NUM];

public:
    CMHeap(int memory,int t){
        row = ROW_NUM;
        Threshold = t;
        width = floor(memory/(sizeof(int)*row));
        bucket = new int[row*width];
        for(int i=0;i<row;i++){hash[i] = new BOBHash32(i+750);}
    }
    void Insert(uint32_t key, int f=1){
        int index,pos,min_pos;
        int min_value =0;
        for(int i=0;i<row;i++){
            index = (hash[i]->run((const char*)&key,KEY_LEN)) % width;
            pos = index + i*width;
            bucket[pos] += f;
            if(i==0||bucket[pos]<min_value){
                min_value = bucket[pos];
                min_pos = pos;
            }
        }

        if(heap.find(key) != heap.end()){
            heap.erase(key);
            heap.insert(make_pair(key,min_value));
        }else{
            if(min_value >= Threshold){
                heap.insert(make_pair(key,min_value));
            }
        }
    }
    int Query(uint32_t key){
        int index,pos;
        int res = 0;
        for(int i =0;i<row;i++){
            index = (hash[i]->run((const char*)&key,KEY_LEN)) % width;
            pos = i*width + index;
            if(i==0||bucket[pos]<res){
                res = bucket[pos];
            }
        }
        return res;
    }
    map<uint32_t,int> Query_threshold(uint32_t threshold){
        map<uint32_t,int> result;
        for(auto it=heap.begin();it!=heap.end();it++){
            if(it->second >= (int)threshold){
                result.insert(make_pair(it->first,it->second));
            }
        }
        return result;
    }
};
#endif
