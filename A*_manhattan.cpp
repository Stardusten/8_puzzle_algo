// by Stardust 2020.12.2

/* Tags: 1. A*
         2. Manhattan
         3. Bitmap
         4. Use Cantor expansion to encode
*/
#include<iostream>
#include<queue>
#include<algorithm>
using namespace std;

// preprocess d[pos_0][move_direction]=new_pos_0
// move_direction: 0-Up, 1-Down, 2-Left, 3-Right
// val: -1-illegal move, 0~8: new_pos_0
const int d[9][4]={
        {-1,3,-1,1},
        {-1,4,0,2},
        {-1,5,1,-1},
        {0,6,-1,4},
        {1,7,3,5},
        {2,8,4,-1},
        {3,-1,-1,7},
        {4,-1,6,8},
        {5,-1,7,-1}
};

// State structure
struct state{
    int s[9]; // 0 represents Space
    int pos; // Position of space
    int steps; // Steps to reach current state
    int h; // Heuristic function
    int f;
    int fa;
}ini,goal;

bool operator<(const state & x, const state & y){ // Reload operator < for priority_queue
    return x.f>y.f;
}

// Use vector to save all the states
vector<state> v;
int v_end=-1; // End index

// Preprocess factor range 0~9 for Cantor expansion
int fact[10];

// Use Cantor expansion to encode all the permutations of 123456780 to integer range 0~9!
int encode(int *p){
    int code=0;
    for(int i=0;i<9;i++){
        int cnt=0;
        for(int j=i+1;j<9;j++)
            if(p[j]<p[i])  cnt++;
        code+=fact[8-i]*cnt;
    }
    return code;
}

// Ceil function for bitmap constructor
inline int ceil(double x){
    return x>(int)x?(int)x+1:(int)x;
}

// Use bitmap as Closed list
class bitmap{
private:
    int* buf;
public:
    bitmap(): buf(nullptr){}
    bitmap(int siz){
        buf=new int[ceil(siz/32)];
    }
    ~bitmap(){
        delete [] buf;
        buf=nullptr;
    }
    void visit(int);
    bool is_visited(int);
};

bitmap vis(362880);

// Use queue as Open list
// To save all the states, we use STL vector to simulate queue
priority_queue<state> q;

// preprocess manhattan[pos_1][pos_2] = manhattan distance between block 1 and block 2
const int manhattan[9][9]={
        0,1,2,1,2,3,2,3,4,
        1,0,1,2,1,2,3,2,3,
        2,1,0,3,2,1,4,3,2,
        1,2,3,0,1,2,1,2,3,
        2,1,2,1,0,1,2,1,2,
        3,2,1,2,1,0,3,2,1,
        2,3,4,1,2,3,0,1,2,
        3,2,3,2,1,2,1,0,1,
        4,3,2,3,2,1,2,1,0
};

int calc_manhattan(const state & cur,const state & goal){
    int dis=0;
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(cur.s[i]==goal.s[j]){
                dis+=manhattan[i][j];
            }
        }
    }
    return dis;
}

// Determine if there is a solution
bool is_solvable(const state & ini, const state & goal){
    int n1=0,n2=0; // Inverse number
    for(int i=0;i<9;i++){
        for(int j=i+1;j<9;j++){
            if(ini.s[i]>ini.s[j] && ini.s[j]!=0) n1++;
            if(goal.s[i]>goal.s[j] && goal.s[j]!=0) n2++;
        }
    }
    if((n1&1)!=(n2&1)) return 0; // Same parity => Solvable
    return 1; // Different parity => Insolvable
}

void print(const int & fa){ // Print path recursively
    if(fa==-1) return;
    print(v[fa].fa);
    for(int i=0;i<9;i++)
        cout<<v[fa].s[i]<<' ';
    cout<<endl;
}

bool bfs(){
    // Initialize factor[]
    fact[0]=1; // 0!=1
    for(int i=1;i<9;i++)
        fact[i]=i*fact[i-1];
    // Initialize ini state
    ini.steps=0;
    ini.h=calc_manhattan(ini,goal);
    ini.f=ini.steps+ini.h;
    ini.fa=-1;
    q.push(ini);
    while(!q.empty()){
        state cur=q.top();
        v.push_back(cur);
        v_end++;
        q.pop();
        if(!cur.h){ // Find!
            cout<<"steps: "<<cur.steps<<endl;
            print(v_end);
            return true;
        }
        for(int i=0;i<4;i++){ // try 4 direction
            if(~d[cur.pos][i]){ // If current move is accepted
                // Construct the new state after move
                state next;
                memcpy(next.s, cur.s, sizeof(cur.s));
                next.pos=d[cur.pos][i];
                swap(next.s[cur.pos],next.s[next.pos]);
                next.steps=cur.steps+1;
                next.h=calc_manhattan(next,goal);
                next.f=next.steps+next.h;
                next.fa=v_end;
                int code=encode(next.s);
                if(!vis.is_visited(code)){
                    vis.visit(code);
                    q.push(next);
                }
            }
        }
    }
    return false;
}

void input(state & st){
    // flexible input method
    char ch;
    for(int i=0;i<9;){
        ch=getchar();
        if(ch=='0') st.pos=i;
        if(ch>='0' && ch<='9') st.s[i++]=ch-'0';
    }
}

int main(){
    ios::sync_with_stdio(false);
    clock_t start,end;
    freopen("Test.in","r",stdin);
    freopen("Test.out","w",stdout);
    input(ini);
    input(goal);
    start=clock();
    if(is_solvable(ini,goal)) bfs();
    else cout<<-1<<endl;
    end=clock();
    cout<<"time: "<<end-start<<"ms"<<endl;
    fclose(stdin);
    fclose(stdout);
    return 0;
}

void bitmap::visit(int n){
    /*
    n is in the j-th bit of the i-th int
    +----------+----------+----------+----   ---+----------+----------
    |  1st int |  2nd int |  3rd int |          | i-th int |
    +----------+----------+----------+----   ---+----------+----------
                                                       ↑
                                                    j-th bit
    */
    int i=n/32;
    int j=n&31; // Equivalent to j=n%32;
    buf[i]|=(1<<j);
    /*
       To change the j-th bit from 0 to 1,
       we need to construct a number whose j-th bit is 1 and the rest bits are 0 (1<<j),
       then OR the original number with the constructed number.
       e.g      1100 0000
                0000 0010
            OR  1100 0010
    */
}

bool bitmap::is_visited(int n){
    int i=n/32;
    int j=n&31;
    return ((buf[i]&(1<<j))!=0);
    /*
       To check the j-th bit,
       we need to construct a number whose j-th is 1 and the rest bits are 0 (1<<j),
       then AND the original number with the constructed number.
       e.g      1100 0000			        1100 0000
                0000 0010			        0100 0000
           AND  0000 0000 (false)      AND  0100 0000 (true)
    */
}