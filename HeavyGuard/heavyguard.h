#ifndef _heavyguard_h
#define _heavyguard_h

#include "../common/param.h"
#include "../common/BOBHash32.h"
#define HK_b 1.08
#define G 8
#define ct 32  // the number of cold items for each bucket
using namespace std;

class HeavyGuard{
private:
    BOBHash32 *bobhash;
    int M;
    int key_len = 4;
    int over_count =0;

public:
    struct node {
        int C;
        uint32_t FP;} HK[10000005][20];
    int ext[1000005][40];
 //   HeavyGuard(int M,int prm):M(M){bobhash = new BOBHash32(prm);}
    HeavyGuard(int memory){
        M = floor(memory * 8/ (64*G+10*ct));// 32bit for key,32bit for count in G; and 4 bit for small flow
        bobhash = new BOBHash32(1000);
    }
    void Print_basic_info(){
        cout<<"HeavyGuard"<<endl<<"\t bucket number: "<<M<<"\t guard number: "<<M*G<<"\t cold item number: "<<M*ct<<endl;
    }

    void Insert(uint32_t key){
        //uint32_t Hsh = (bobhash->run((const char*)&key,key_len)) % M;
        uint32_t H = bobhash-> run((const char*)&key,key_len);
        uint32_t Hsh = H % M;
        //uint32_t FP = (H>>16), Hsh = H%M;
    //    Hsh = H % M;
        uint32_t FP = key;
        bool FLAG = false;
        for(int k =0;k<G;k++){
            int c = HK[Hsh][k].C;
            if(HK[Hsh][k].FP == FP){
                HK[Hsh][k].C++;
                FLAG = true;
                break;
            }
            if (FLAG) break;
        }
        if(!FLAG){
            int X,MIN=1000000000;
            for(int k =0;k<G;k++){
                int c = HK[Hsh][k].C;
                if(c<MIN){MIN=c;X=k;}
            }
            if(!(rand()%int(pow(HK_b,HK[Hsh][X].C)))){
                HK[Hsh][X].C--;
                if(HK[Hsh][X].C<=0){
                    HK[Hsh][X].FP = FP;
                    HK[Hsh][X].C = 1;
                } else{
                    int p = H % ct;
                    if(ext[Hsh][p]+1<1024){
                        ext[Hsh][p]++;
                    }else{over_count++;}
                }
            }
        }
    }

    int Query(uint32_t key)
    {
  //      uint32_t Hsh = (bobhash-> run((const char*)&key,key_len)) % M;
 //       Hsh = H % M;
        uint32_t H = bobhash-> run((const char*)&key,key_len);
        uint32_t Hsh = H % M;
        uint32_t FP = key;
        //uint32_t FP = (H>>16),Hsh = H%M;
        for(int k =0;k<G;k++){
            int c = HK[Hsh][k].C;
            if(HK[Hsh][k].FP == FP) return max(1,c);
        }
        int p = H % ct;
        return max(1,ext[Hsh][p]);
    }
    void Print_overcount(){
        cout<<"HG overflow count :"<<over_count<<endl;
    }

    map<uint32_t,int> Query_threshold(uint32_t threshold){
        map<uint32_t,int> result;
        //int Hsh;
        //H = bobhash-> run((const char*)&key,key_len);
        for(int i=0;i<M;i++){
            for(int j =0;j<G;j++){
                if(HK[i][j].C >= (int)threshold)
                    result.insert(make_pair(HK[i][j].FP, HK[i][j].C));
            }
        }

        return result;
    }
};
#endif
