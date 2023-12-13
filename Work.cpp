#include<iostream>
#include<vector>
#include <fstream>
#include <time.h>
#include <string>
#include<ctime>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <iterator>
//#include <sys/wait.h>
using namespace std ;

class M1{
	vector<int> buffer ;
	string file, Ofile ;
	long double turnaroundtime ;
	public:
		bool Read(){
			buffer.clear() ;
			fstream infile ;
			infile.open(file.c_str(), ios::in) ;
			if( !infile.is_open()){
				cout << "### " << file << " does not exist! ### " << endl << endl ;
            	return false ;
			} // if
			else{
				int num = 0;
				while( infile >> num ){
					buffer.push_back(num) ;
				} // while
			} // else
			
			infile.close() ;
			return true ;
		} // Read()
		
		void Write(){
			fstream outfile ;
			outfile.open(Ofile.c_str(), ios::out) ;
			
			outfile << "Sort : \n" ;
			for( int i = 0 ; i < buffer.size() ; i++ ){
				outfile << buffer[i] << "\n" ;
			} // for
			
			time_t tmNow = time(0);
   			char *dt = ctime(&tmNow);
		    outfile << "CPU Time : " << turnaroundtime << "\n" ;
		    outfile << "Output Time :"<< dt ;
			outfile.close() ;
		} // Write()
		
		void bubblesort(){
			int n = buffer.size() ;
			for( int pass = 1 ; pass < n ; ++pass ){ // 每一次都會將最大值放在最後 
				for( int index = 0 ; index < n - pass ; index++ ){ //最後面不用檢查，因為一定是最大，所以每一次檢查的次數減少。 
					if( buffer[index] > buffer[index+1] )
						swap( index, index+1 );
				} // for
				
			} // for
			
			
		} // end of bubblesort
		
		void swap( int num1, int num2 ){
			int temp ; 
			temp = buffer[num1] ;
			buffer[num1] = buffer[num2] ;
			buffer[num2] = temp ;
		} // end of swap
		
		void Run( string name ){
			//file = "input_50w.txt" ;
			//Ofile = "input_50w_output1.txt" ;
			
			file = name + "_input.txt" ;
			Ofile = name + "_output1.txt" ;
			
			Read() ;
			clock_t start, end ;
			start = clock() ;
			bubblesort() ;
			end = clock();
			turnaroundtime = 1000*((long double) (end - start)) / CLOCKS_PER_SEC;
			Write() ; 
		} //Run()
};

class M2{
	vector<int> buffer ;
	string file, Ofile ;
	long double turnaroundtime ;
	public:
		void Initial(){
			buffer.push_back(9) ;
			buffer.push_back(8) ;
			buffer.push_back(7) ;
			buffer.push_back(6) ;
			
			buffer.push_back(5) ;
			buffer.push_back(4) ;
			buffer.push_back(3) ;
			buffer.push_back(2) ;
			
			buffer.push_back(1) ;
			buffer.push_back(0) ;
		} // Initial
		
		bool Read(){
			buffer.clear() ;
			fstream infile ;
			infile.open(file.c_str(), ios::in) ;
			if( !infile.is_open()){
				cout << "### " << file << " does not exist! ### " << endl << endl ;
            	return false ;
			} // if
			else{
				int num = 0;
				while( infile >> num ){
					buffer.push_back(num) ;
				} // while
			} // else
			
			infile.close() ;
			return true ;
		} // Read()
		
		void Write(){
			fstream outfile ;
			outfile.open(Ofile.c_str(), ios::out) ;
			
			outfile << "Sort : \n" ;
			for( int i = 0 ; i < buffer.size() ; i++ ){
				outfile << buffer[i] << "\n" ;
			} // for
			
			time_t tmNow = time(0);
   			char *dt = ctime(&tmNow);
		    outfile << "CPU Time : " << turnaroundtime << "\n" ;
		    outfile << "Output Time :"<< dt ;
			outfile.close() ;
		} // Write()
		
		void mergesort( vector<int>&buffer, int first, int last ){
			if( first < last ){
				int mid = ( first + last ) / 2 ;
				mergesort( buffer, first, mid ) ;
				mergesort( buffer, mid+1, last ) ;
				merge( buffer, first, mid, last) ;
			}
			
		} // end of mergesort
		
		void merge( vector<int>&buffer, int first, int mid, int last ){
			int i, j, k;
			int n1 = mid - first + 1 ;
			int n2 = last - mid ;
			vector<int> L(buffer.begin()+first, buffer.begin()+mid+1), R(buffer.begin()+mid+1, buffer.begin()+last+1) ;
			
			for ( i = 0; i < n1; i++ ){
				L[i] = buffer[first + i]; 
			} // for
			   
			for ( j = 0; j < n2; j++ ){
				R[j] = buffer[mid + 1 + j] ;
			} // for				
				
			i = 0 ;
			j = 0 ;
			k = first ;
			
			while (i < n1 && j < n2) {
			    if (L[i] <= R[j])
			        buffer[k++] = L[i++] ;
			    else
			        buffer[k++] = R[j++] ;
			} // while
			
			while ( i < n1 ){
				buffer[k++] = L[i++] ;
			} // while
			    
			    
			while ( j < n2 ){
				buffer[k++] = R[j++] ;
			} // while
			
		} // merge
		
		void Run( string name, int numOfk ){
			file = name + "_input.txt" ;
			Ofile = name + "_output2.txt" ;
			
			Read() ;
			clock_t start, end ;
			int k = numOfk ;
			//Initial() ;
			
			int np = buffer.size()/16384 + 1 ;
			int pipe_arr[np][2] ;
			for (int i = 0 ; i < np ; i++) {
		        if (pipe(pipe_arr[i]) < 0) {
		            perror("pipe error");
		            exit(EXIT_FAILURE);
		        } // if
		    } // for
		    
			start = clock() ;
			pid_t pid ;
			int status ;
			pid = fork() ;
			if( pid == 0 ){
				for( int i = 0 ; i < k ; i++ ){
					int first = i * (buffer.size()/k) ;
			    	int last ;
				    if( i == k-1 ) last = buffer.size() ;
				    else last = first + (buffer.size()/k) ;
			 		//bubble sort
			 		for( int pass = first ; pass < last ; pass++ ){ 
			 			//cout << pass << endl;
						for( int index = first ; index < last-1 ; index++ ){ 
							if( buffer[index] > buffer[index+1] ){
								//Swap
								int temp ;
								temp = buffer[index] ;
								buffer[index] = buffer[index+1] ;
								buffer[index+1] = temp ;
							} // if
								
						} // for
						
					} // for
				} // for
					
				cout << "end bubblesort-------------------------" << endl;
				
				
				mergesort( buffer, 0, buffer.size()-1) ;
				
				for( int j = 0 ; j < buffer.size() ; j++ ){
					int numberp = j/16384 ;
					//cout << j << " " << buffer[j] << endl;
					close( pipe_arr[numberp][0]) ;
					write( pipe_arr[numberp][1], &buffer[j], sizeof(buffer[j]) ) ;
					//close( mpipe_arr[numberp][1]) ;
				} // for
				
				cout << "end mergesort-------------------------" << endl;
				exit(0) ;
			} // if
			pid = wait(&status);
			

		    for(int j = 0 ; j < buffer.size() ; j++){
		    	int numberp = j/16384 ;
		    	close(pipe_arr[numberp][1]);
		        read(pipe_arr[numberp][0], &buffer[j], sizeof(buffer[j])) ;
		        //cout << j << " " << buffer[j] << endl;
		        //cout << numberp << " " << j << " " << buffer3[j] << endl;
		        
		    } // for
			
			
			end = clock();
			turnaroundtime = 1000*((long double) (end - start)) / CLOCKS_PER_SEC;
			Write() ; 
			
		} //Run()
};


vector<int> ans ;
class M3 {
	vector<int> buffer3 ;
	vector<int> tmp ;
	string file, Ofile ;
	long double turnaroundtime ;
	int number, k ;
	//static vector<int> ans ;
	
	public:
	    void Initial(){
			buffer3.push_back(9) ;
			buffer3.push_back(8) ;
			buffer3.push_back(7) ;
			buffer3.push_back(6) ;
			
			buffer3.push_back(5) ;
			buffer3.push_back(4) ;
			buffer3.push_back(3) ;
			buffer3.push_back(2) ;
			
			buffer3.push_back(1) ;
			buffer3.push_back(0) ;
		} // Initial
		
		bool Read(){
			buffer3.clear() ;
			ans.clear() ;
			fstream infile ;
			infile.open(file.c_str(), ios::in) ;
			if( !infile.is_open()){
				cout << "### " << file << " does not exist! ### " << endl << endl ;
            	return false ;
			} // if
			else{
				int num = 0;
				while( infile >> num ){
					buffer3.push_back(num) ;
				} // while
			} // else
			
			infile.close() ;
			return true ;
		} // Read()
		
		void Write( string name ){
			fstream outfile ;
			Ofile = name + "_output3.txt" ;
			outfile.open(Ofile.c_str(), ios::out) ;
			
			outfile << "Sort : \n" ;
			//cout << ans.size() << endl;
			for( int i = 0 ; i < buffer3.size() ; i++ ){
				cout << buffer3[i]  << endl ;
				outfile << buffer3[i] << "\n" ;
			} // for
			
			time_t tmNow = time(0);
   			char *dt = ctime(&tmNow);
		    outfile << "CPU Time : " << turnaroundtime << "\n" ;
		    outfile << "Output Time :"<< dt ;
		    cout << "CPU Time : " << turnaroundtime << "\n" ;
		    cout << "Output Time :"<< dt ;
			outfile.close() ;
		} // Write()
		
		void Run( string name, int numOfk ){
			//file = "input_1w.txt" ;
			//Ofile = "input_1w_output3.txt" ;
			
			file = name + "_input.txt" ;
			//Ofile = name + "_output3.txt" ;
			
			Read() ;
			cout << buffer3.size() << endl;
			clock_t start, end ;
			k = numOfk ;
			//cout << "Please enter the number of k:" ;
			//cin >> k ;
			//Initial() ; 
			
			start = clock() ;
			
			int np = buffer3.size()/16384 + 1 ;
			int pipe_arr[np][2] ;
			int result[buffer3.size()] ; 
			int status ;
			pid_t pid[k] ;
			int nump = 0 ;
			//創建pipe 
			for (int i = 0 ; i < np ; i++) {
		        if (pipe(pipe_arr[i]) < 0) {
		            perror("pipe error");
		            exit(EXIT_FAILURE);
		        } // if
		    } // for
			
			for( int i = 0 ; i < k ; i++ ){
				pid[i] = fork() ;
				
			 	if( pid[i] == 0 ){
			 		
			 		int first = i * (buffer3.size()/k) ;
			    	int last ;
				    if( i == k-1 ) last = buffer3.size() ;
				    else last = first + (buffer3.size()/k) ;
			 		//bubble sort
			 		for( int pass = first ; pass < last ; pass++ ){ 
			 			//cout << pass << endl;
						for( int index = first ; index < last-1 ; index++ ){ 
							if( buffer3[index] > buffer3[index+1] ){
								//Swap
								int temp ;
								temp = buffer3[index] ;
								buffer3[index] = buffer3[index+1] ;
								buffer3[index+1] = temp ;
							} // if
								
						} // for
						
					} // for
					
					
					bool keep = true ;
					for( int j = first ; j < last ; j++ ){
						//cout << j << endl;
						int numberp = j/16384 ;
						close(pipe_arr[numberp][0]) ;
						//cout << numberp << " " << j << endl;
						write( pipe_arr[numberp][1], &buffer3[j], sizeof(buffer3[j]) ) ;
						//close(pipe_arr[numberp][1]) ;
					} // for
					
			 		exit(0) ;
			 		
				} // if  
			    
			    pid[i] = wait(&status);
			    
			} // for
	        
	        for( int i = 0 ; i < k ; i++ ){
	        	pid[i] = wait(&status);
			} // for
	        
			cout << "end bubblesort--------------------------" << endl;
			
			for(int i = 0 ; i < k ; i++){
			    int first = i * (buffer3.size() / k);
			    int last ;
			    if( i == k-1 ) last = buffer3.size() ;
			    else last = first + (buffer3.size()/k) ;
			
			    for(int j = first; j < last; j++){
			    	int numberp = j/16384 ;
			    	//cout << numberp << " " << j << " " << buffer3[j] << endl ;
			    	close(pipe_arr[numberp][1]);
			        read(pipe_arr[numberp][0], &buffer3[j], sizeof(buffer3[j])) ;
			        //close(pipe_arr[numberp][0]);
			    } // for
			
			    
			} // for
			 
			
			k = k - 1 ;
			np = buffer3.size()/16384 + 1 ;
			int mpipe_arr[np][2] ;
			int mresult[buffer3.size()] ; 
			//int status ;
			pid_t mpid[k] ;
			 
			//創建pipe 
			for (int i = 0; i < np ; i++) {
		        if (pipe(mpipe_arr[i]) < 0) {
		            perror("pipe error") ;
		            exit(1) ;
		        } // if
		    } // for
			
			for( int i = 0 ; i < k ; i++ ){
				mpid[i] = fork() ;
				
			 	if( mpid[i] == 0 ){
			 		int first = i * (buffer3.size()/k) ;
			    	int last ;
			    	if( i == k-1 ) last = buffer3.size() ;
			    	else last = first + (buffer3.size()/k) ;
					
					if (last > buffer3.size()) {
			            last = buffer3.size();
			        } // if 
					
			 		mergesort( buffer3, first, last-1 ) ;
					
					for( int j = first ; j < last ; j++ ){
						int numberp = j/16384 ;
						close( mpipe_arr[numberp][0]) ;
						//
						write( mpipe_arr[numberp][1], &buffer3[j], sizeof(buffer3[j]) ) ;
						//close( mpipe_arr[numberp][1]) ;
					} // for 
					
					
			 		exit(0) ;
			 		
				} // if
			    mpid[i] = wait(&status);
			    
			} // for
			
			for( int i = 0 ; i < k ; i++ ){
	        	mpid[i] = wait(&status);
			} // for
			
			for(int i = 0; i < k; i++){
			    int first = i * (buffer3.size() / k);
			    int last ;
			    if( i == k-1 ) last = buffer3.size() ;
			    else last = first + (buffer3.size()/k) ;
			
			    for(int j = first ; j < last ; j++){
			    	int numberp = j/16384 ;
			    	close(mpipe_arr[numberp][1]);
			        read(mpipe_arr[numberp][0], &buffer3[j], sizeof(buffer3[j])) ;
					//cout << numberp << " " << j << " " << buffer3[j] << endl ; 
			        //cout << numberp << " " << j << " " << buffer3[j] << endl;
			        
			    } // for
			
			    
			} // for
			
			
			mergesort( buffer3, 0, buffer3.size()-1 ) ;
			/*
			for( int i = 0 ; i < buffer3.size() ; i++ ){
				cout << i << " " << buffer3[i] << endl;
			}
			*/
			end = clock() ;
			turnaroundtime = 1000*((long double) (end - start)) / CLOCKS_PER_SEC;
			Write(name) ; 
			
		} //Run()
		
	    void Merge( vector<int>&buffer3, int first, int mid, int last ){
			int i, j, k;
			int n1 = mid - first + 1 ;
			int n2 = last - mid ;
			vector<int> L(buffer3.begin()+first, buffer3.begin()+mid+1), R(buffer3.begin()+mid+1, buffer3.begin()+last+1) ;
			
			for ( i = 0; i < n1; i++ ){
				L[i] = buffer3[first + i]; 
			} // for
			   
			for ( j = 0; j < n2; j++ ){
				R[j] = buffer3[mid + 1 + j] ;
			} // for				
				
			i = 0 ;
			j = 0 ;
			k = first ;
			
			while (i < n1 && j < n2) {
			    if (L[i] <= R[j])
			        buffer3[k++] = L[i++] ;
			    else
			        buffer3[k++] = R[j++] ;
			} // while
			
			while ( i < n1 ){
				buffer3[k++] = L[i++] ;
			} // while
			    
			    
			while ( j < n2 ){
				buffer3[k++] = R[j++] ;
			} // while
			
		} // merge
		
		void mergesort( vector<int> &buffer, int first, int last ){
			if( first < last ){
				int mid = (first+last)/ 2 ;
		        mergesort( buffer, first, mid) ;
		        mergesort( buffer, mid + 1, last) ;
		        Merge( buffer, first, mid, last) ;
			} // if 
			
		} // end of mergesort
		
};

vector<int> buffer ;
int number, k ;
void swap( int num1, int num2 ){
	int temp ; 
	temp = buffer[num1] ;
	buffer[num1] = buffer[num2] ;
	buffer[num2] = temp ;
} // end of swap

void bubblesort( int first, int last ){
	for( int pass = first ; pass < last ; pass++ ){ 
		for( int index = pass ; index < last-1 ; index++ ){ 
			if( buffer[index] > buffer[index+1] )
				swap( index, index+1 );
		} // for
		
	} // for
	
} // end of bubblesort

void* bubblesort(void* arg){
    int NumberOfthread = number++ ; //第幾個thread 
    
    int first = NumberOfthread * (buffer.size()/k) ;
    int last = first + (buffer.size()/k) ;
 	
    bubblesort( first, last ) ;
	return arg ;
} // end of bubblesort

void merge( vector<int>&buffer, int first, int mid, int last ){
	int i, j, k;
	int n1 = mid - first + 1 ;
	int n2 = last - mid ;
	vector<int> L(buffer.begin()+first, buffer.begin()+mid+1), R(buffer.begin()+mid+1, buffer.begin()+last+1) ;
	
	for ( i = 0; i < n1; i++ ){
		L[i] = buffer[first + i]; 
	} // for
	   
	for ( j = 0; j < n2; j++ ){
		R[j] = buffer[mid + 1 + j] ;
	} // for				
		
	i = 0 ;
	j = 0 ;
	k = first ;
	
	while (i < n1 && j < n2) {
	    if (L[i] <= R[j])
	        buffer[k++] = L[i++] ;
	    else
	        buffer[k++] = R[j++] ;
	} // while
	
	while ( i < n1 ){
		buffer[k++] = L[i++] ;
	} // while
	    
	    
	while ( j < n2 ){
		buffer[k++] = R[j++] ;
	} // while
	
} // merge

void mergesort( vector<int>&buffer, int first, int last ){
	if( first < last ){
		int mid = ( first + last ) / 2 ;
		mergesort( buffer, first, mid ) ;
		mergesort( buffer, mid+1, last ) ;
		merge( buffer, first, mid, last) ;
	} // if 
	
} // end of mergesort

void Tmergesort( vector<int>& buffer, int first, int last, int NumOfThreads ){
	if ( NumOfThreads <= 1 ) 
        mergesort( buffer, first, last ) ;
	else {
        int mid = ( first + last ) / 2;
        thread LThread( Tmergesort, ref(buffer), first, mid, NumOfThreads / 2 ) ;
        thread RThread( Tmergesort, ref(buffer), mid + 1, last, NumOfThreads - NumOfThreads / 2 ) ;
        LThread.join() ;
        RThread.join() ;
        merge( buffer, first, mid, last ) ;
    } // else
    
} // Tmergesort()

class M4{
	
	string file, Ofile ;
	double turnaroundtime ;
	
	public:
		void Initial(){
			buffer.push_back(9) ;
			buffer.push_back(8) ;
			buffer.push_back(7) ;
			buffer.push_back(6) ;
			
			buffer.push_back(5) ;
			buffer.push_back(4) ;
			buffer.push_back(3) ;
			buffer.push_back(2) ;
			
			buffer.push_back(1) ;
			buffer.push_back(0) ;
		} // Initial
		
		bool Read(){
			buffer.clear() ;
			fstream infile ;
			infile.open(file.c_str(), ios::in) ;
			if( !infile.is_open()){
				cout << "### " << file << " does not exist! ### " << endl << endl ;
            	return false ;
			} // if
			else{
				int num = 0;
				while( infile >> num ){
					buffer.push_back(num) ;
				} // while
			} // else
			
			infile.close() ;
			return true ;
		} // Read()
		
		void Write( string name ){
			fstream outfile ;
			Ofile = name + "_output4.txt" ;
			
			outfile.open(Ofile.c_str(), ios::out) ;
			
			outfile << "Sort : \n" ;
			for( int i = 0 ; i < buffer.size() ; i++ ){
				outfile << buffer[i] << "\n" ;
			} // for
			
			time_t tmNow = time(0);
   			char *dt = ctime(&tmNow);
		    outfile << "CPU Time : " << turnaroundtime << "\n" ;
		    outfile << "Output Time :"<< dt ;
		    cout << "CPU Time : " << turnaroundtime << "\n" ;
		    cout << "Output Time :"<< dt ;
			outfile.close() ;
		} // Write()
		
		void Run( string name, int numOfk ){
			//file = "input_100w.txt" ;
			//Ofile = "input_100w_output4.txt" ;
			
			file = name + "_input.txt"  ;
		
			Read() ;
			//cout << "--------------------------" << endl;
			time_t start, end ;
			//cout << "Please enter the number of k:" ;
			//cin >> k ;
			k = numOfk ;
			start = clock();
			
			number = 0 ;
			pthread_t thread[k] ;
			
			for(int i = 0; i < k ; i++){ //產生k個thread做bubbleosrt
				pthread_create(&thread[i], NULL, bubblesort, (void*)NULL);                
			} // for
        	
        	for( int i = 0 ; i < k ; i++ ){
        		pthread_join(thread[i],NULL) ;
			} // for
			//cout << "end bubblesort--------------------------" << endl;
			
			Tmergesort( buffer, 0, buffer.size() - 1, k-1 ) ;
			//cout << "end Mergesort--------------------------" << endl;
			end = clock();
			turnaroundtime = 1000*((long double) (end - start)) / CLOCKS_PER_SEC;
			Write(name) ; 
			
		} //Run()
		
		
		
		
};

int main(){

	int mode = -1 ;
	bool keep = true ;
	
	while( keep ){
		string name ;
		cout << "please enter the filename:\n" ;
		cin >> name ;
		
		int k = 0 ;
		cout << "please enter the k:\n" ;
		cin >> k ;
		
		cout << "please enter the mode:\n" ;
		cin >> mode ;
		
		if( mode == 0 ) keep = false ;
		else{
			
			if( mode == 1 ){
				M1 m1 ;
				m1.Run(name) ;
				cout << "M1 Complete!" << endl;	
			} // if
			else if( mode == 2 ){
				M2 m2 ;
				m2.Run(name,k) ;
				cout << "M2 Complete!" << endl;
			} // else if
			else if( mode == 3 ){
				M3 m3 ;
				m3.Run(name,k) ;
				cout << "M3 Complete!" << endl;
			} // else if
			else if( mode == 4 ){
				M4 m4 ;
				m4.Run(name,k) ;
				cout << "M4 Complete!" << endl;
			} // else if
			else if( mode == 5 ){
				M1 m1 ;
				m1.Run(name) ;
				cout << "M1 Complete!" << endl;
				
				M2 m2 ;
				m2.Run(name,k) ;
				cout << "M2 Complete!" << endl;
				
				M3 m3 ;
				m3.Run(name,k) ;
				cout << "M3 Complete!" << endl;
				
				M4 m4 ;
				m4.Run(name,k) ;
				cout << "M4 Complete!" << endl;
				
			} // else if
			else cout << "mode doesn't exsits" << endl ;
			
		} // else
		
	} // while
	
	cout << "END" << endl;
	 
} 
