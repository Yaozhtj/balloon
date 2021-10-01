#ifndef DEFLATE_H
#define DEFLATE_H
#define _CRT_SECURE_NO_WARNINGS
#include "hash.h"
#include "dishuffman.h"
#include "llhuffman.h"
#include <fstream>
#include <cstdio>
#include <assert.h>

class Deflate {
public:
	Deflate();
	~Deflate();
	void Compress(string fileName, string newFileName);
	void MoveWindow(size_t& lookAhead, uint16& strStart);
	uint16 GetMaxMatchLength(uint16 matchHead, uint16& tMatchDistance, uint16 strStart, uint32 lookAhead);
	void WriteFlag(uint8& fData, uint8& fBitCnt, bool hasEncode, uint8& oData, uint8& oBitCnt);
	void OutHuffman(uint8& oData, uint8& oBitCnt);
	inline void OutBit(uint8& oData, uint8& oBitCnt, bool bit);
private:
	uint8* window;				// �������ڣ���СΪ64K�����ڴ�Ż�����������
	HashTable* lzHash;			// ���ڶ�Ӧ�Ĺ�ϣ�����Ŵ��ڵĻ������ϸ���
	DisHuffman* disHuffman;		// Distance����Huffman��
	LlHuffman* llHuffman;		// literal/length����Huffman��
	FILE* fI;					// ��������ļ�
	FILE* fO;
	uint8* fBuff;				// ������
	uint32 fBuffCnt;
	uint8* lBuff;
	uint32 lBuffCnt;
	uint16* dBuff;
	uint32 dBuffCnt;
	uint8* oBuff;
	uint32 oBuffCnt;
};

#endif