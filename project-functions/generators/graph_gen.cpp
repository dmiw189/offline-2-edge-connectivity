#include<bits/stdc++.h>
#include <fstream>
#include <cmath>

using namespace std;
using ll = long long;

// Hash function
struct hashFunction{
   size_t operator()(const pair<int ,int> &x) const{return x.first ^ x.second;}
};

void GenRandomGraphs(ll NOEdge, ll NOVertex, unordered_set<pair<ll, ll>, hashFunction> &edge){
   srand(time(NULL));
   ll i;
   pair<ll, ll> epair;
   i = 0;
   //Assign random values to the number of vertex and edges of the graph, Using rand().
   while(i < NOEdge){
      epair = make_pair(rand()%NOVertex, rand()%NOVertex);
      //Print the connections of each vertex, irrespective of the direction.
      if(epair.first == epair.second || edge.find(epair) != edge.end() || edge.find(make_pair(epair.second, epair.first)) != edge.end())
         continue;
      else{
         edge.insert(epair);
         i++;
      }
   }
}

ll max(ll a, ll b){
   return (a>b)?a:b;
}

int main(){
   ll M, N;
   int T, a, b;
   srand(time(NULL));
   cin>>a>>b;
   for(auto i=a; i<=b; i++){
      ofstream outfile("..\\..\\graphs\\graph_"+to_string(i)+".txt");
      if(outfile.is_open()){
         T = (i + rand()%(i+5))>>1;
         outfile<<T<<endl;
         cout<<"T="<<T;
         for(auto j=0; j<T; j++){
            N = (max(i+(j>>1)+6, ll(pow(10, (2*i+j)/2 -2)))) + rand()%(max(i+(j>>1)+6, ll(pow(10, ((2*i+j)>>1)-3))));
            cout<<", N="<<N;
            M = rand()%((N*(N-1))/2);
            if(j%2 == 0)
               while(M < N)
                  M = (N + rand())%((N*(N-1))/2);
            cout<<" and M="<<M<<endl;
            outfile<<N<<" "<<M<<endl;
            
            unordered_set<pair<ll, ll>, hashFunction> edge;

            GenRandomGraphs(M, N, edge);

            pair<ll, ll> epair;
            for(auto &item : edge){
               epair = item;
               outfile<<epair.first<<" "<<epair.second<<endl;
            }
         }
         outfile.close();
      }
      else cout<<"Unable to open graph_"+to_string(i)+".txt";
   }
   
   return 0;
}