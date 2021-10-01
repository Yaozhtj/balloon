#ifndef DISHUFFMAN_H
#define DISHUFFMAN_H
#include "universal.h"
#include "treenode.h"
#include <algorithm>
#include <queue>

class DisHuffman{
public:
	DisHuffman();
	~DisHuffman();
	void AddFrequency(uint16 d);
	void CreatNormalTree();
	void CreatDisHfmTree();
	void GetCodeLength(Node p, uint16 l);
	void GetHfmCode();
	void ResetHfm();
	void DestroyTree(Node p);

	class Cmp{
	public:
		bool operator()(const Node& a, const Node& b) const {
			return a->frequency > b->frequency;
		}
	};

private:

	uint8* disToCode;			// distance��Ӧ��������(0-29)
	uint16* codeExtraBits;		// �������extraBits(��չλ��)
	uint16* codeBeginDis;		// �������distance��Χ������
	uint16* codeLength;			// �������Ӧ�����ֳ���
	uint16* codeLengthCnt;		// �������ȵ��������ֳ��ֵĴ���

	uint32* hfmCodeInt;		// �������Ӧ������(����)
	uint32* nextHfmCodeInt;	// �������ȵ��������ֵ���һ������(����)
	BS* hfmCode;			// �������Ӧ������(�ַ�����)

	Node* codeNode;			// ���ڹ���ԭʼHuffman���Ľڵ�����
	Node root;				// ���ڵ�
	priority_queue<Node, vector<Node>,  Cmp> nodeHeap;  // ����ԭʼHuffman��ʱ��С����
	friend class Deflate;
	friend class Inflate;
};
#endif