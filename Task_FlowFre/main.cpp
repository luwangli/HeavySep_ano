#include "../common/traceRead.h"
#include "../common/param.h"
#include "../CUSketch/cu.h"
//#include "../ColdFilter/SC_CUSketch.h"
#include "../HeavyGuard/heavyguard.h"
//s#include "../CUSketch/cusketch.h"
#include "../CMSketch/cm.h"
#include "../HS3/HS-CU.h"
#include "../HS3/HS-CM.h"

#include "../elasticsketch/ElasticSketch.h"
#include "../ColdFilter/SC_CUSketch.h"
#include "../ASketch/ASketch.h"
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

    int packet_num;
    float hs_ratio, alpha, beta;
    char path[100] = "../data/trace.txt";
    //char path[100];
    //cout<<"Input data (for example: ../data/2002.txt) "<<endl;
   // cin >>path;
    cout<<"Input memory(KB) (150KB)"<<endl;
    //cin >>total_memory;
    const int total_memory = 150;
    cout <<"Input hs ratio, alpha,and beta (0.6 0.4 1): "<<endl;
    //cin>>hs_ratio>>alpha>>beta;
    hs_ratio = 0.6;
    alpha = 0.4;
    beta = 1;
    packet_num = TraceRead(path, insert_data, benchmark_data);

   // HS *hs = new HS(total_memory*1024,1,1);
   // hs->Print_basic_info();
   int threshold = 1000;
    vector<pair<uint32_t,int>> real_threshold_result;
    for(auto it:benchmark_data){
        if(it.second>threshold){
            real_threshold_result.push_back( make_pair(it.first,it.second));
        }
    }


    cout<<"******************algorithm paramter**************"<<endl;

    HSCU *hscu = new HSCU(total_memory*1024,hs_ratio,alpha,beta);
    hscu->Print_basic_info();
    cout<<"*****"<<endl;
    CMSketch *CM = new CMSketch(total_memory*1024);
    CM->Print_basic_info();
    cout<<"*****"<<endl;
    CUSketch *CU = new CUSketch(total_memory*1024);
    CU->Print_basic_info();
    cout<<"*****"<<endl;
    HeavyGuard *HG = new HeavyGuard(total_memory*1024);
    HG->Print_basic_info();
    cout<<"*****"<<endl;
    HSCM *hscm = new HSCM(total_memory*1024,hs_ratio,alpha,beta);
    hscm->Print_basic_info();
    cout<<"*****"<<endl;
    auto *asketch = new ASketch<total_memory*1024,32,3>();
    //first para is memory, second is ratios of filter layer, third is bucket in the SIMD layer
    auto cf_cu = new CUSketchWithSC<total_memory*1024,90,16>();
    //asketch->print_basic_info();
    #define HEAVY_MEM (30 * 1024)
    #define BUCKET_NUM (HEAVY_MEM / 64)
    #define TOT_MEM_IN_BYTES (100 * 1024)
    auto elastic = new ElasticSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();

    int report_val;
    ofstream outFile;
    outFile.open("result.csv",ios::app);
    char const *AL[7]={"HSCU","CMSketch","CUSketch","HSCM","ElasticSketch","ASketch","ColdFilter"};

    for(int t =0;t<7;t++){
        ab_e = 0;
        re_e = 0;
        switch(t){
            case 0:{
                cout<<"*******************HSCU RESULT: *************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    hscu->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                hscu->Print_overcount();

                for(auto itr:benchmark_data){
                    report_val = hscu->Query(itr.first);
                    ab_e += abs(report_val - itr.second);
                    re_e += abs((report_val-itr.second)/itr.second);
                }
                break;
            }
            case 1:{
                cout<<"*********************CM RESULT: ******************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    CM->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);

                for(auto itr:benchmark_data){
                    report_val = CM->Query(itr.first);
                    ab_e += abs(report_val - itr.second);
                    re_e += abs((report_val-itr.second)/itr.second);
                }

                break;
            }
            case 2:{
                cout<<"*********************CU RESULT: ******************"<<t<<endl;

                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    CU->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);

                for(auto itr:benchmark_data){
                    report_val = CU->Query(itr.first);
                    ab_e += abs(report_val - itr.second);
                    re_e += abs((report_val-itr.second)/itr.second);
                }
                break;
            }
            case 3:{
                cout<<"*******************HSCM RESULT: *************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    hscm->Insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);
                hscm->Print_overcount();

                for(auto itr:benchmark_data){
                    report_val = hscm->Query(itr.first);
                    ab_e += abs(report_val - itr.second);
                    re_e += abs((report_val-itr.second)/itr.second);
                }
                break;
            }

            case 4:{
                cout<<"*******************ElasticSketch RESULT: ***************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    elastic->insert((uint8_t*)&insert_data[i]);
                }
                clock_gettime(CLOCK_MONOTONIC, &end_time);

                for(auto itr:benchmark_data){
                    report_val = elastic->query((uint8_t*)&itr.first);
                    ab_e += abs(report_val - itr.second);
                    re_e += abs((report_val-itr.second)/itr.second);
                }
                break;

            }
            case 5:{
                cout<<"*********************ASketch RESULT: ******************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    asketch->insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);

                for(auto itr:benchmark_data){
                    report_val = asketch->query(itr.first);
                    ab_e += abs(report_val - itr.second);
                    re_e += abs((report_val-itr.second)/itr.second);
                }
                break;
            }
            case 6:{
                cout<<"*********************ColdFilter RESULT: ******************"<<t<<endl;
                clock_gettime(CLOCK_MONOTONIC, &start_time);
                for(int i=0;i<packet_num;i++){
                    cf_cu->insert(insert_data[i]);}
                clock_gettime(CLOCK_MONOTONIC, &end_time);

                for(auto itr:benchmark_data){
                    report_val = cf_cu->query(itr.first);
                    ab_e += abs(report_val - itr.second);
                    re_e += abs((report_val-itr.second)/itr.second);
                }
                break;
            }
            default: cout<<"stop"<<endl;
        }


        timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
        insert_throughout = (double)1000.0*packet_num / timediff;
        avg_ab_e = ab_e / benchmark_data.size();
        avg_re_e = re_e / benchmark_data.size();
        cout<<"throughput: "<<insert_throughout<<endl;
        cout<<"average absolute error: "<<avg_ab_e<<endl<<"average relative error: "<<avg_re_e<<endl;
        outFile<<path<<","<<total_memory<<","<<hs_ratio<<","<<alpha<<","<<beta<<","<<AL[t]<<","<<insert_throughout
        <<","<<avg_ab_e<<","<<avg_re_e<<endl;
    }

}
