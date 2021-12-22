//read data stream, return arrary

#ifndef _TRACEREAD_H
#define _TRACEREAD_H
#include "../common/param.h"

using namespace std;

#define MAX_PACKET 32000000
typedef pair<uint32_t, int> KV;

uint32_t str_to_dec(string str)
{
    uint32_t res = 0;
    for (uint32_t i=0; i <str.length(); i++)
    {
        if(str[i]>='0' && str[i]<='9')
            res = res *10 + str[i] - '0';
    }
    return res;
}

vector<pair<uint32_t,int>> topKFrequent(unordered_map<uint32_t,int> benchmark_data, int k)
{
    using T = pair<int,uint32_t>;
    // because in priority_queue, compare the first element at first
    // so we adjust the order of <key, value> to <value, key>
    vector<T> temp;
    priority_queue<T,vector<T>,greater<T>> heap;//min-heap
    for(auto itr:benchmark_data){
        T insert_element = make_pair(itr.second, itr.first);
        if(heap.size()>=(uint32_t)k){
            if(itr.second > heap.top().first){
                heap.pop();
                heap.push(insert_element);
            }
        }else heap.push(insert_element);

    }
    vector<pair<uint32_t,int>> res;
    while(!heap.empty()){
        res.push_back(heap.top());
        heap.pop();
    }
    cout<<"Real Top-k result: "<<endl;
    return res;
}

int TraceRead(char const *filename, uint32_t *datastream, unordered_map<uint32_t, int> &benchmark)
{
    FILE *Fin = fopen(filename,"rb");
    if(!Fin)
    {
        cerr << "cannot open file:" << filename <<endl;
        exit(-1);
    }

    ifstream fin(filename);
    string line;
    uint32_t res_dec;
    int count = 0;
    while (getline(fin, line)){
 //       cout << line <<endl;
        res_dec = str_to_dec(line);
 //       cout << res_dec <<endl;
        datastream[count] = res_dec;
        benchmark[res_dec]++;
        count++;
        if (count == MAX_PACKET ){
            cout << "reach max packet number" <<endl;
            break;
        }
    }
    fin.clear();
    fin.close();
    int flow_num = benchmark.size();
    cout<<"******************basic information********************"<<endl;
    cout<<"flow number: "<<flow_num<<"\t packet number: "<<count<<endl;
    return count;

}

int TraceHeavyChange(char const *filename,int half_num,uint32_t *datastream, unordered_map<uint32_t,int> &Thres){
    FILE *Fin = fopen(filename,"rb");
    if(!Fin)
    {
        cerr << "cannot open file:" << filename <<endl;
        exit(-1);
    }
    unordered_map<uint32_t,int> bench1,bench2;
    ifstream fin(filename);
    string line;
    uint32_t res_dec;
    int count=0;
    while(getline(fin,line)){
        res_dec = str_to_dec(line);
        datastream[count] = res_dec;
        if(count<half_num){
            bench1[res_dec]++;
            count++;
        }else{
            if(count<2*half_num){bench2[res_dec]++;count++;}
        }
        if(count == MAX_PACKET){cout<<"reach max packet number"<<endl; break;}
    }
    fin.clear();
    fin.close();
    //get heavy change flow
    for(auto it=bench1.begin();it!=bench1.end();it++){
        auto tmp = bench2.find(it->first);
        if(tmp!=bench2.end()){
             Thres.insert(make_pair(it->first,(it->second - tmp->second)));
        }else{
            Thres.insert(make_pair(it->first,it->second));
        }
    }
    for(auto it= bench2.begin();it!=bench2.end();it++){
        auto tmp = bench1.find(it->first);
        if(tmp == bench1.end()){
            Thres.insert(make_pair(it->first,it->second));
        }
    }

    int flow_num = bench1.size()+bench2.size();
    cout<<"******************basic information********************"<<endl;
    cout<<"flow number: "<<flow_num<<"\t packet number: "<<count<<endl;
    return count;
}

#endif
