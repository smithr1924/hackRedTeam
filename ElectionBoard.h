#ifndef ELECBD_
#define ELECBD_
#include <cmath>
#include <algorithm>
#include <string>
#include "primeGenerator.h"

using namespace std;

class ElectionBoard
{
	public:
	int modInverse(int n, int p) {
		n =n%p;
		for(int x = 1; x < p; x++) {
			if(n*x % p == 1) return x;
		}
		return 0;
	}
	
	private:
		int e;       // These three variables are only used for generating blind signatures
		int d;       // they an RSA public encryption key, private decryption key, and Euler's totient(N),
		int totientN;// respectively.
		int u;
		int lambda;
		int p;
		int q;
		int N;
		int g;
		int r;
			
	void compute_pq(){
		PrimeGenerator primer;
		p = primer.getPrime();
		q = primer.getPrime();
		while (gcd(p*q, (p-1)*(q-1)) != 1){
			p = primer.getPrime();
			q = primer.getPrime();
		}
	}	
	
	void compute_lambda(){
		lambda = lcm(p-1,q-1);
	}
	
	int modpow(long long base, int exp, int modulus) {
	  base %= modulus;
	  int result = 1;
	  while (exp > 0) {
		if (exp & 1) result = (result * base) % modulus;
		base = (base * base) % modulus;
		exp >>= 1;
	  }
	  return result;
	}
	
	void compute_eANDd(){
		e = rand() % totientN +1;
		while (gcd(e, totientN) != 1){
			e = rand() % totientN +1;
		}
		
		d = modInverse(e,totientN);
	}
	
	void compute_u(){
		int u_  = modpow(g, lambda, (N*N));
		int Lu = (u_ - 1)/N;
		u = modInverse(Lu, N);
	}
	
	void compute_g(){
		/*g is in Z(N^2)
		we chose g as N+1 for efficiency, ease of computation, and to help
		keep our values from becoming too large and overflowing*/
		g = N+1;
	}
	
	void compute_r(){ /*r is in Z(N)*/
		r = rand() % N +1;
		while (gcd(r, N) != 1){
			r = rand() % N+1;
		}
	}
		
	public:
	
	//Constructor:
	ElectionBoard() {
		compute_pq();
		N = p*q; 
		totientN = (p-1)*(q-1);
		compute_eANDd();
		compute_g(); 
		compute_r(); 
		compute_lambda();
		compute_u();
	}
	
	//Getters:
	/////////////////////////
	int get_N(){
		return N;
	}
	
	int get_r(){
		compute_r();
		return r;
	}
	
	int get_g(){
		return g;
	}
	
		int get_e(){
		return e;
	}
	//////////////////////////
	
	/*function to sign blind votes*/
	long long sign(long long v){
		return modpow(v, d,N);
	}
	
	/*gcd function which is used as an auxillary for lcm*/
	int gcd(int a, int b)
	{
		for (;;)
		{
			if (a == 0) return b;
			b %= a;
			if (b == 0) return a;
			a %= b;
		}
	}

	/*least common multiple function*/
	int lcm(int a, int b)
	{
		int temp = gcd(a, b);

		return temp ? (a / temp * b) : 0; 
	}
	
	int findMax(vector<int> sums){
		int maxi = 0;
		for (int i = 0; i < sums.size(); i++){
			if (sums[i] > maxi){
				maxi = sums[i];
			}
		}
		return maxi;
	}
	
	/*decryption function*/
	int decrypt(unsigned long long c){
		c = c % (N*N);
		long temp = modpow(c,lambda,(N*N)); 
		int Ltemp = (temp -1.0)/N; 
		int plaintext = (Ltemp * u) % N;
		return plaintext;
	}

	/*EM decrypts votes and anounces election winner here*/
	void decryptResults(vector<long long> & VoterSums, vector<long long> & CandidateSums, string* candidates){
		for(int i = 0; i<VoterSums.size(); i++){
			/*decrypt voter sums and confirm each one adds up to 1*/
			int plainVote = decrypt(VoterSums[i]);
			if(plainVote != 1){
				cout << plainVote << endl;
				cout<< "ERROR: INVALID VOTE,RE-VOTE NEEDED"<<endl;
			}
		}

		vector<int> plainSums;
		for(int i = 0; i<CandidateSums.size(); i++){
			/*decrypt candidate sums and announce election winner*/
			int plainSum = decrypt(CandidateSums[i]);
			plainSums.push_back(plainSum);
		}
		int max = findMax(plainSums);
		vector<string> winners;
		
		for(int i = 0; i<plainSums.size(); i++){
			if(plainSums[i] == max){
				winners.push_back(candidates[i]);
			}
		}

		if(winners.size() == 1){
			cout<<"Election winner, with "<<max<<" vote/s, is: "<<winners[0]<<endl;
		}
		else{
			cout<<"Election is a "<<winners.size()<<" way tie with winners: "<<endl;
			for(int i = 0; i<winners.size(); i++){
				if(i == winners.size()-1){
					cout<<"and "<<winners[i]<<endl;
				}
				else{
					cout<<winners[i]<<endl;
				}
			}
		}
	}
	
};
#endif