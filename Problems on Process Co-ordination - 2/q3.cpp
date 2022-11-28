#include<bits/stdc++.h>
#include <pthread.h>
#include <fstream>




#define uint unsigned int
#define ll long long int
#define endl "\n"
#define uint unsigned int
#define _bsc_ 0
#define _msc_ 1
#define _phd_ 2
#define _pdc_ 3
#define _degree_count_ 4
#define _ks1_ 0
#define _ks2_ 1
#define _ks3_ 2
#define _ks4_ 3
#define _ks_count_ 4
#define _pref_count_ 8
#define _student_ratio_sum_ 5


using namespace std;
pthread_mutex_t loc;
ofstream fout;
string output="";
string outpu="";

class Student{
public:
    uint degree;
    vector<vector<uint>> preference;
    vector<int> enrolled;
    Student(uint degree, vector<vector<uint>> &preference){
        this->degree = degree;
        this->preference = preference;
        enrolled.resize(4,-1);
    }
};
 


struct thread_args
{
    vector<vector<vector<int>>> *seats;
    Student* pref;
    int sid;
    // thread_args(){}
    // ~thread_args(){};
};




// class Plist{
//     public:
//         uint degree;
//         vector<vector<uint>> preference;
//         Plist(uint degree, vector<vector<uint>>& preference){
//             this->degree = degree;
//             this->preference = preference;
//         }
// };

uint rand_range(uint min_val, uint max_val){
    uint range = max_val - min_val + 1;
    uint num = rand() % range + min_val;
    return num;
}

Student* generate_preference(uint degree, uint cquota[]){
    uint pref_made_count;
    uint pref_left;
    vector<uint> pref_count(_ks_count_);
    while(pref_made_count!=_pref_count_){
        pref_made_count = 0;
        pref_left = _pref_count_;
        for(uint i=0;i<_ks_count_;i++){
            uint curr_cquota = cquota[i];
            uint course_pref_count = rand_range(1,min(pref_left - (_ks_count_ - (i+1)), curr_cquota));
            pref_count[i]=course_pref_count;
            pref_made_count+=course_pref_count;
            pref_left-=course_pref_count;
        }
    }
    vector<vector<uint>> prefs;
    uint rand_course_id;
    for(uint i=0;i<_ks_count_;i++){
        vector<uint> pref;
        for(uint j=0;j<pref_count[i];j++){
            rand_course_id = rand_range(0,cquota[i]-1);
            while(find(pref.begin(), pref.end(), rand_course_id)!=pref.end()) rand_course_id = rand_range(0,cquota[i]-1);
            pref.push_back(rand_course_id);
        }
        prefs.push_back(pref);
    }
    return new Student(degree, prefs);
}


// pthread_mutex_t x;

void* allocate(void* targsv){
    struct thread_args *targs = (struct thread_args *)targsv;
    vector<vector<vector<int>>>& seats = *(targs->seats);
    Student* s = targs->pref;
    int sid=targs->sid;
    bool fail=0;
    pthread_mutex_lock(&loc);
    for(int i=0; i<4; ++i){
        for(auto pref_course: s->preference[i]){
            if(seats[i][pref_course][s->degree]>0){
                s->enrolled[i]=pref_course;
                break;
            }
        }
    }
    for(auto course:s->enrolled){
        if(course==-1){
            fail=1;
            break;
        }
    }

    if(!fail){
        // fout<<"student:"<<sid<<" got allocated course";
        string temp="";
        for(int i=0; i<4; ++i){
            
            seats[i][s->enrolled[i]][s->degree]--;
            temp+=to_string(s->enrolled[i])+" ";
        }
        // fout<<temp<<endl;
         outpu+="Student "+to_string(sid)+" of branch "+to_string(s->degree)+" "+"got allocated:"+temp+"\n";
    }
    else{
        // fout<<"student:"<<sid<<" is not allocated any course"<<endl;
        output+="Student "+to_string(sid)+" of branch "+to_string(s->degree)+" "+"is not allocated"+"\n";
        // for(int i=0; i<4; ++i){
        //     if(s->enrolled[i]==-1){
        //         cout << "ks[" << i <<"]: ";
        //         for(auto cno: s->preference[i]){
        //             cout << cno << " ";
        //         }
        //         cout << endl;
        //         break;
        //     }    
        // }
        s->enrolled = {-1,-1,-1,-1};
    }
    pthread_mutex_unlock(&loc);
    return NULL;
}

int main(int argc, char** argv){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    ll n;
    vector<pthread_t> tids;
    // void *start_func(void *), *arg;
    fout.open("allocation.txt");

    if(argc!=3){
        cout<<"Incorrect arguments passed"<<endl;
    }
    // fout<<argv[1];
    // fout<<argv[2];
    // fout<<"holllllidk";
    
    ll students=atoi(argv[1]);
    ll total_courses_count=atoi(argv[2]);
    ll course_capacity=60;


    uint student_count = students;
    uint course_count = total_courses_count;
    uint scb5 = student_count/_student_ratio_sum_;
    uint ccb5 = course_count/_ks_count_;
    uint scb5t2 = scb5*2;
    uint squota[4] = {scb5,scb5,scb5t2,scb5};
    uint cquota[4] = {ccb5,ccb5,ccb5,ccb5};
    vector<Student*> all_prefs;
    
    while(squota[0]+squota[1]+squota[2]+squota[3] > 0){
        for(uint i=0;i<4;i++){
            if(squota[i] > 0){
                all_prefs.push_back(generate_preference(i, cquota));
                squota[i]--;
            }
        }
    }

    vector<vector<vector<int>>> seats(4);
    for(int i=0; i<4; ++i){
        seats[i].resize(total_courses_count/4, {12,12,24,12});
    }


    pthread_t thread[students];


    for(int i=0;i<students;i++){
        
        // pthread_create(&tid, NULL, allocate,(void *) &targs);
        pthread_t t;
        tids.push_back(t);
    }
    vector<struct thread_args> targs(students);
    for(int i=0;i<students;i++){
        targs[i].sid = i;
        targs[i].seats = &seats;
        targs[i].pref = all_prefs[i];
        pthread_create(&tids[i], NULL, allocate,(void *) &targs[i]);
    }
    for(int i=0;i<students;i++){
        pthread_join(tids[i],NULL);
    }
    cout<<output;
    fout<<outpu;
    for(int i=0; i<4; ++i){
            fout<<"Knowledge spectrum "<<i<<" has following students count in its courses:"<<endl;
        for(int j=0;j<(total_courses_count)/4;j++){
            fout<<"Course "<<j<<" has "<<60-(seats[i][j][0]+seats[i][j][1]+seats[i][j][2]+seats[i][j][3])<<" students"<<endl;
            // fout << "ks[" << i << "]." << j << ": " << 60-(seats[i][j][0]+seats[i][j][1]+seats[i][j][2]+seats[i][j][3]) << endl;
    
        //    seats[i][s->enrolled[i]][s->degree]--;
        //             temp+=to_string(s->enrolled[i])+" ";
        }
    }
    // allocate(seats, all_prefs[0]);
    return 0;
}