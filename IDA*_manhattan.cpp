// by Stardust 2020.12.2

/* Tags: 1. IDA*
         2. Manhattan
*/

#include<iostream>
#include<algorithm>
#include<stack>
using namespace std;

int max_f; // Maximun search f value

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
}ini,goal;

stack<state> st; // Save states to print path

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

bool IDA_star(state & cur, const int & steps, const int & h, const int & prepos){
    if(!memcmp(cur.s,goal.s,sizeof(goal.s))){ // Find!
        cout<<"steps: "<<steps<<endl;
        st.push(cur);
        return true; // Set return flag
    }
    // Reach max_f and no solution was found
    // Return recursively and search with bigger max_f later
    if(steps+h>max_f) return false;
    for(int i=0;i<4;i++){ // try 4 direction
        // If the next state is the same as the previous state,
        // there is a loop, so we need to skip this state
        // We choose to compare the position of 0 in the two states instead of using memcmp,
        // because you cannot move 0 back and move another block at the same time
        if(d[cur.pos][i]==prepos) continue;
        if(~d[cur.pos][i]){  // If current move is accepted
            // Construct the new state after move
            state next;
            memcpy(next.s,cur.s,sizeof(cur.s));
            next.pos=d[cur.pos][i];
            swap(next.s[cur.pos],next.s[next.pos]);
            if(IDA_star(next, steps+1, calc_manhattan(next,goal), cur.pos)){
                st.push(cur);
                return true; // Return recursively
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

void print(){
    while(!st.empty()){
        state & tmp=st.top();
        for(int i=0;i<9;i++)
            cout<<tmp.s[i]<<' ';
        cout<<endl;
        st.pop();
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
    max_f=calc_manhattan(ini,goal);
    if(is_solvable(ini,goal)){
        // Increase max_f iteratively until a solution was found
        while(!IDA_star(ini,0,max_f,-1)) max_f++;
    }else cout<<-1<<endl;
    print();
    end=clock();
    cout<<"time: "<<end-start<<"ms"<<endl;
    fclose(stdin);
    fclose(stdout);
    return 0;
}