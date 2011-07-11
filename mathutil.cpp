#include "headers.h"

float* generateBoxKernel() {
	float* result = new float[4];
	for(int x=0; x<4; x++) {
		result[x] = 0.25f;	
	}
	return result;
}


float* generateTentKernel(float scale) {
	float* result = new float[8];
	float step = 1.f/16.f;
	float t = -1.f;
	for(int x=0; x<8; x++) {
		if(abs(t) < abs(scale)) result[x] = 1 - abs(t/scale);
		else result[x] = 0;
		t += step;
	}
	return result;
}

int* convolute(int* f, int f_size, float* g, int g_size) {
	// convolutes two discrete sampled functions, algorithm found on wikipedia
	int* result = new int[f_size];
	for(int x=0; x<f_size; x++) {
		result[x] = 0;
		for(int y=0; y<g_size; y++) {
			if(y > x) result[x] = f[x];
			else result[x] += int(floor(f[x-y]*g[y]));	
		}
	}
	delete [] f;
	delete [] g;
	return result;
}