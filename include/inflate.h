#ifndef INFLATE_H
#define INFLATE_H
#define _CRT_SECURE_NO_WARNINGS
#include "dishuffman.h"
#include "llhuffman.h"
#include <fstream>
#include <cstdio>
#include <assert.h>

class Inflate {
public:
	Inflate();
	~Inflate();
	void Uncompress(string fileName, string newFileName);
	uint16 EncodeALl();
	uint16 EncodeADis();
	uint8 InBit();
private:
	DisHuffman* disHuffman;		// Distance�ع�Huffman��
	LlHuffman* llHuffman;		// literal/length�ع�Huffman��
	FILE* fI;					// ��������ļ�
	FILE* fO;		
	uint8* oBuff;				// ������
	uint32 oBuffCnt;
	uint8 iData = 0;			// 8bits������Ϣ
	uint8 iBitCnt = 0;			// ������Ϣ�Ѿ������bitλ��
};


#endif // !INFLATE_H
