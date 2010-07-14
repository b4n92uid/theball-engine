#include <iostream>
#include "../sources/VectorLib.h"

using namespace std;
using namespace tbe;

#define TYPE int
#define UNIT 1

int main()
{
	Vector4<TYPE> vec1(UNIT), vec2(UNIT);
	
	vec1 = UNIT;
	vec1 = vec2;
	
	vec1 += UNIT;
	vec1 -= UNIT;
	vec1 *= UNIT;
	vec1 /= UNIT;
	
	vec1 += vec2;
	vec1 -= vec2;
	vec1 *= vec2;
	vec1 /= vec2;
	
	vec1 = vec1 + UNIT;
	vec1 = vec1 - UNIT;
	vec1 = vec1 * UNIT;
	vec1 = vec1 / UNIT;
	
	vec1 = vec1 + vec2;
	vec1 = vec1 - vec2;
	vec1 = vec1 * vec2;
	vec1 = vec1 / vec2;
	
	if(vec1);
	
	if(!vec1);
	
	vec1 = -vec2;
	vec1 = -UNIT;
	
	vec1 == vec2;
	vec1 != vec2;
	vec1 < vec2;
	vec1 > vec2;
	
	vec1 == UNIT;
	vec1 != UNIT;
	vec1 < UNIT;
	vec1 > UNIT;
	
	TYPE* ptr = vec1;
	
	return 0;
}
