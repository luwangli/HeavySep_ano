#ifndef _CMSKETCH_H
#define _CMSKETCH_H
#include "../common/param.h"
#include "../common/BOBHash32.h"
using namespace std;
class CMSketch
{
private:
    int *bucket;
    BOBHash32 *hash[ROW_NUM];
    int row;
    int width;

public:
    CMSketch(int memory){
        row = ROW_NUM;
        width = floor(memory/(sizeof(int)*row));
        bucket = new int[row*width];
        for(int i =0;i<row;i++){hash[i] = new BOBHash32(i+750);};
    }
    void Print_basic_info(){
        cout<<"CMSketch"<<endl<<"\t bucket number: "<<row*width<<"\t width: "<<width<<endl;
    }
    void Insert(uint32_t key, int f=1){
        int index,pos;
        for(int i =0;i<row;i++){
            index = (hash[i]->run((const char*)&key,KEY_LEN)) % width;
            pos = index+ width*i;
            bucket[pos]+=f;
        }
    }
    int Query(uint32_t key){
        int index,pos;
        int res = 0;
        for(int i =0;i<row;i++){
            index = (hash[i]->run((const char*)&key,KEY_LEN)) % width;
            pos = i*width + index;
            if(i==0||bucket[pos]<res){res = bucket[pos];}
        }
        return res;
    }
};
#endif
