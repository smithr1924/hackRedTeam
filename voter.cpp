/*
** 
**	Alex Vargas
**	Madrone Goldman
**
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <set>
#include <cmath>

#include "primeGenerator.h"
#include "BulletinBoard.h"
#include "ElectionBoard.h"
#include <stdlib.h>

using namespace std;

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

int modInverse(int n, int p) {
	n =n%p;
	for(int x = 1; x < p; x++) {
		if(n*x % p == 1) return x;
	}
	return 0;
}

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

long long encrypt(int N, int r, int g, int m){
	long long ciphertext =  (modpow(g,m,N*N) * modpow(r,N,N*N)) % (N*N);
	return ciphertext;
}

int main(int argc, char* argv[]){
	/*
		Candidate file
		-First line is single number n of candidates
		-Next n lines are those candidates
	*/
	ifstream cands;
	cands.open("candidates.txt");
	int n;
	cands >> n;
	string candidates[n];
	string temp;
	getline(cands, temp);
	for (int i =0; i < n; i++){
		getline(cands, candidates[i]);
		cout << candidates[i] << endl;
	}
	cands.close();
	
	
	ElectionBoard EM;
	BulletinBoard BB(EM.get_N(), n);
	
	// receive voter input 
	set<string> voters;
	while (true){
		cout << "Please enter your name: " << endl;
		getline(cin, temp);
		cout<<endl;
		if (temp == "-1"){
			break;
		}
		if (voters.find(temp) == voters.end()){
			voters.insert(temp);
		}else{
			cout << "Hey nerd, you already voted!" << endl;
			continue;
		}
		
		//input vote
		int plainVote;
		while (true){
			cout << "Enter the number of your preferred candidate: " << endl;
			for (int i=1; i <= n; i++){
				cout << i << ". " << candidates[i-1] << endl;
			}
			getline(cin, temp);
			plainVote = atoi(temp.c_str());
			cout<<endl;
			if (plainVote < 1 || plainVote > n){
				cout << "Invalid vote. Try again." << endl;
				continue;
			}
			break;
		}
		
		
		vector<int> plainVect;//stores plaintext votes
		for (int i = 0; i < n; i++){
			if (i == plainVote-1){
				plainVect.push_back(1);
			}else{
				plainVect.push_back(0);
			}
		}

		vector<long long> secretVect;//stores encrypted votes
		
		/*voters register with EM, receive public encryption key and encrypt their votes*/	
		for (int i =0; i < plainVect.size(); i++){
			secretVect.push_back(encrypt(EM.get_N(), EM.get_r(), EM.get_g(), plainVect[i]));
		}	
		
		
		
	//BLIND SIGNATURE:
		vector<long long> signedVect;
		
		 //calculate r (random and coprime to N)
		 int r = EM.get_r();
		 bool bad_votes = false;
		 
		 for(int i = 0; i < secretVect.size(); i++){
			
			// voter blinds encrypted vote
			 long long blinded = (secretVect[i] * modpow(r,EM.get_e(),EM.get_N() )) % EM.get_N() ;
			 signedVect.push_back( blinded );
			
			//voter sends blinded vote to EM for signing
			signedVect[i] = EM.sign(signedVect[i]); 

			//voter unblinds signed vote
			signedVect[i] = (signedVect[i] * EM.modInverse(r,EM.get_N())) % EM.get_N() ; 
			
			//voter sends unblinded signed vote to BB along with unsigned encrypted vote 
			if(!(BB.signatureConfirm(signedVect[i], secretVect[i], EM.get_e(),EM.get_N() ))){
				cout<<"ERROR: Bulletin Board cannot confirm signature. Continuing on to next voter\n";
				bad_votes = true;
				break;
			}
		 }
		if(bad_votes){continue;}
		
		
		//send authenticated vote to Bulentin Board
		BB.addVote(secretVect);
		
		
		// voter and BB ZKP
		
		/*  					Coulden't get Zero Knowledge Proof working correctly   
		int count = 0;
		while(count < 20){ //loop enough times
			bool verif = true;
			for (int i =0; i < n; i++){
				int N = EM.get_N();
				int r = rand() % N +1;
				while (gcd(r, N) != 1){
					r = rand() % N+1;
				}
				int s = rand() % N +1;
				while (gcd(s, N) != 1){
					s = rand() % N+1;
				}
				int u = (modpow(EM.get_g(), r, N*N) * modpow(s, N, N*N)) % (N*N);
				cout << "s^N: " << modpow(s, N, N*N) << endl;
				cout << "x^r: " << modpow(EM.get_g(), r, N*N) << endl;
				
				
				int e = BB.sendToBB(u, EM.get_g());
				int v = (r - e * plainVect[i]);
				cout << "x: " << EM.get_r() << endl;
				cout << "s: " << s << endl;
				
				int xE = modpow(EM.get_r(), e, N);
				cout << "xE: " << xE << endl;
				int w = (s * modInverse(xE, N*N)) % N; //the get_r() is the x in the ZKP	

				float inter = 1.0*modpow(w, N, N*N) / modpow(s, N, N*N)									//WORKIGN HERE
				
				// int w = (s * modpow(EM.get_r(), e, N)) % N; //the get_r() is the x in the ZKP
				verif = BB.response(v, w, secretVect[i]); //need secretVect
				
				long long check = modpow(EM.get_g(), v, N*N);
				check = (check * modpow(secretVect[i], e, N*N)) % (N*N);
				check = (check * modpow(w, N, N*N)) % (N*N);
				cout << "voter u: " << u << endl;
				cout << "voter u check: " << check << endl;
				
				if (!verif){
					cout << "Yo hold the phone dog this isn't lining up." << endl;
					break;
				}
			}
			count++;
			if (!verif){
				break;
			}
			if (count == 20){
				BB.addVote(secretVect);
			}
		}*/
		
	
	
	}
	
	vector<long long> CandidateSums;
	vector<long long> VoterSums;
	
	//initialize CandidateSums and VoterSums
	for(int i = 0; i<n; i++){
		CandidateSums.push_back(1);
	}
	for(int i = 0; i<voters.size(); i++){
		VoterSums.push_back(1);
	}
	
	//BB sends encrypted votes to counting authority for counting
	BB.computeSums(n, CandidateSums, VoterSums);
	
	// CA adds up encrypted votes and sends to EM for decryption
	EM.decryptResults(VoterSums, CandidateSums, candidates);	
	
	return 0;
}