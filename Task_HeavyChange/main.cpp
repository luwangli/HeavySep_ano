#include "./traceRead.h"
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
    map<uint32_t,int> cuh_thres,hg_thres,heavysep_thres,thres,thres1,thres2;

    int packet_num,total_memory,threshold;
    int given_packet_num = 40000000;
    //int given_packet_num = 9000000;
    int tmp;
    //char path[100] = "../data/2002.txt";
    char path[100];
    cout<<"Input data (for example: ../data/trace.txt)"<<endl;
    cin>>path;
    cout<<"Input memory(KB) and threshold (for example: 20 500)"<<endl;
    cin >>total_memory>>threshold;
    //threshold = tmp/2;
    packet_num = TraceHeavyChange(path, given_packet_num/2, insert_data, benchmark_data);

    vector<pair<uint32_t,int>> real_threshold_result;
    for(auto it:benchmark_data){
        if(it.second>threshold){
            real_threshold_result.push_back( make_pair(it.first,it.second));
        }
    }

    HEAVYSEP *heavysep1 = new HEAVYSEP(0.5*total_memory*1024,1,0.4,1);
    HEAVYSEP *heavysep2 = new HEAVYSEP(0.5*total_memory*1024,1,0.4,1);
    heavysep1->Print_basic_info();


    HeavyGuard *HG1 = new HeavyGuard(0.5*total_memory*1024);
    HeavyGuard *HG2 = new HeavyGuard(0.5*total_memory*1024);

    //HG->Print_basic_info();

    auto wavingsketch1 = new WavingSketch2(0.5*total_memory*1024);
    auto wavingsketch2 = new WavingSketch2(0.5*total_memory*1024);

    CUHeap *cuh1 = new CUHeap(0.5*total_memory*1024,400);
    CUHeap *cuh2 = new CUHeap(0.5*total_memory*1024,400);

    CMHeap *cmh1 = new CMHeap(0.5*total_memory*1024,400);
    CMHeap *cmh2 = new CMHeap(0.5*total_memory*1024,400);

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
        thres.clear();
        switch(t){
            case 0:{
                cout<<"*******************HeavySep2 result: *************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    if(i<packet_num/2){ heavysep1->Insert(insert_data[i]);}
                    else{heavysep2->Insert(insert_data[i]);}
                }
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres1 = heavysep1->Query_threshold(threshold);
                thres2 = heavysep2->Query_threshold(threshold);
                break;
            }
            case 1:{
                cout<<"*********************HeavyGuard RESULT: ******************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    if(i<packet_num/2){ HG1->Insert(insert_data[i]);}
                    else{HG2->Insert(insert_data[i]);}
                }
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres1 = HG1->Query_threshold(threshold);
                thres2 = HG2->Query_threshold(threshold);
                break;
            }
            case 2:{
                cout<<"*********************CU Heap RESULT: ******************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    if(i<packet_num/2){ cuh1->Insert(insert_data[i]);}
                    else{cuh2->Insert(insert_data[i]);}
                }
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres1 = cuh1->Query_threshold(threshold);
                thres2 = cuh2->Query_threshold(threshold);
                break;
            }
            case 3:{
                cout<<"*************CM heap Result********************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    if(i<packet_num/2){ cmh1->Insert(insert_data[i]);}
                    else{cmh2->Insert(insert_data[i]);}
                }
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres1 = cmh1->Query_threshold(threshold);
                thres2 = cmh2->Query_threshold(threshold);
                break;
            }
            case 4:{
                cout<<"**************WavingSketch Result*************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    if(i<packet_num/2){ wavingsketch1->Insert(insert_data[i]);}
                    else{wavingsketch2->Insert(insert_data[i]);}
                }
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                thres1 = wavingsketch1->Query_threshold(threshold);
                thres2 = wavingsketch2->Query_threshold(threshold);
                break;
            }
            default: cout<<"stop"<<endl;
        }

        for(auto it1 = thres1.begin();it1!=thres1.end();it1++){
            auto res = thres2.find(it1->first);
            if(res != thres2.end()){
                // key in thres1 also exisit in thres2;
                int dif = abs(abs(res->second) - abs(it1->second));
                if(dif >= threshold){
                    thres.insert(make_pair(res->first,dif));
                }
            }else{
                thres.insert(make_pair(it1->first,abs(it1->second)));
            }
        }
        for(auto it2 = thres2.begin();it2!=thres2.end();it2++){
            auto res = thres1.find(it2->first);
            if(res == thres1.end()){
                thres.insert(make_pair(it2->first,abs(it2->second)));
            }
        }

        for(auto it = thres.begin();it!=thres.end();it++){
            cnt++;
            for(auto itr = real_threshold_result.begin();itr!=real_threshold_result.end();itr++){
                if(it->first == itr->first){
                    tp++;
                    int val = abs(it->second - itr->second);
                    hh_ab_e += val;
                    hh_re_e += abs(val*1.0/itr->second);
                }
            }
        }

        hh_avg_ab_e = hh_ab_e * 1.0 / tp;
        hh_avg_re_e = hh_re_e * 1.0 / tp;
        precision = tp*1.0 / thres.size();
        recall = tp*1.0/real_threshold_result.size();
        f1 = 2*precision*recall / (precision + recall);
        cout << "real heavy change flow number(over  "<<threshold<<"): "<<real_threshold_result.size()<<endl;
        cout <<"heavy change flow number detected: "<<thres.size()<<endl;
        cout<<"precion: "<<precision<<"\t recall: "<<recall<<"\t f1 score: "<< f1<<endl;
        cout<<"Heavy flow detection, AAE: "<<hh_avg_ab_e<<"\t ARE: "<<hh_avg_re_e<<endl;

        timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
        insert_throughout = (double)1000.0*packet_num / timediff;
        cout<<"throughput: "<<insert_throughout<<endl;
        outFile<<path<<","<<total_memory<<","<<threshold<<","<<AL[t]<<","<<insert_throughout<<","
        <<precision<<","<<recall<<","<<f1<<","<<hh_avg_ab_e<<","<<hh_avg_re_e<<endl;

    }


}
