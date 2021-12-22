#include "../common/traceRead.h"
#include "../common/param.h"

#include "../ColdFilter/SC_CUSketch.h"
#include "../ColdFilter/ASketch.h"

using namespace std;
uint32_t insert_data[MAX_PACKET];
unordered_map<uint32_t,int> benchmark_data;

int main()
{
    timespec start_time, end_time;
    long long timediff;
    double insert_throughout;
    float ab_e,re_e,avg_ab_e,avg_re_e;
    int report_val;

    int packet_num,total_memory;
//    char path[100] = "../data/s5.9.txt";
    char path[100];
    cout<<"Input data (for example: ../data/2002.txt"<<endl;
    cin >> path;
    //cout<<"Input memory(KB) (for example: 20)"<<endl;
    //cin >>total_memory;
    packet_num = TraceRead(path, insert_data, benchmark_data);
    ofstream outFile;
    outFile.open("CFresult.csv",ios::app);


        const int Mem = 100;
    auto cf_cu = new CUSketchWithSC<Mem*1024,90,16>();
    auto asketch = new ASketch<Mem*1024,32,3>();
    /*for(int i=0;i<packet_num;i++){
        cf_cu->insert(insert_data[i]);
    }
    cf_cu->synchronize();*/
    ab_e = 0;
    re_e = 0;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
        for(int i=0;i<packet_num;i++){
            cf_cu->insert(insert_data[i]);
        }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    cf_cu->synchronize();
    for( auto itr: benchmark_data){
        report_val = cf_cu->query(itr.first);
        ab_e += abs(report_val - itr.second);
        re_e += abs((report_val - itr.second)/itr.second);
    }
    cout<<"CF_CU result"<<endl;
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughout = (double)1000.0*packet_num / timediff;
    avg_ab_e = ab_e / benchmark_data.size();
    avg_re_e = re_e / benchmark_data.size();
    cout<<"throughput: "<<insert_throughout<<endl;
    cout<<"average absolute error: "<<avg_ab_e<<endl<<"average relative error: "<<avg_re_e<<endl;
    outFile<<path<<","<<Mem<<",CFCU,"<<insert_throughout<<","<<avg_ab_e<<","<<avg_re_e<<endl;


//***********************************ASKETHCH
    ab_e = 0;
    re_e = 0;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
        for(int i=0;i<packet_num;i++){
            asketch->insert(insert_data[i]);
        }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    //cf_cu->synchronize();
    for( auto itr: benchmark_data){
        report_val = asketch->query(itr.first);
        ab_e += abs(report_val - itr.second);
        re_e += abs((report_val - itr.second)/itr.second);
    }
    cout<<"ASketch result"<<endl;
    timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughout = (double)1000.0*packet_num / timediff;
    avg_ab_e = ab_e / benchmark_data.size();
    avg_re_e = re_e / benchmark_data.size();
    cout<<"throughput: "<<insert_throughout<<endl;
    cout<<"average absolute error: "<<avg_ab_e<<endl<<"average relative error: "<<avg_re_e<<endl;
    outFile<<path<<","<<Mem<<",ASketch,"<<insert_throughout<<","<<avg_ab_e<<","<<avg_re_e<<endl;

}
