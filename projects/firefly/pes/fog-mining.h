#ifndef FOG_MINING_H_
#define FOG_MINING_H_

#define THRESHHOLD_DIFF		1000

typedef struct{
	uint16_t prev;
	uint16_t curr;
	uint16_t safelen;
}fogdata_t, *fogdata_ptr;

#endif
