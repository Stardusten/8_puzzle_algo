// by Stardust 2020.12.2

/* Tags: 1. BFS
         2. Bitmap
         3. Use Cantor expansion to encode
*/

#include<iostream>
#include<queue>
#include<algorithm>
#include<ctime>
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

// State structure
struct state{
    int s[9]; // 0 represents Space
    int pos; // Position of space
    int fa; // Save father state idx for each state to print the path
    int steps; // steps to reach current state
}ini,goal;

// Use queue as Open list
// To save all the states, we use STL vector to simulate queue
vector<state> q;

// Use bitmap as Closed list
bitmap vis(362880);

void input(state & st){
    // flexible input method
    char ch;
    for(int i=0;i<9;){
        ch=getchar();
        if(ch=='0') st.pos=i;
        if(ch>='0' && ch<='9') st.s[i++]=ch-'0';
    }
}

void print(int n){ // Recursive print path
    if(n==-1) return; // Reach ini state, return
    print(q[n].fa);
    for(int i=0;i<9;i++)
        cout<<q[n].s[i]<<' ';
    cout<<endl;
}

bool bfs(){
    // Initialize factor[]
    fact[0]=1; // 0!=1
    for(int i=1;i<9;i++)
        fact[i]=i*fact[i-1];
    ini.fa=-1; // Recursive termination condition
    ini.steps=0;
    q.push_back(ini); // Push ini state into the Open list
    int front=0, rear=1;
    while(front<rear){ // Loop until queue is empty
        state & cur=q[front];
        if(!memcmp(cur.s,goal.s,sizeof(goal.s))){ // Find!
            cout<<"steps: "<<cur.steps<<endl;
            print(front);
            return true;
        }
        for(int i=0;i<4;i++){ // try 4 direction
            if(~d[cur.pos][i]){ // If current move is accepted
                // Construct the new state after move
                state next;
                memcpy(next.s,cur.s,sizeof(cur.s));
                next.pos=d[cur.pos][i];
                swap(next.s[cur.pos],next.s[next.pos]);
                next.fa=front;
                next.steps=cur.steps+1;
                // Encode new state
                int code=encode(next.s);
                if(!vis.is_visited(code)){ // If the new state is unvisited
                    vis.visit(code); // Mark it as visited
                    q.push_back(next); // Add it to the Open list
                    rear++;
                }
            }
        }
        front++;
    }
    return false;
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