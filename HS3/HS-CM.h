#ifndef _HSCM_H
#define _HSCM_H
#include "../common/param.h"
#include "../common/BOBHash32.h"
using namespace std;
#define SLOT_NUM 8

class HSCM{
private:
    struct BUCKET_node{
        uint32_t key;
        uint32_t count;
    };
    struct CELL_node{
        BUCKET_node bucket[SLOT_NUM];
    };
    int hs_width,cm_width,cm_row,over_count=0;
    float hs_alpha, hs_beta;
    CELL_node *hs_cell;
    int *cm_bucket;
    BOBHash32 *hash[ROW_NUM];

public:
    HSCM(int memory, float ratio, float a, float b){
        hs_alpha = a;
        hs_beta = b;
        hs_width = floor((memory * ratio)/sizeof(CELL_node));
        hs_cell = new CELL_node[hs_width];
        cm_row = ROW_NUM;
        cm_width = floor((memory*(1-ratio)*8)/(10*cm_row));//10 bit for bucket in cm
        cm_bucket = new int[cm_row*cm_width];
        Clear();
    }

    void Insert(uint32_t key, int f=1){
        bool flag = 0;
        int index;
        // first insert in heavySep layer;
        index = (hash[0]->run((const char*)&key,KEY_LEN)) % hs_width;
        for(int i =0;i<SLOT_NUM;i++){
            if(hs_cell[index].bucket[i].key == key){
                flag = 1;
                hs_cell[index].bucket[i].key = key;
                hs_cell[index].bucket[i].count +=f;
                break;
            }
            if(flag) break;
        }
        if(!flag){
            int pos,MIN=10000000;
            for(int i=0;i<SLOT_NUM;i++){
                if((hs_cell[index].bucket[i].count & 0x00ffffff)< (uint32_t)MIN){
                    pos = i; MIN = hs_cell[index].bucket[i].count & 0x00ffffff;
                }
            }
            uint32_t tmp = hs_cell[index].bucket[pos].count & 0x00ffffff;
            if(rand()/double(RAND_MAX)<pow((hs_beta*1.0/tmp), hs_alpha)){
                if((hs_cell[index].bucket[pos].count>>31) ==0){
                    hs_cell[index].bucket[pos].count += 0x01000000;
                    if((hs_cell[index].bucket[pos].count>>24) >= tmp){
                        hs_cell[index].bucket[pos].count = 1;
                        hs_cell[index].bucket[pos].key = key;
                        flag = 1;
                    }else{
                        if((hs_cell[index].bucket[pos].count >> 24) > 100){
                            hs_cell[index].bucket[pos].count = (hs_cell[index].bucket[pos].count & 0x00ffffff) | 0x80000000;
                        }
                    }
                }
            }
        }
        // next insert to cm
        if(!flag){
            int pos,min_pos,min_value=0;
            for(int i=0;i<cm_row;i++){
                index = (hash[i]->run((const char*)&key,KEY_LEN)) % cm_width;
                pos = index + i*cm_width;
                if(cm_bucket[pos] + f <1024){cm_bucket[pos] += f;}
                else{over_count++;}
                cm_bucket[pos] += f;
            }
        }

    }

    int Query(uint32_t key){
        int index,pos,tmp_value;
        bool flag = 0;
        index = (hash[0]->run((const char*)&key,KEY_LEN)) % hs_width;
        for(int i =0;i<SLOT_NUM;i++){
            if(hs_cell[index].bucket[i].key == key){
                flag = 1;
                tmp_value = hs_cell[index].bucket[i].count & 0x00ffffff;
                return tmp_value;
            }
        }
        if(!flag){
            for(int i=0;i<cm_row;i++){
                index = (hash[i]->run((const char*)&key,KEY_LEN)) % cm_width;
                pos = index + i*cm_width;
                if(i==0||cm_bucket[pos]<tmp_value){tmp_value=cm_bucket[pos];}
            }
            return tmp_value;
        }
    }
    void Print_basic_info(){
        cout<<"HS_CM "<<endl;
        cout<<"hs memory: "<< hs_width*sizeof(CELL_node)*1.0/1024<<"KB"<<"\t bucket number: "<<hs_width*SLOT_NUM<<endl;
        cout<<"sketch memory: "<<cm_width*cm_row*10*1.0/1024/8<<"KB \t sketch bucket number:"<<cm_width*cm_row<<endl;
    }

    void Print_overcount(){
        cout<<"over count: "<<over_count<<endl;
    }
    void Clear(){
        for(int i=0;i<hs_width;i++){
            for(int j=0;j<SLOT_NUM;j++){
                hs_cell[i].bucket[j].count =hs_cell[i].bucket[j].count = 0;
            }
        }
        for(int i=0;i<cm_row;i++){hash[i] = new BOBHash32(i+750);}
    }

    map<uint32_t,int> Query_threshold(uint32_t threshold){
        map<uint32_t,int> result;
        uint32_t tmp;
        for(int i=0;i<hs_width;i++){
            for(int j=0;j<SLOT_NUM;j++){
                tmp = hs_cell[i].bucket[j].count & 0x00ffffff;
                if(tmp > threshold)
                    result.insert(make_pair(hs_cell[i].bucket[j].key, tmp));
            }
        }
        return result;
    }

};
#endif
