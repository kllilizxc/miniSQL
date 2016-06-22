#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

struct CTOI {
	union {
		char bytes[4];
		uint32_t ctoi;
	};
};

struct CTOF {
	union {
		char bytes[4];
		float ctof;
	};
};

char* IntToChar(uint32_t i) {
	CTOI *temp;
	temp = new CTOI;
	temp->ctoi = i;
	return temp->bytes;
}

char* FloatToChar(float f) {
	CTOF *temp;
	temp = new CTOF;
	temp->ctof = f;
	return temp->bytes;
}

char *StringToChar(string str, int size) {
	char *tempc = new char[size];
	memset(tempc, '\0', size);
	for (int i = 0; i < str.size() && i < size; ++i) {
		tempc[i] = str[i];
	}
	return tempc;
}

int main() {
	char empty[] = "1";
	char full[] = "0";

	string PrevFileName = "t1_000.dbt";
	string NextFileName = "t1_002.dbt";
	uint32_t RecCount = 4;
	uint32_t HeadRecOffset = 300;
	
	uint32_t r1c1 = 1;
	float r1c2 = 35.0;
	char r1c3[10] = "10 words.";

	uint32_t r2c1 = 2;
	float r2c2 = 35.2;
	char r2c3[10] = "10 woads.";

	uint32_t r3c1 = 3;
	float r3c2 = 45.7;
	char r3c3[10] = "10 wowds.";

	char memblock[148];
	memcpy(memblock, StringToChar(NextFileName, 32), 32);
	memcpy(memblock + 32, StringToChar(PrevFileName, 32), 32);
	memcpy(memblock + 64, IntToChar(RecCount), 4);
	memcpy(memblock + 68, IntToChar(HeadRecOffset), 4);

	memcpy(memblock + 72, full, 1);
	memcpy(memblock + 73, IntToChar(r1c1), 4);
	memcpy(memblock + 77, FloatToChar(r1c2), 4);
	memcpy(memblock + 81, r1c3, 10);

	memcpy(memblock + 91, empty, 1);
	memcpy(memblock + 92, IntToChar(r1c1), 4);
	memcpy(memblock + 96, FloatToChar(r1c2), 4);
	memcpy(memblock + 100, r1c3, 10);

	memcpy(memblock + 110, full, 1);
	memcpy(memblock + 111, IntToChar(r2c1), 4);
	memcpy(memblock + 115, FloatToChar(r2c2), 4);
	memcpy(memblock + 119, r2c3, 10);

	memcpy(memblock + 129, full, 1);
	memcpy(memblock + 130, IntToChar(r3c1), 4);
	memcpy(memblock + 134, FloatToChar(r3c2), 4);
	memcpy(memblock + 138, r3c3, 10);

	ofstream file("t1_001.dbt", ios::out | ios::binary);
	if (file.is_open())
	{
		file.write(memblock, 148);
		file.close();

		cout << "the entire file content is in memory";

		delete[] memblock;
	}
	else cout << "Unable to open file";

	return 0;
}