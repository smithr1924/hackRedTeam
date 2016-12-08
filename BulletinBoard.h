#ifndef BULLBD_
#define BULLBD_

#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <stdlib.h>

using namespace std;

class BulletinBoard{
	private:
	vector<vector<long long> > board; //encrypted board
	int N; //large p*q
	int e; //random number to be generated during ZKP
	int u; //number sent by voter composed of (g^r * s^N) mod N^2 
	int g; //public encryption number
	
	long long modpow(long long base, int exp, int modulus) {
	  base %= modulus;
	  int result = 1;
	  while (exp > 0) {
		if (exp & 1) result = (result * base) % modulus;
		base = (base * base) % modulus;
		exp >>= 1;
	  }
	  return result;
	}

	public:
	
		/* 
			Constructor of the board.
		*/
		BulletinBoard (int prime, int numCands){
			N = prime;
		}
		
		/*
			Adds an encrypted vote to the board.
		*/
		void addVote (vector<long long> encryptedVote){
			board.push_back(encryptedVote);
		}
		
		/*
			Board is sent variable u and large prime N.
			Returns random number e.
		*/
		int sendToBB(int u_, int g_){
			u = u_;
			g = g_;
			e=rand() % N-1;
			return e;
		}
		
	    /*
			BB confirms signature by checking that signed vote (m^d) raised to 
			the encryption key e is equal to m
	    */
		bool signatureConfirm(long long signy , long long encrypty, int e, int N){
			return ((encrypty%N) == modpow(signy,e,N));
		}
		
		/*
			Board is sent the response from the voter and verifies the voter's vote.
		*/
		bool response(int v, int w, int c){
			int check = pow(g, v) * pow(c, e) * pow(w, N);
			return check == u;
		}
		
		/*
			Finds encrypted sums of the Bulentin Board.  This is the counting authority.
		*/
		void computeSums(int n, vector<long long> &CandidateSums, vector<long long> &VoterSums){
			/*compute voter sums*/
			for(int i = 0; i<board.size(); i++){
				for(int j = 0; j<n; j++){
					VoterSums[i] =  (VoterSums[i] * board[i][j]) % (N*N);
				}
			}
			/*compute candidate sums*/
			for(int i = 0; i<n; i++){
				for(int j = 0; j<board.size(); j++){
					CandidateSums[i] = (CandidateSums[i] * board[j][i]) % (N*N);
				}
			}
		}
		
};

#endif