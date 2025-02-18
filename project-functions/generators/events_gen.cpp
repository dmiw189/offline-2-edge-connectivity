#include<bits/stdc++.h>
using namespace std;
using ll = long long;

struct hashFunction{
   size_t operator()(const pair<int ,int> &x) const{return x.first ^ x.second;}
};

void fun(string z){
    srand(time(NULL));
    int T, query;
    ll N, M, a, b, NOEvents, e;
    unordered_set<pair<ll,ll>, hashFunction>:: iterator itr;
    unordered_set<pair<ll, ll>, hashFunction> edges;
    vector<tuple<char, ll, ll>> events;

    for (int i=0; i<5; i++) {
        ofstream outfile("..\\..\\events\\events_"+ z + "." + to_string(i) + ".txt");
        if(outfile.is_open()){
            ifstream infile("..\\..\\graphs\\graph_"+z+".txt");
            if(infile.is_open()){
                infile>>T;
                outfile<<T<<endl;
                for(auto t=0; t<1;t++){
                    edges.clear();
                    events.clear();
                    infile>>N>>M;
                    for(auto m=0; m<M; m++){
                        infile>>a>>b;
                        edges.insert(make_pair(a, b));
                    }
                    NOEvents = N*(i+1);
                    // if(t%2 == 0)
                    //     while(NOEvents < N)
                    //         NOEvents = N + rand()%N;
                    cout<<NOEvents<<endl;
                    outfile<<NOEvents<<endl;
                    e = 0;
                    while(e<NOEvents){
                        query = rand()%3;

                        if(query == 0 && M <N*(N-1)/2){//Insert 
                            for(;;){
                                a = rand()%N;
                                b = rand()%N;
                                //cout<<"I("<<a<<","<<b<<")\n";
                                if(a == b || edges.find(make_pair(a, b)) != edges.end() || edges.find(make_pair(b, a)) != edges.end())
                                    continue;
                                else{
                                    M++;;
                                    events.push_back(make_tuple('I', a, b));
                                    edges.insert(make_pair(a, b));
                                    e++;
                                    break;
                                }
                            }
                        }
                        else if(query == 1){//Delete
                                if(M>0){
                                    itr = edges.begin(); 
                                    advance(itr, rand()%M);
                                    a = (*itr).first;
                                    b = (*itr).second;
                                    //cout<<"D("<<a<<","<<b<<")\n";
                                    M--;
                                    events.push_back(make_tuple('D', a, b));
                                    edges.erase(itr);
                                    e++;
                                }
                        }
                        else if(query == 2){//Query         
                            do{
                                a = rand()%N;
                                b = rand()%N;
                                //cout<<"Q("<<a<<","<<b<<")\n";
                            }while(a == b);
                            events.push_back(make_tuple('Q', a, b));
                            e++;
                        }
                    } 

                    for(auto &item : events)
                        outfile<<get<0>(item)<<" "<<get<1>(item)<<" "<<get<2>(item)<<endl;

                }

                infile.close();
            }else cout<<"Unable to open graph"+z+".txt";

            outfile.close();
        } else cout<<"Unable to open events"+z+".txt";
    }
}

int main(){
    ios_base::sync_with_stdio(false);
    int a, b;
    do{
        cout<<"Please give a\n";
        cin>>a;
    }while(a >8 || a<0);

    do{
        cout<<"Please give b\n";
        cin>>b;
    }while(b>8 || b<0 || b<a);

    for(auto i=a; i<=b; i++)
        fun(to_string(i));
}