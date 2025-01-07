#include<bits/stdc++.h>
#include <fstream>
#include <cmath>

using namespace std;
using ll = long long;

//g++ -std=c++11 input_generator.cpp -o gen.exe

void input(int z){
    set<pair<ll, ll>> pairs;
    int T;
    ll V, M, x, y;
    string s;
    ofstream outfile("..\\..\\graphs\\graph_"+to_string(z)+".txt");
    srand (time(NULL));
	int counter= 1;
    T = rand() %(5*z+1)+ 2;
    if (outfile.is_open()){
        cout<<"(1):Open_file"<<endl;
        outfile<<T<<endl;
        //for (auto i=0; i<T; i++){
		for (auto i=0; i<1; i++){
            cout<<"(2):T-Recursion"<<endl;
            pairs.clear();
            V = (rand() %(ll(pow(5,z)))+1)*T; 
            M = rand() %(V*T + ll(pow(2,z))+2 +T);  
            cout<<"(2b):V,M"<<endl;
            outfile<<V<<" ";
            cout<<"V="<<V<<endl;
            if(V>1){
                for(auto j=0; j<M; j++){
                    cout<<"(3a):M-Recursion"<<endl;
                    x = y = 0;
                    while(x == y || (pairs.find(make_pair(y,x))!=pairs.end())){
                        cout<<"x="<<x<<" y="<<y<<endl;
                        x = rand() % V;
                        y = rand() % V;
                        cout<<"x="<<x<<" y="<<y<<endl;
                    } 
                    pairs.insert(make_pair(x,y));
                }
            }
            cout<<"(3b):Pairs"<<endl;
            outfile<<pairs.size()<<endl;
			
            for(auto &j : pairs)
			{
			  if (counter < pairs.size())
			  {
				outfile<<j.first<<" "<<j.second<<endl;  
			  }
			  
			  else
			  {
				outfile<<j.first<<" "<<j.second;  
			  }
			  counter++;
			}
                
            cout<<"(4):Finish"<<endl;
            
        }
        outfile.close();
    }
    else 
        cout<<"Unable to open input file"; 
    cout<<"(5):Exit";

}

int main(){
    ios_base::sync_with_stdio(false);
    int max_inputs;
    cin>>max_inputs;
    for(int z = 0; z < max_inputs; z++)
        input(z); 
    return 0;
}
