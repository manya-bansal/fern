#include "black_scholes.h"
#include "array-mock.h"
using namespace mock;

float normal_cdf(float x)
{
	return std::erfc(-x / std::sqrt(2)) / 2;
}

void black_scholes(Array<float> stock_price, Array<float> strike_price, Array<float> volatility, Array<float> call, int64_t len)
{
    float r = 0.01;
    float T = 0.25;
	for (int i = 0; i < len; i++)
	{
		float sigma = volatility.data[i];
		float d1 = (log(stock_price.data[i] / strike_price.data[i]) + (r + sigma * sigma / 2) * T) / (sigma * sqrt(T));
		float d2 = d1 - sigma * sqrt(T); 
		call.data[i] = normal_cdf(d1) * stock_price.data[i] - normal_cdf(d2) * strike_price.data[i] * exp(-r * T);
	}
}

int main(void)
{
	float EPSILON = 0.001;
	float stock_price[10] = {100, 389, 281, 221, 114, 69, 82, 302, 102, 32};
	float strike_price[10] = {120, 391, 277, 200, 113, 80, 85, 310, 122, 20};
	float vol_in[10] = {0.5, 0.7, 0.3, 0.45, 0.22, 0.9, 0.4, 0.1, 0.6, 0.53};
	Array<float> price = Array<float>(10);
	Array<float> strike = Array<float>(10);
	Array<float> volatility = Array<float>(10);

	for (int i = 0; i < 10; i++)
	{
		price.data[i] = stock_price[i];
		strike.data[i] = strike_price[i];
		volatility.data[i] = vol_in[i];
	}

	Array<float> out = Array<float>(10);
	Array<float> naive_out = Array<float>(10);

	my_fused_impl(strike, price, volatility, out, 10);
	black_scholes(price, strike, volatility, naive_out, 10);
	bool good = true;
	for (int i = 0; i < out.logical_size; i++)
	{
		// printf("%f %f\n", out.data[i], naive_out.data[i]);
		if (abs(out.data[i] - naive_out.data[i]) > EPSILON) {
			good = false;
			printf("Error: at idx %d, expected %f, got %f\n", i, naive_out.data[i], out.data[i]);
		}
	}
	if (good) {
		printf("Good. Values match\n");
	}
	return 0;
}
