#include <iostream>
#include "MD5.hpp"
#include <unistd.h>

//初始化
MD5::MD5()
:chunkByte_(N)
{
	init();

	memset(chunk_, 0, chunkByte_);
	totalByte_ = lastByte_ = 0;
}

void MD5::init(){
	a_ = 0x67452301;
	b_ = 0xefcdab89;
	c_ = 0x98badcfe;
	d_ = 0x10325476;

	size_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
	memcpy(sft_, s, sizeof(s));

	// for (int i = 0; i < 64; i++){
	// 	k_[i] = (size_t)(abs(sin(i + 1.0)) * pow(2.0, 32));
	// }
	size_t a[]={
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
        0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,0x698098d8,
        0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,
        0xa679438e,0x49b40821,0xf61e2562,0xc040b340,0x265e5a51,
        0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
        0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,
        0xfcefa3f8,0x676f02d9,0x8d2a4c8a,0xfffa3942,0x8771f681,
        0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,
        0xbebfbc70,0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
        0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,0xf4292244,
        0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,
        0xffeff47d,0x85845dd1,0x6fa87e4f,0xfe2ce6e0,0xa3014314,
        0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391};
	memcpy(k_,a,sizeof(a));
}


//处理每个chunk
void MD5::calculateMD5(size_t *chunk){
	size_t a = a_;
	size_t b = b_;
	size_t c = c_;
	size_t d = d_;

	size_t f, g;   //f哈希函数返回值   chunk[g]


	//64次变换，4轮操作，每一轮操作：16个子操作
	for (size_t i = 0; i < 64; i++){
		if (0 <= i&&i < 16){
			f = F(b, c, d);
			g = i;
		}
		else if (16 <= i&&i < 32){
			f = G(b, c, d);
			g = (5 * i + 1) % 16;
		}
		else if (32 <= i&&i < 48){
			f = H(b, c, d);
			g = (3 * i + 5) % 16;
		}
		else{
			f = I(b, c, d);
			g = (7 * i) % 16;
		}
		
		size_t dtmp = d;
		d = c;
		c = b;
		size_t exp=(a + f + k_[i] + chunk[g]);
		b = b + shiftLeftRotate(exp, sft_[i]);
		int time=10;
		
		if(i==15){
			std::cout<<"------------------"<<std::endl;
		while(time--){
			std::cout<<(a + f + k_[i] + chunk[g])<<std::endl;
			sleep(1);
		}
		std::cout<<"------------------"<<std::endl;
		}
		a = dtmp;
		
		//TODO
			// if(i>12&&i<17){
			// std::cout<<"----------------"<<std::endl;
			// std::cout<<"i:  "<<i<<std::endl;
			// std::cout<<"a:  "<<a<<std::endl;
			// std::cout<<"b:  "<<b<<std::endl;
			// std::cout<<"c:  "<<c<<std::endl;
			// std::cout<<"d:  "<<d<<std::endl;
			// std::cout<<"f:  "<<f<<std::endl;
			// std::cout<<"g:  "<<g<<std::endl;
			// std::cout<<"exp:  "<<exp<<std::endl;
			// std::cout<<"chunk_[g] "<<chunk_[g]<<std::endl;
			// std::cout<<"k_[i] "<<k_[i]<<std::endl;
			// std::cout<<"sft_[i] "<<sft_[i]<<std::endl;
			// std::cout<<"----------------"<<std::endl;
			// }
	}


	//更新buffer的值
	a_ += a;
	b_ += b;
	c_ += c;
	d_ += d;
	
}

//处理最后一个chunk
void MD5::calculateMD5Final(){
	//lastByte_:<64byte 最后一块数据大小
	unsigned char *p=chunk_ + lastByte_;
	//填充位的前八bit位：1000 0000 0X80
	*p++ = 0x80;
	size_t remainFillByte = chunkByte_ - lastByte_ - 1;
	if (remainFillByte < 8){
		//不够8字节存放原原始文档的bit长度
		//先将这个处理单元 chunk_ 剩下的部分用0填充满
		memset(p, 0, remainFillByte);
		//对这个chunk_进行处理
		calculateMD5((size_t*)chunk_);
		//重写开辟一个全0 的chunk_
		memset(chunk_, 0, chunkByte_);
	}
	//最后一块chunk_小于448bit
	else{
		memset(p, 0, remainFillByte);
	}
	//最后八个字节存放原始文档的bit长度
	((unsigned long long*)chunk_)[7] = totalByte_ * 8;
	calculateMD5((size_t*)chunk_);
}




std::string MD5::getFileMd5(const char * filename){
	//输入文件流，按二进制处理
	std::ifstream fin(filename, std::ifstream::binary);
	if (fin.is_open()){
		while (!fin.eof()){
			//每次读64个字节
			fin.read((char*)chunk_, chunkByte_);
			//gcount 当前读了多少字节
			//不顾64个，读到最后一块了，跳出循环，单独处理
			if (chunkByte_ != fin.gcount())
				break;
			//当前读到64字节
			totalByte_ += chunkByte_;
			calculateMD5((size_t*)chunk_);
		}
		lastByte_ = fin.gcount();
		totalByte_ += lastByte_;
		calculateMD5Final();
	}
	
	return changeHex(a_) + changeHex(b_) + changeHex(c_) + changeHex(d_);
}

std::string MD5::getStrMd5(const std::string &str){
	if (str.empty()){
		return "";
	}
	else{
		unsigned char*  pstr = (unsigned char *)str.c_str();
		size_t numChunk = str.size() / chunkByte_;
		memset(chunk_, 0, chunkByte_);
		for (size_t i = 0; i < numChunk; i++){
			totalByte_ += chunkByte_;
			calculateMD5((size_t*)(pstr + i*chunkByte_));
		}
		lastByte_ = str.size() % chunkByte_;
		memcpy(chunk_, pstr + totalByte_, lastByte_ );
		totalByte_ += lastByte_;
		calculateMD5Final();
	}
	return changeHex(a_) + changeHex(b_) + changeHex(c_) + changeHex(d_);
}


std::string MD5::changeHex(size_t num){
	static std::string strMap = "0123456789abcdef";
	std::string ret;
	std::string byteStr;
	for (int i = 0; i < 4; i++){
		//因为每次向ret中添加的一个字节的内容，
		//但是一个ret是四个字节，
		//并且在ret拼接时是尾插
		//但是在byteStr中拼接时是头插
		//所以每次进入循环先将bytStr清空
		byteStr = "";
		size_t b = (num >> (i * 8)) & 0x000000ff;
		for (int j = 0; j < 2; ++j){
			byteStr.insert(0, 1, strMap[b % 16]);
			b /= 16;
		}
		ret += byteStr;
	}
	return ret;
}
