#include "../common/traceRead.h"
#include "../common/param.h"
#include "./HeavyGuard.h"
#include "./HeavySep.h"
#include "./CUHeap.h"
#include "./CMHeap.h"
#include "./wavingsketch.h"

using namespace std;
uint32_t insert_data[MAX_PACKET];
unordered_map<uint32_t,int> benchmark_data;

int main()
{
    timespec start_time, end_time;
    long long timediff;
    double insert_throughout;
    float ab_e,re_e,avg_ab_e,avg_re_e;
    float hh_ab_e,hh_re_e,hh_avg_ab_e,hh_avg_re_e;
    float recall,precision,f1;
    int cnt =0;
    int tp =0;
    map<uint32_t,int> cuh_thres,hg_thres,heavysep_thres,thres;

    int packet_num,total_memory,threshold;

    char path[100];
    cout<<"Input data (for example: ../data/trace.txt)"<<endl;
    cin>>path;
    cout<<"Input memory(KB) and threshold (for example: 20 500)"<<endl;
    cin >>total_memory>>threshold;
    packet_num = TraceRead(path, insert_data, benchmark_data);

   // HS *hs = new HS(total_memory*1024,1,1);
   // hs->Print_basic_info();
 //  int threshold = 500;
    vector<pair<uint32_t,int>> real_threshold_result;
    for(auto it:benchmark_data){
        if(it.second>threshold){
            real_threshold_result.push_back( make_pair(it.first,it.second));
        }
    }

    HEAVYSEP *heavysep = new HEAVYSEP(total_memory*1024,1,0.6,1);
    heavysep->Print_basic_info();

    HeavyGuard *HG = new HeavyGuard(total_memory*1024);
    HG->Print_basic_info();

    auto wavingsketch = new WavingSketch2(total_memory*1024);

    CUHeap *cuh = new CUHeap(total_memory*1024,400);

    CMHeap *cmh = new CMHeap(total_memory*1024,400);

    int report_val;
    ofstream outFile;
    outFile.open("result.csv",ios::app);
    char const *AL[6]={"HeavySep","HeavyGuard","CUHeap","CMHeap","WavingSketch"};
    for(int t =0;t<5;t++){
        ab_e = 0;
        re_e = 0;
        hh_ab_e = 0;
        hh_re_e = 0;
        tp = 0;
        recall =0;
        precision = 0;
        f1 = 0;
        switch(t){
            case 0:{
                cout<<"*******************HeavySep2 result: *************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    heavysep->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres = heavysep->Query_threshold(threshold);
                break;
            }
            case 1:{
                cout<<"*********************HeavyGuard RESULT: ******************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    HG->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres = HG->Query_threshold(threshold);
                break;
            }
            case 2:{
                cout<<"*********************CU Heap RESULT: ******************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    cuh->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres = cuh->Query_threshold(threshold);
                break;
            }
            case 3:{
                cout<<"*************CM heap Result********************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    cmh->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres = cmh->Query_threshold(threshold);
                break;
            }
            case 4:{
                cout<<"**************WavingSketch Result*************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    wavingsketch->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres = wavingsketch->Query_threshold(threshold);
                break;
            }
            default: cout<<"stop"<<endl;
        }

        for(auto it = thres.begin();it!=thres.end();it++){
            cnt++;
            for(auto itr = real_threshold_result.begin();itr!=real_threshold_result.end();itr++){
                if(it->first == itr->first){
                    tp++;
                    hh_ab_e += abs(it->second - itr->second);
                    hh_re_e += abs((it->second-itr->second)*1.0/itr->second);
                }
            }
        }

        hh_avg_ab_e = hh_ab_e * 1.0 / tp;
        hh_avg_re_e = hh_re_e * 1.0 / tp;
        precision = tp*1.0 / thres.size();
        recall = tp*1.0/real_threshold_result.size();
        f1 = 2*precision*recall / (precision + recall);
        cout << "real heavy flow number(over  "<<threshold<<"): "<<real_threshold_result.size()<<endl;
        cout <<"heavy flow number in hs: "<<thres.size()<<endl;
        cout<<"precion: "<<precision<<"\t recall: "<<recall<<"\t f1 score: "<< f1<<endl;
        cout<<"Heavy flow detection, AAE: "<<hh_avg_ab_e<<"\t ARE: "<<hh_avg_re_e<<endl;

        timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
        insert_throughout = (double)1000.0*packet_num / timediff;
        cout<<"throughput: "<<insert_throughout<<endl;
        outFile<<path<<","<<total_memory<<","<<threshold<<","<<AL[t]<<","<<insert_throughout<<","
        <<precision<<","<<recall<<","<<f1<<","<<hh_avg_ab_e<<","<<hh_avg_re_e<<endl;

    }

}
