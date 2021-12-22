#ifndef _CUSKETCH_H
#define _CUSKETCH_H
#include "../common/param.h"
#include "../common/BOBHash32.h"
using namespace std;

class CUSketch{
private:
    int *bucket;
    int row;
    int width;
    BOBHash32 *hash[ROW_NUM];
public:
    CUSketch(int memory){
        row = ROW_NUM;
        width = floor(memory/(sizeof(int)*row));
        bucket = new int[row*width];
        for(int i=0;i<row;i++){hash[i] = new BOBHash32(i+750);}
    }
    void Print_basic_info(){
        cout<<"CUSketch"<<endl<<"\t bucket number: "<<row*width<<"\t width: "<<width<<endl;
    }
    void Insert(uint32_t key,int f=1){
        int index,pos,min_pos;
        int min_value=0;
        for(int i=0;i<row;i++){
            index = (hash[i]->run((const char*)&key,KEY_LEN)) % width;
            pos = index + i*width;
            if(i==0||bucket[pos]<min_value){
                min_value = bucket[pos];
                min_pos = pos;
            }
        }
        bucket[min_pos] += f;
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
};

#endif
