#include <stdint.h>
#include "Parameters.h"
#include "io.h"
#include "Alg.h"
#include "Polynomial.h"

 
int sig_keygen(
		unsigned char * pk, unsigned long long * pk_bytes, 
		unsigned char * sk, unsigned long long * sk_bytes) 
{
	uint32_t i, j;
	unsigned char seedbuf[3*SEEDBYTES];
	unsigned char * rho_se = seedbuf;			// for (s,e)
	unsigned char * rho_A = seedbuf + SEEDBYTES;	// for the matrix A
	unsigned char * key = seedbuf + 2*SEEDBYTES;	// for Key. 

	Polynomial A[ROWS_A][COLUMNS_A];
	Polynomial t[DIM_t], t1[DIM_t], t0[DIM_t]; 
	Polynomial s[DIM_s];
	Polynomial e[DIM_e]; 
	Polynomial temp;
	 
	// sk1 = (rho_A, key, s, e)
	// sk2 = (tr, t0)
	unsigned char * sk1 = sk + 0;
	unsigned char * sk2 = sk + SIZE_sk1;
	unsigned char * tr = sk2 + 0;

	// check invalid argument
	if(NULL == pk || NULL == sk)
		return -2;
	
	
	
	// step 1: generate random bytes. 
	unsigned char seed[SEEDBYTES] = {0};
	for(i=0; i<SEEDBYTES; i++)
		seed[i] = i;
	rand_init(seed, SEEDBYTES);
	
	rand_byts(SEEDBYTES, seedbuf);
	
	shake256(seedbuf, 3*SEEDBYTES, seedbuf, SEEDBYTES);
	
	

	// step 2: generate matrix A. 
	Generate_A(A, rho_A);

	// step 3: generate s, e. 
	for(i = 0; i < DIM_s; i++)
		Poly_uniform_eta(s+i, rho_se, i);	
	for(i = 0; i < DIM_e; i++)
		Poly_uniform_eta(e+i, rho_se, DIM_s+i);
	
	// step 4: pack sk1 = (seed_A, key, s, e)
	pack_sk1(sk1, rho_A, key, s, e);

	//step 5: compute t = As+e.  
 	PolyVec_forward_ntt(s, DIM_s);
	for(i=0; i<ROWS_K; i++)
	{	
		Poly_pointwise_invmontgomery(t+i, A[i]+0, s+0);
		for(j=1; j<COLUMNS_A; j++)
		{
			Poly_pointwise_invmontgomery(&temp, A[i]+j, s+j);
			Poly_add(t+i, t+i, &temp);
		}
		Poly_invntt_frominvmont(t+i);
	}
	PolyVec_add(t, t, e);
	
	// step 6: compute (t1, t0) <- (t1, t0) := Power2Round(t). 
	PolyVec_Power2Round(t1, t0, t);
	
	// step 7: pack pk = (rho_A, t1).
	pack_pk(pk, rho_A, t1);

	// step 8: tr = CRH(pk) = CRH(rho_A, t1)
	shake256(tr, CRHBYTES, pk, SIZE_pk);
	
	// step 9: pack sk2 = (tr, t0);
 	pack_sk2(sk2, tr, t0);
	
	// step 10: return
	*pk_bytes = SIG_PUBLICKEYBYTES;
	*sk_bytes = SIG_SECRETKEYBYTES;
	return 0;
}

int sig_sign(
	unsigned char * sk, unsigned long long sk_bytes,
	unsigned char * m, unsigned long long mlen,
	unsigned char * sm, unsigned long long * smlen)
{
	int ret = sig_sign_with_param_fixed(sk, sk_bytes, m, mlen, NULL, 0, sm, smlen);

	return ret;
}

int sig_sign_with_param_fixed(
	unsigned char * sk, unsigned long long sk_bytes,
	unsigned char * m, unsigned long long m_bytes,
	unsigned char * rand, unsigned long long rand_bytes,
	unsigned char * sm, unsigned long long * sm_bytes)
{
	uint32_t i, j, counter, flag;
	uint32_t nonce = 0;

	Polynomial A[ROWS_A][COLUMNS_A];
	Polynomial s[DIM_s], e[DIM_e];
	Polynomial t0[DIM_t0];
	
	Polynomial u[ROWS_K], v[ROWS_K];
	
	Polynomial c, c_backup; 
	Polynomial y[DIM_y];
	Polynomial z[DIM_z];
	Polynomial w[DIM_w], w1[DIM_w1], w0[DIM_w1];
	
	Polynomial r0[ROWS_K], r1[ROWS_K];
	Polynomial h[ROWS_K];

	unsigned char * sk1 = sk;
	unsigned char * sk2 = sk + SIZE_sk1;
	
	unsigned char buffer[CRHBYTES+MESSAGE_LENGTH_MAX];
	unsigned char * tr = buffer; 
	
	// seebuf = (seed_A, seed_key, mu) 
	unsigned char seedbuf[2*SEEDBYTES + CRHBYTES];
	unsigned char * rho_A = seedbuf + 0;
	unsigned char * key = seedbuf + SEEDBYTES; 
	unsigned char * mu = seedbuf + 2*SEEDBYTES;

	// add by lmwen to check invalid argument
	if(NULL ==sk ||NULL == m || NULL == sm || NULL == sm_bytes || sk_bytes!=SIG_SECRETKEYBYTES)
		return -2;
	
	if(m_bytes>MESSAGE_LENGTH_MAX || m_bytes==0)
	{
		// printf("For ease of demonstration, the signing algorithm only handles message of length:  0< length<=%d. \n", MESSAGE_LENGTH_MAX);
		return -2;
	}
		
	// step1: unpack sk1, sk2, and generate A. 
	unpack_sk1(A, rho_A, key, s, e, sk1);
	unpack_sk2(tr, t0, sk2);
	
	// step 2: mu = CRH(tr, message); 
	memcpy(buffer+CRHBYTES, m, m_bytes);
	shake256(mu, CRHBYTES, buffer, CRHBYTES + m_bytes);
	
	
	PolyVec_forward_ntt(s, DIM_s);
	PolyVec_forward_ntt(e, DIM_e);
	PolyVec_forward_ntt(t0, DIM_t0);
	
	while(1)
	{
		// step 3: sample y
		for(i=0; i < DIM_y; i++)
		{
			poly_uniform_gamma1m1(y+i, key, nonce++);
			Poly_assignment(z+i, y+i);
		}

		// step 4: compute w = Ay
		PolyVec_forward_ntt(y, DIM_y);
		for(i=0; i<ROWS_K; i++)
		{	
			Poly_pointwise_invmontgomery(w+i, A[i]+0, y+0);
			for(j=1; j<COLUMNS_A; j++)
			{
				Poly_pointwise_invmontgomery(r0+i, A[i]+j, y+j);
				Poly_add(w+i, w+i, r0+i);
			}
			Poly_invntt_frominvmont(w+i);
		}
		// PolyVec_freeze(w, DIM_w);
				
		// step 5: compute w1 
		PolyVec_decompose(w1, w0, w, DIM_w); 

		// step 6: sample c
		challenge(&c, mu, w1);
		Poly_assignment(&c_backup, &c);
		
		// step 7: z := y + c*s.
		// step 8: flag1 = Norm{z} vs. bound.
		PolyVec_forward_ntt(&c, DIM_c);
		for(j=0; j<DIM_s; j++)
		{
			Poly_pointwise_invmontgomery(v+j, &c, s+j);
			Poly_invntt_frominvmont(v+j);
			Poly_add(z+j, z+j, v+j);
			flag = Poly_check_norm(z+j, NORMBOUND_z);
			if(flag) break;
		}
		if(j<DIM_s) continue;		
	
		
		for(j=0; j<ROWS_K; j++)
		{
			// step 9: u := w-ce, 
			Poly_pointwise_invmontgomery(u+j, &c, e+j);
			Poly_invntt_frominvmont(u+j);
			Poly_sub(u+j, w+j, u+j);

			// step 10: [r1, r0] := Decompose(u[j]), where u:=w-ce. 
			Poly_decompose(r1+j, r0+j, u+j);

			// step 11: flag2 = (Norm{r0} >= bound) 
			flag = Poly_check_norm(r0+j, NORMBOUND_r0);
			if(flag) break;

			// step 12: flag3 = (r1<>w1)
			flag = Poly_compare(r1+j, w1+j);
			if(flag) break;
		}
		if(j<ROWS_K) continue;

		for(j=0; j<ROWS_K; j++)
		{
			//step 13: v := ct0. 
			Poly_pointwise_invmontgomery(v+j, &c, t0+j);
			Poly_invntt_frominvmont(v+j);
			
			//step 14: flag4 := (Norm(v[i]) >= bound)
			flag = Poly_check_norm(v+j, NORMBOUND_ct0);
			if(flag==1)  break; 
		}
		if(j<ROWS_K) continue;
		
		// step 15: flag5 = #1's in h vs. OMEGA.
		PolyVec_add(v, u, v);
		counter = PolyVec_make_hint(h, v, u);
		if(counter>OMEGA) continue; 

		// step 16: pack signature=(z,h,c). 
		PolyVec_freeze(z, DIM_z);
		pack_signature(sm, z, h, &c_backup); 
		*sm_bytes = CRYPTO_BYTES;

		// step 17: return. 
		return 0;
	}	
}

int sig_verf(
	unsigned char * pk, unsigned long long pk_bytes,
	unsigned char * sm, unsigned long long smlen,
	unsigned char * m, unsigned long long mlen)
{
	Polynomial A[ROWS_A][COLUMNS_A], t1[DIM_t1];
	Polynomial z[DIM_z], h[DIM_h], c; 
	Polynomial c_backup; 
	
	Polynomial u[ROWS_K];
	Polynomial v[ROWS_K];
	Polynomial temp;

	unsigned char rho_A[SEEDBYTES]; 
	unsigned char mu[CRHBYTES];
	unsigned char buffer[CRHBYTES+MESSAGE_LENGTH_MAX];

	uint32_t flag; 
	uint32_t i, j; 

	// check invalid argument
	if(NULL ==pk ||NULL == m || NULL == sm || smlen != SIG_BYTES || pk_bytes!=SIG_PUBLICKEYBYTES)
		return -1;
	
	if(mlen>MESSAGE_LENGTH_MAX || mlen==0)
	{
		printf("For ease of demonstration, the signing algorithm only handles message of length:  0< length<=%d. \n", MESSAGE_LENGTH_MAX);
		return -2;
	}

	// step 1: unpack pk, and generate A. 
	unpack_pk(A, rho_A, t1, pk);

	// step 2: unpack (z, h, c) from sm.
	unpack_signature(z, h, &c, sm);

	// step 3: flag1: norm(z) vs. NORMBOUND_z.
	flag = PolyVec_check_norm(z, DIM_z, NORMBOUND_z);
	if(flag) // indicatin failure. 
		return 1;
	
	// step 4: mu := CRH(tr, message)
	shake256(buffer, CRHBYTES, pk, SIZE_pk);
	memcpy(buffer+CRHBYTES, m, mlen);
	shake256(mu, CRHBYTES,buffer, CRHBYTES + mlen);
	
 	// step 5: u := Az-ct1, v := ct1. 
	Poly_assignment(&c_backup, &c);
	PolyVec_forward_ntt(&c, DIM_c);
	PolyVec_forward_ntt(z, DIM_z);
	for(i=0; i<ROWS_A; i++)
	{
		Poly_pointwise_invmontgomery(u+i, A[i]+0, z+0);
		for(j=1; j<COLUMNS_A; j++)
		{
			Poly_pointwise_invmontgomery(&temp, A[i]+j, z+j);
			Poly_add(u+i, u+i, &temp);
		}
		
		Poly_forward_ntt(t1+i);	
		Poly_pointwise_invmontgomery(v+i, &c, t1+i);
		Poly_sub(u+i, u+i, v+i);
		Poly_invntt_frominvmont(u+i);
	}

	// step 6: w1' := UseHint(h, Az-ct1).
	PolyVec_use_hint(v, h, u);

	// step 7: temp := H(mu, w1')
	challenge(&temp, mu, v);

	// step 8: flag3 = Poly_compare(&temp, &backup_c);
	// step 9: return. 
	return (Poly_compare(&temp, &c_backup)==0);
}

