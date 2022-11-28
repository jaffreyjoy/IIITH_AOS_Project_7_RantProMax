#include <iostream>
#include <vector>
#include <pthread.h>
#include <algorithm>
#include <unistd.h>
#include <queue>

using namespace std;

//thread vectors --START
queue<pthread_t> hydrogenThreads;
queue<pthread_t> oxygenThreads;
vector<pthread_t> sitesThreads;
//thread vectors --END

//Count variables
int hAtoms,oAtoms,nSites,threshold,currentEnergy=0;
int energyNeeded;
int done=false;

//Mutex --START
pthread_mutex_t hoLocker;
pthread_mutex_t energyLocker;
pthread_mutex_t energyLocker2;
vector<pthread_mutex_t> siteMutex;
//Mutex --END

//Output
vector<vector<int> > outputFormatted;

//Hydrogen
void* hydrogen(void* args)
{
	sleep(2);
	return NULL;
}

//Oxygen
void* oxygen(void* args)
{
	sleep(1);
	return NULL;
}

//Semaphore functions --START
void wait()
{
	while(energyNeeded<=0 && done==false);
	pthread_mutex_lock(&energyLocker2);
	energyNeeded--;
	pthread_mutex_unlock(&energyLocker2);
}
void post()
{
    pthread_mutex_lock(&energyLocker2);
	energyNeeded++;
	pthread_mutex_unlock(&energyLocker2);
}
//Semaphore functions --END

//Custom sort comparator
bool cmp(vector<int> &a , vector<int> &b)
{
    return a[3] > b[3];
}

//Site
void* site(void* siteNumber)
{
 
	int siteNum =  *((int*)siteNumber);
	int left = siteNum - 1;
	int right = siteNum;
	while(1)
	{
		int hleft,oleft;
    if(done==true) break;

    //Locking phase -- START
    wait();
		if( siteNum == 0 )
		{
			pthread_mutex_lock(&siteMutex[right]);
		}
		else if( siteNum == nSites - 1 )
		{
			pthread_mutex_lock(&siteMutex[left]);
		}
		else
		{
			pthread_mutex_lock(&siteMutex[left]);
			pthread_mutex_lock(&siteMutex[right]);
		}
		pthread_mutex_lock(&hoLocker);
		//Locking phase -- END
	
		//Unlocking phase INCASE of H2O depletion
		if(hydrogenThreads.size()<=0 || oxygenThreads.size()<=0){
			if( siteNum == 0 )
			{
				pthread_mutex_unlock(&siteMutex[right]);
			}
			else if( siteNum == nSites - 1 )
			{
				pthread_mutex_unlock(&siteMutex[left]);
			}
			else
			{
				pthread_mutex_unlock(&siteMutex[left]);
				pthread_mutex_unlock(&siteMutex[right]);
			}
			done=true;
			pthread_mutex_unlock(&hoLocker);
			
		 	break;
		}

		pthread_t hThread1 = hydrogenThreads.front();
		hydrogenThreads.pop();
		pthread_t hThread2 = hydrogenThreads.front();
		hydrogenThreads.pop();
		pthread_t oThread1 = oxygenThreads.front();
		oxygenThreads.pop();
		hleft = hydrogenThreads.size();
		oleft = oxygenThreads.size();

		pthread_mutex_unlock(&hoLocker); //Unlocking H2O
		
		pthread_create(&hThread1,NULL,hydrogen,NULL);
		pthread_create(&hThread2,NULL,hydrogen,NULL);
		pthread_create(&oThread1,NULL,oxygen,NULL);
		
		pthread_mutex_lock(&energyLocker);
		currentEnergy++;

    //Save details for output
    vector<int> temp = { siteNum , currentEnergy , hleft , oleft };
		outputFormatted.push_back(temp);

		pthread_mutex_unlock(&energyLocker);
		pthread_join(hThread1,NULL);
		pthread_join(hThread2,NULL);
		pthread_join(oThread1,NULL);
		pthread_mutex_lock(&energyLocker);
		currentEnergy--;		
		pthread_mutex_unlock(&energyLocker);

		//Unlocking phase -- START
		if( siteNum == 0 )
		{
			pthread_mutex_unlock(&siteMutex[right]);
		}
		else if( siteNum == nSites - 1 )
		{
			pthread_mutex_unlock(&siteMutex[left]);
		}
		else
		{
			pthread_mutex_unlock(&siteMutex[left]);
			pthread_mutex_unlock(&siteMutex[right]);
		}
    post();
		//Unlocking phase -- END
	}
	return NULL;
}

int main( int argc , char* argv[] )
{
	if( argc < 5 )
	{
		cout<<"Details missing from input!";
		return 0;
	}

	hAtoms 	  = atoi(argv[1]);
	oAtoms    = atoi(argv[2]);
	nSites    = atoi(argv[3]);
	threshold = atoi(argv[4]);
	
	energyNeeded = threshold;

	int hydroPairs = hAtoms/2;
	int h2o = min( hydroPairs , oAtoms );
	int maxReactionsAllowed = min( h2o,nSites );

	vector<int> nums( nSites );

	for( int i = 0;i < maxReactionsAllowed; i++ )
	{
		pthread_t newSite;
		sitesThreads.push_back(newSite);
	}

	for( int i = 0; i < hAtoms ; i++ )
	{
		pthread_t newHAtom;
		hydrogenThreads.push(newHAtom);
	}

	for( int i = 0; i < oAtoms ; i++ )
	{
		pthread_t newOAtom;
		oxygenThreads.push(newOAtom);
	}
	
	for( int i = 0; i < sitesThreads.size() - 1 ; i++)
	{
		pthread_mutex_t newSiteMutex;
		if (pthread_mutex_init(&newSiteMutex, 0) != 0) {
	        printf("\n mutex init has failed\n");
	        return 1;
	    }else
	    {
	    	siteMutex.push_back(newSiteMutex);
	    }
	}	
	for( int i = 0; i < sitesThreads.size() ; i++)
	{
		nums[i] = i;
		pthread_create(&sitesThreads[i],NULL,site,&nums[i]);
	}	
	for( int i = 0; i < sitesThreads.size() ; i++)
	{
		pthread_join(sitesThreads[i],NULL);
	}	
	for( int i = 0; i < siteMutex.size() ; i++)
	{
		pthread_mutex_destroy(&siteMutex[i]);
	}
	pthread_mutex_destroy(&energyLocker);
    sort(outputFormatted.begin(),outputFormatted.end(),cmp);
    for(int i=0;i<outputFormatted.size();i++)
    {
        cout<<"Reaction number:"<<to_string(i+1)<<endl;
        cout<<"Reaction completed at Site: "<<outputFormatted[i][0]<<"\nCurrent Chip Energy: "<<outputFormatted[i][1]<<
				" Remaining Hydrogen: "<<outputFormatted[i][2]<<" Remaining Oxygen: "<<outputFormatted[i][3]<<
				"\n----------------------------------------------------------------------------\n"<<
				"\n----------------------------------------------------------------------------\n";
    }
	return 0;
}