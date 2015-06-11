#define CORDIC_MAX_ITERATIONS 30
#define CORDIC_SCALING_FACTOR      (1 << CORDIC_MAX_ITERATIONS)
#define CORDIC_SCALING_FACTOR_SQRT (1 << (CORDIC_MAX_ITERATIONS / 2))


int CORDIC_circular_vectoring_mode(int n, long long *px0, long long *py0)
{
	unsigned int theta[CORDIC_MAX_ITERATIONS];
	int i;
	int sigma;

	long long x,y;
	int z;
	long long x_tmp, y_tmp;
	double k = 1;
			
	x = *px0; y = *py0;
	z = 0;

	for (i = 0; i < n; ++i) {

		theta[i] = floor((atan2 (1, 1 << i) * CORDIC_SCALING_FACTOR + 0.5));
		k = k / sqrt (1 + 1.0 / (((long long)1) << (2 * i)));
	
		if  (y >= 0)  {
			sigma = -1;
		} else {
			sigma = 1;
		}

		if (i > 0) {
			x_tmp = x - sigma * (((y >> (i - 1)) + 1) >> 1);
			y_tmp = (((x >> (i - 1)) + 1) >> 1) * sigma + y;
		} else {
			x_tmp = x - sigma * y;
			y_tmp = x * sigma + y;
		}

		x = x_tmp;
		y = y_tmp;

		z = z - theta[i] * sigma;
	
		printf ("i = %d, k = %lf\n", i, k);
		
	} // End of for loop

	*px0 = x;
	
	return z;
}	   


long long CORDIC_amplitude (long long x, long long y)
{
	long long amp;
	long long K1 = 0.607253 * CORDIC_SCALING_FACTOR;

	CORDIC_circular_vectoring_mode(CORDIC_MAX_ITERATIONS, &x, &y);

	amp = (K1 * x + (CORDIC_SCALING_FACTOR / 2))/ CORDIC_SCALING_FACTOR;

	return amp;
}

int CORDIC_arctan (long long x, long long y)
{
	if ((y == 0) && (x == 0)) {
		return -1; // error condition
	} else {
		// scale for best precision
		while ( (abs(x) < (CORDIC_SCALING_FACTOR / 2)) && 
               (abs(y) < (CORDIC_SCALING_FACTOR / 2))) { 
				x *= 2;
				y *= 2;
		} // End of while loop

		return CORDIC_circular_vectoring_mode (CORDIC_MAX_ITERATIONS, &x, &y);
	}
}

long long CORDIC_circular_rotation_mode(int n, long long *px0, long long *py0,
                                        signed long long z0)
{
	unsigned int theta[CORDIC_MAX_ITERATIONS];
	int i;
	int sigma;

	long long x,y;
	long long z;
	long long x_tmp, y_tmp;
		
	x = *px0; y = *py0;
	z = z0;
	for (i = 0; i < n; ++i) {
		theta[i] = floor((atan2 (1, 1 << i) * CORDIC_SCALING_FACTOR + 0.5));
		
       if  (z < 0)  {
			sigma = -1;
		} else {
			sigma = 1;
		}

		if (i > 0) {
			x_tmp = x - sigma * (((y >> (i - 1)) + 1) >> 1);
			y_tmp = (((x >> (i - 1)) + 1) >> 1) * sigma + y;
		} else {
			x_tmp = x - sigma * y;
			y_tmp = x * sigma + y;
		}

		x = x_tmp;
		y = y_tmp;

		z = z - (long long)theta[i] * sigma;
				
	} // End of for loop
		
	*px0 = x; *py0 = y;
	return z;
}

long long CORDIC_sin (long long z)
{
	long long K1 = 0.607253 * CORDIC_SCALING_FACTOR;
	long long x = CORDIC_SCALING_FACTOR;
	long long y = 0;

	CORDIC_circular_rotation_mode(CORDIC_MAX_ITERATIONS, &x, &y, z);

	return ((K1 * y + (CORDIC_SCALING_FACTOR / 2))/ CORDIC_SCALING_FACTOR);
}


long long CORDIC_cos (long long z)
{
	long long K1 = 0.607253 * CORDIC_SCALING_FACTOR;
	long long x = CORDIC_SCALING_FACTOR;
	long long y = 0;

	CORDIC_circular_rotation_mode(CORDIC_MAX_ITERATIONS, &x, &y, z);

	return ((K1 * x + (CORDIC_SCALING_FACTOR / 2))/ CORDIC_SCALING_FACTOR);
}

int CORDIC_linear_vectoring_mode (int n, long long x0, long long y0, int z0)
{
  int i;
	int sigma;

	long long x,y;
	int z;
	long long x_tmp, y_tmp;

	x = x0; y = y0;
	z = z0;

	for (i = 1; i <= n; ++i) {

		if  (y >= 0)  {
			sigma = -1;
		} else {
			sigma = 1;
		}

		x_tmp = x;
		y_tmp = (((x >> (i - 1)) + 1) >> 1) * sigma + y;

		z = z - (CORDIC_SCALING_FACTOR >> i) *   sigma;

		x = x_tmp;
		y = y_tmp;
		
	} // End of for loop

	return z;
}

int CORDIC_division (long long x, long long y)
{
	if ((y == 0) && (x == 0)) {
		return -1; // error condition
	} else {
		// scale for best precision
		while ((abs(x) < (CORDIC_SCALING_FACTOR / 2)) && (abs(y) < (CORDIC_SCALING_FACTOR / 2))) { 
				x *= 2;
				y *= 2;
		} // End of while loop

		return CORDIC_linear_vectoring_mode (CORDIC_MAX_ITERATIONS, x, y, 0);
	}

}

int CORDIC_linear_rotation_mode(int n, 
                                long long x0, long long y0, long long z0)
{
	int i;
	int sigma;

	long long x,y;
	long long z;
	long long x_tmp, y_tmp;

	x = x0; y = y0;
	z = z0;
	for (i = 1; i <=n; ++i) {

		if  (z >= 0)  {
			sigma = 1;
		} else {
			sigma = -1;
		}

		x_tmp = x ;
		y_tmp = (((x >> (i - 1)) + 1) >> 1) * sigma + y;

		z = z - (long long)(CORDIC_SCALING_FACTOR >> i) *   sigma;

		x = x_tmp;
		y = y_tmp;
		
	} // End of for loop

	return y;
}

long long CORDIC_mult (int x, int y)
{
	long long result;

	result = CORDIC_linear_rotation_mode (CORDIC_MAX_ITERATIONS + 1, 
         (long long)x * CORDIC_SCALING_FACTOR_SQRT * 2, 0, 
         (long long)y * CORDIC_SCALING_FACTOR_SQRT);

	result = (result + 1) >> 1;

	return result;
}

void CORDIC_hyperbolic_vectering_mode (int n, 
                               long long *px0, long long *py0, long long *pz0)
{
	unsigned int theta[CORDIC_MAX_ITERATIONS + 1];
	int i,j, iter;
	int sigma;
	int again;

	long long x, y, z;
	long long x_tmp, y_tmp;
	int repeat_point = 4;
	
	double k = 1;
	iter = 1;

	x = *px0; y = *py0; z = *pz0;

	for (i = 1; i <= n; ++i) {

		if (i == repeat_point) {
			again = 1;
			repeat_point = repeat_point * 3 + 1;
		} else {
			again = 0;
		}

		for (j = 0; j <= again; ++j) {
			if  (y >= 0)  {
				sigma = -1;
			} else {
				sigma = 1;
			}
		
			x_tmp = x + sigma * (((y >> (i-1)) + 1) >> 1);
			y_tmp = (((x >> (i-1)) + 1) >> 1) * sigma + y;
			x = x_tmp;
			y = y_tmp;

			theta[i] = floor(((log((1 + (double)1/(1 << i)) / (1 - (double)1/(1 << i)) ) / 2) * CORDIC_SCALING_FACTOR + 0.5));

			z = z - (long long)theta[i] * sigma;
						
			k = k / sqrt ( 1 - 1.0 / ((long long)1 << (2 * i)));
			printf ("---------------------- iter = %d, k = %f\n", iter++, k);

		} // End of for loop
		
	} // End of for loop

	*px0 = x;
	*py0 = 0;
	*pz0 = z;
}

long long CORDIC_sqrt (long long w)
{
	long long quarter = (long long)CORDIC_SCALING_FACTOR / 4;

	long long x = w + quarter;
	long long y = w - quarter;
	long long z = 0;
	long long result;

	long long Km1 = 1.207497 * CORDIC_SCALING_FACTOR_SQRT;

	CORDIC_hyperbolic_vectering_mode (CORDIC_MAX_ITERATIONS, &x, &y, &z);

	result = (long long)((Km1 * x + (CORDIC_SCALING_FACTOR / 2))/  
                      CORDIC_SCALING_FACTOR);
	
	return result;
}

long long CORDIC_ln (long long w)
{
	long long one = (long long)CORDIC_SCALING_FACTOR;

	long long x = w + one;
	long long y = w - one;
	long long z = 0;
	long long result;
		
	CORDIC_hyperbolic_vectering_mode (CORDIC_MAX_ITERATIONS, &x, &y, &z);
	z *= 2;

	return z;
}

void CORDIC_hyperbolic_rotation_mode (int n, long long *px0, long long *py0, long long *pz0)
{
	unsigned int theta[CORDIC_MAX_ITERATIONS + 1];
	int i,j, iter;
	int sigma;
	int again;

	long long x, y, z;
	long long x_tmp, y_tmp;
	int repeat_point = 4;

	x = *px0; y = *py0; z = *pz0;
	for (i = 1; i <= n; ++i) {

		if (i == repeat_point) {
			again = 1;
			repeat_point = repeat_point * 3 + 1;
		} else {
			again = 0;
		}

		for (j = 0; j <= again; ++j) {
			if  (z >= 0)  {
				sigma = 1;
			} else {
				sigma = -1;
			}
		
			x_tmp = x + sigma * (((y >> (i-1)) + 1) >> 1);
			y_tmp = (((x >> (i-1)) + 1) >> 1) * sigma + y;
			x = x_tmp;
			y = y_tmp;

			theta[i] = floor(((log((1 + (double)1/(1 << i)) / (1 - (double)1/(1 << i)) ) / 2) * CORDIC_SCALING_FACTOR + 0.5));
			z = z - (long long)theta[i] * sigma;
		} // End of for loop
		
	} // End of for loop

	*px0 = x;
	*py0 = 0;
	*pz0 = z;
}

long long CORDIC_exp (long long w)
{
	long long one = (long long)CORDIC_SCALING_FACTOR;

	long long x = one;
	long long y = one;
	long long z = w;
	long long result;

	long long Km1 = 1.207497 * CORDIC_SCALING_FACTOR;

	CORDIC_hyperbolic_rotation_mode (CORDIC_MAX_ITERATIONS, &x, &y, &z);

	result = (long long)((Km1 * x + (CORDIC_SCALING_FACTOR / 2))/ CORDIC_SCALING_FACTOR);
	
	return result;
}


