#ifndef SIGNATURE_H
#define SIGNATURE_H

#include "Parameters.h"
#include "Polynomial.h"
#include "io.h"

#define SIG_SECRETKEYBYTES 		CRYPTO_SECRETKEYBYTES
#define SIG_PUBLICKEYBYTES 		CRYPTO_PUBLICKEYBYTES
#define SIG_BYTES 				CRYPTO_BYTES
#define SIG_ALGNAME 			"Mulan"

#define MESSAGE_LENGTH_MAX		3300
#define	MESSAGE_LENGTH_IN_TEST	59


int sig_keygen(
		unsigned char * pk, unsigned long long * pk_bytes, 
		unsigned char * sk, unsigned long long * sk_bytes); 
	
int sig_sign(
	unsigned char * sk, unsigned long long sk_bytes,
	unsigned char * m, unsigned long long mlen,
	unsigned char * sm, unsigned long long * smlen);
int sig_sign_with_param_fixed(
	unsigned char * sk, unsigned long long sk_byts,
	unsigned char * m, unsigned long long m_byts,
	unsigned char * rand, unsigned long long rand_byts,
	unsigned char * sn, unsigned long long * sn_byts);

int sig_verf(
	unsigned char * pk, unsigned long long pk_bytes,
	unsigned char * sm, unsigned long long smlen,
	unsigned char * m, unsigned long long mlen);

#endif
