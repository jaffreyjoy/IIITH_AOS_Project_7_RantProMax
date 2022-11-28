#include<bits/stdc++.h>
#include<thread>
using namespace std;

//ofstream gout, nout;

int geeks, nonGeeks, singers;
bool geek_done=0, nonGeek_done=0, stop=0;
mutex bridge_lock;
int waqt=0;
string dash = "\n====================================================\n";

class bridge{
public:
	int geeks_on_bridge, nonGeeks_on_bridge, singers_on_bridge, available_slots;
	bridge(){
		geeks_on_bridge=0;
		nonGeeks_on_bridge=0;
		singers_on_bridge=0;
		available_slots=4; 
	}
	bool validate_arrangement(){
        int buffer = 0;
        // cout << waqt << endl;
 		if(geeks_on_bridge==3 && nonGeeks_on_bridge==1){
 			if(singers>0){
 				buffer = 1;
 				--nonGeeks_on_bridge;
 				++singers_on_bridge;
				--singers;
 			}
 			else{
 				cout << "\nAll remaining geeks and non geeks will starve to death\n"<< endl;
 				return 1;
 			}
		}
		else if(nonGeeks_on_bridge==3 && geeks_on_bridge==1){
			if(singers>0){
				buffer = 2;
				--geeks_on_bridge;
				++singers_on_bridge;
				--singers;
			}
			else{
				cout << "\nAll remaining geeks and non geeks will starve to death\n"<< endl;
 				return 1;
			}
		}

		cout << "Trip Started with :: ";
        for(int i=0; i<geeks_on_bridge; ++i)
        	cout << "Geek ";
        for(int i=0; i<nonGeeks_on_bridge; ++i)
        	cout << "Non-Geek ";
        for(int i=0; i<singers_on_bridge; ++i)
        	cout << "Singer ";
        cout << endl;

        if(buffer>0){
            if(buffer==1){
                geeks_on_bridge = 1;
                nonGeeks_on_bridge = 0;
            }
            else{
                geeks_on_bridge = 0;
                nonGeeks_on_bridge = 1;
            }
            available_slots = 3;
            singers_on_bridge=0;
            buffer = 0;
        }
        else{
            geeks_on_bridge=0;
            nonGeeks_on_bridge=0;
            singers_on_bridge=0;
            available_slots = 4;
        }
        cout << "Geek count :: " << geeks << endl << "Non geek count :: " << nonGeeks << dash;
        if(geeks+nonGeeks+geeks_on_bridge+nonGeeks_on_bridge < 4) return 1;
        return 0;
	}
};

void geekArrives(bridge *b){
	while(true){
		bridge_lock.lock();
		//gout << "lock acq at time :: " << waqt << endl;
		if(b->available_slots>0 && geeks>0){
			++b->geeks_on_bridge;
			--geeks;
			--b->available_slots;
		}
		if(b->available_slots==0){
			if(b->validate_arrangement()){
				//gout << "time at lock release :: " << waqt++ << dash;
				bridge_lock.unlock();
				break;
			}
		}
		//gout << "time at lock release :: " << waqt++ << dash;
		bridge_lock.unlock();
		if(geeks<=0)break;
	}
	geek_done=1;
}

void nonGeekArrives(bridge *b){
	while(true){
		bridge_lock.lock();
		//nout << "lock acq at time :: " << waqt << endl;
		if(b->available_slots>0 && nonGeeks>0){
			++b->nonGeeks_on_bridge;
			--nonGeeks;
			--b->available_slots;
		}
		if(b->available_slots==0){
			if(b->validate_arrangement()){
				//nout << "time at lock release :: " << waqt++ << dash;
				bridge_lock.unlock();
				break;
			}
		}
		//nout << "time at lock release :: " << waqt++ << dash;
		bridge_lock.unlock();
		if(nonGeeks<=0) break;
	}
	nonGeek_done=1;
}

int main(int argc, char **argv){
	geeks = atoi(argv[1]);
	nonGeeks = atoi(argv[2]);
	singers = atoi(argv[3]);
	bridge b = bridge();
	//gout.open("geek_logs.txt");
	//nout.open("ngeek_logs.txt");
	thread geek(geekArrives, &b);
	thread nonGeek(nonGeekArrives, &b);

	geek.join();
	nonGeek.join();
	//gout.close();
	//nout.close();
	return 0;
}
