#include <stdint.h>
#include <time.h>
#include "Alg.h"
#include "Parameters.h"
#include "Rounding.h"

#define NTESTS 100000
#define MLEN 59


int main(void)
{		
	uint32_t i;

	uint32_t ret;

	unsigned long long smlen;
	unsigned char m[MLEN];
	unsigned char sn[MLEN + CRYPTO_BYTES];
	unsigned char pk[CRYPTO_PUBLICKEYBYTES];
	unsigned char sk[CRYPTO_SECRETKEYBYTES];
	
	unsigned long long t0[NTESTS], t1[NTESTS], t2[NTESTS];
	unsigned long long pk_bytes, sk_bytes, sn_bytes;
	
	for(i=0; i<NTESTS; i++) 
	{
		randombytes(m, MLEN);

		sig_keygen(pk, &pk_bytes, sk, &sk_bytes);
		sig_sign(sk, sk_bytes, m, MLEN, sn, &sn_bytes);
		ret = sig_verf(pk, pk_bytes, sn, sn_bytes, m, MLEN);
		if(ret!=1)
		{
			printf("ret = %d, Verification failed. \n", ret);
			getchar();
			return -1;
		}
 	}

	printf("Finished successfully. \n\n");
	return 0;
}

