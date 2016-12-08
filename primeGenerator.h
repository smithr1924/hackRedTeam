#ifndef PRGEN_
#define PRGEN_

#include <vector>
#include <cmath>
#include <stdio.h>
#include <string.h>
using namespace std;

class PrimeGenerator{
	public:
	int size;
	int primes[118];
	
	public:
	PrimeGenerator(){
		
		size = 53;
		int p[53] = {
		31,37,41,43,47,53,59,61,67,71
		,73,79,83,89,97,101,103,107,109,113
		,127,131,137,139,149,151,157,163,167,173
		,179,181,191,193,197,199,211,223,227,229
		,233,239,241,251,257,263,269,271,277,281
		,283,293,307};
		
		memcpy(primes, p, sizeof(primes));
	}

	public:
		int getPrime(){
			int n = rand() % size;
			return primes[n];
		}
};

#endif