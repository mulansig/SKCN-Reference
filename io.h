#ifndef IO_H
#define IO_H

#include "Polynomial.h"
				
void pack_sk1(
		unsigned char * sk1, 
		const unsigned char * rho_A, 
		const unsigned char * key,
		const Polynomial s[DIM_s],
		const Polynomial e[DIM_e]);
void unpack_sk1(
		Polynomial A[ROWS_A][COLUMNS_A], unsigned char * rho_A, 
		unsigned char * key,
		Polynomial s[DIM_s], 
		Polynomial e[DIM_e], 
		const unsigned char * sk1);
		
void pack_polyeta(unsigned char *r, const Polynomial *a);
void unpack_polyeta(Polynomial *r, const unsigned char *a);

void pack_sk2(
		unsigned char * sk2, 
		unsigned char * tr, 
		const Polynomial t0[DIM_t0]);
void unpack_sk2(
		unsigned char * tr, 
		Polynomial t0[DIM_t0], 
		const unsigned char * sk2);

void pack_pk(
		unsigned char * pk, 
		const unsigned char * rho_A, 
		const Polynomial t1[DIM_t1]);
void unpack_pk(
		Polynomial A[ROWS_A][COLUMNS_A], 
		unsigned char * rho_A, 
		Polynomial t1[DIM_t1], 
		const unsigned char * pk);		
		
void pack_c(
		unsigned char * p, 
		const Polynomial * c);
void unpack_c(
		Polynomial * c, 
		const unsigned char * p);

void pack_h(
		unsigned char * p, 
		const Polynomial h[DIM_h]);
void unpack_h(
		Polynomial h[DIM_h], 
		const unsigned char * p);
				
void pack_z(
		unsigned char * p, 
		const Polynomial z[DIM_z]);
void unpack_z(
		Polynomial z[DIM_z], 
		const unsigned char * p);
		
void pack_signature(
		unsigned char * signature, 
		const Polynomial z[DIM_z], 
		const Polynomial h[DIM_h], 
		const Polynomial *c);
void unpack_signature(
		Polynomial z[DIM_z], 
		Polynomial h[DIM_h], 
		Polynomial *c, 
		const unsigned char * signature);


void unpack_t0(Polynomial t0[DIM_t0], const unsigned char *p);
void pack_t0(unsigned char * p, const Polynomial t0[DIM_t0]);

void pack_polyeta(unsigned char *r, const Polynomial *a);
void unpack_polyeta(Polynomial *r, const unsigned char *a);


#endif