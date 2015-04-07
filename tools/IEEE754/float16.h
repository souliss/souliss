/**************************************************************************
	Souliss

	Modified by Dario Di Maio
	
***************************************************************************/

#ifndef FLOAT16_H
#define FLOAT16_H

void float16(uint16_t *output, float *input);
void float32(uint16_t *input,  float *output);
float returnfloat32(uint16_t *input);
uint16_t returnfloat16(float *input);

#endif
