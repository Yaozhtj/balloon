#include "hash.h"

/*==========================================================
���ܣ�	HashTable��Ĺ��캯��
����ֵ��	void
������	�����ϣ������head��prev�����뺬��2*HSIZE��Ԫ�صĿռ�
		��head��prev������ڴ��������������Ҹ�0
==========================================================*/
HashTable::HashTable(uint16 size) {
	prev = new uint16[(int32)size * 2];
	head = prev + size;
	// ��head��prev����Ԫ�ظ�0
	memset(prev, 0, (int32)size * 2 * sizeof(uint16));
}

/*==========================================================
���ܣ�	HashTable�����������
����ֵ��	void
������	delete����prev����ĺ���2*HSIZE��Ԫ�صĿռ�
		��head��prevָ���ÿ�
==========================================================*/
HashTable::~HashTable() {
	delete[] prev;
	prev = nullptr;
	head = nullptr;
}

/*==========================================================
���ܣ�	���¹�ϣ��
����ֵ��	void
������	��������ǰ�ƶ�32Kʱ������ǰ��������С����MIN_LOOKAHEAD��
		��Ҫ��head�����д��λ�ô���32K��Ԫ�ؼ�ȥ32K������ŵ�λ
		��С��32K��Ԫ����0
==========================================================*/
void HashTable::Updata() {
	for (uint32 i = 0; i < HSIZE; i++) {
		// ����head����
		if (head[i] >= WSIZE) {
			head[i] -= WSIZE;
		}else{
			head[i] = 0;
		}
		// ����prev����
		if (prev[i] >= WSIZE) {
			prev[i] -= WSIZE;
		}else{
			prev[i] = 0;
		}
	}
}

/*==========================================================
���ܣ�	����ַ����Ĺ�ϣ����
����ֵ��	void
������	ͨ�������ַ��������ַ����ĵ������ַ�����ȡ���ַ����Ĺ�ϣ
		��ַ���������ַ�����ϣ��ַ��Ҫ�����ַ���ֻ����һ���ַ���
		ԭ����ǰ�����ַ��Ĺ�ϣ��ַ�Ѿ�����������һ���ַ����Ĺ�ϣ
		��ַ�У�����ֻ��Ҫ�������ַ��Ϳ��Խ��ô��Ĺ�ϣ��ַ�����
==========================================================*/
void HashTable::HashFunction(uint16& hashAddr, uint8 ch) {
	hashAddr = ((hashAddr << 5) ^ ch) & HASHMASK;
}

/*==========================================================
���ܣ�	ͨ���ַ����Ĺ�ϣ��ַ����λ�ò��뵽��ϣ����
����ֵ��	void
������	ÿ�����л�������ȡһ���ַ�ʱ���ͼ���������������ַ����
		���ַ����Ĺ�ϣ��ַ��head���������������ö�Ӧhead����
		Ԫ�أ���Ϊ0˵����ƥ�䣬��֮ǰû�г��ֹ��ô������ٽ���Ԫ��
		����prev����������Ϊ��ǰλ�õ�Ԫ�أ��Ӷ��γ�ƥ����
==========================================================*/
void HashTable::Insert(uint16& matchHead, uint8 ch, uint16 pos, uint16& hashAddr) {
	// ͨ����ϣ�����������ǰ���Ĺ�ϣ��ַ
	HashFunction(hashAddr, ch);
	// ����ƥ����ͷ�����ڲ���ƥ�䴮�����ٽ�head[hashAddr]��ֵ���뱾�������ƥ�䴮��λ�ã�����ƥ����ͷ
	matchHead = head[hashAddr];
	// ��head[hashAddr]��ֵ���뱾�������ƥ�䴮��λ�ã�����prev����������Ϊ��ǰλ�õ�Ԫ�أ��Ӷ��γ�ƥ����
	prev[pos & HASHMASK] = head[hashAddr];
	// ����ǰλ�ø���ƥ������ͷ
	head[hashAddr] = pos;
}