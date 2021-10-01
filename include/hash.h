#ifndef HASH_H
#define HASH_H
#include "universal.h"
class HashTable {
public:
	HashTable(uint16 size);
	~HashTable();
	void Updata();
	void HashFunction(uint16& hashAddr, uint8 ch);
	void Insert(uint16& matchHead, uint8 ch, uint16 pos, uint16& hashAddr);

private:
	uint16* head;		// ��ϣ���head���飬���Դ��ĳ�����ַ����ַ�����ƥ��ͷ������Ϊ�����ַ����ַ�����Ӧ�Ĺ�ϣ��ַ��ͨ����ϣ������ȡ��
	uint16* prev;		// ��ϣ���prev���飬���Դ����һ���ַ�����λ�ã����ڽ����ϣ��ͻ���ַ�����ƥ����*/
	friend class Deflate;
};

#endif