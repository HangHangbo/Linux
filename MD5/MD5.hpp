

#include <cmath>
#include <cstring>
#include <fstream>
#include <string>
#define N 64

class MD5{
public:
	MD5();
	void init();
	std::string getFileMd5(const char * filename);
	std::string getStrMd5(const std::string &str);
private:
	
	std::string string_To_UTF8(const std::string & str);
	//处理每个chunk
	void calculateMD5(size_t* chunk);
	//处理最后一个chunk
	void calculateMD5Final();
	
	//std::string getStrMd5(const std::string &str);
	std::string changeHex(size_t num);
		//每个chunk 占64个byte 每四个字节处理 便将chunk转位size_t 
	size_t F(size_t x, size_t y, size_t z){
		return (((x) & (y)) | ((~x) & (z)));
		
	}
	size_t G(size_t x, size_t y, size_t z){
		return (((x) & (z)) | ((y) & (~z)));
	}
	size_t H(size_t x, size_t y, size_t z){
		return  ((x) ^ (y) ^ (z));
	}
	size_t I(size_t x, size_t y, size_t z){
		return	((y) ^ ((x) | (~z)));
	}
	size_t shiftLeftRotate(size_t num, size_t n){
		// size_t a=num<<n;
		// size_t b=num>>(32-n);
		return ((num) << (n)) | ((num) >> (32 - (n)));
		
	}
private:

	//因为在算k_[N]的时候要计算2^32 int存不下
	size_t a_ ;
	size_t b_ ;
	size_t c_ ;
	size_t d_ ;
	/*
	word A: 01 23 45 67
	word B: 89 ab cd ef
	word C: fe dc ba 98
	word D: 76 54 32 10
	低字节子前
	a= 0x67452301;
	b = 0xefcdab89;
	c = 0x98badcfe;
	d= 0x10325476;
	*/

	size_t k_[N];
	//循环左移偏移量
	size_t sft_[N];

	const size_t chunkByte_;
	unsigned char chunk_[N];	//每次要处理的大小

	size_t lastByte_;           //最后一块要处理的数据大小

	//原始文件的字节长度
	unsigned long long totalByte_;
};
