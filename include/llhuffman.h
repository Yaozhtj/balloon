#ifndef LLHUFFMAN_H
#define LLHUFFMAN_H
#include "universal.h"
#include "treenode.h"
#include <algorithm>
#include <queue>

class LlHuffman {
public:
	LlHuffman();
	~LlHuffman();
	void AddFrequency(uint8 l, bool f);
	void CreatNormalTree();
	void CreatLlHfmTree();
	void GetCodeLength(Node p, uint16 l);
	void GetHfmCode();
	void ResetHfm();
	void DestroyTree(Node p);

	class Cmp {
	public:
		bool operator()(const Node& a, const Node& b) const {
			return a->frequency > b->frequency;
		}
	};

private:
	uint16* lToCode;			// length��Ӧ��������(257-285)
	uint16* codeExtraBits;		// �������extraBits��չλ��
	uint16* codeBeginLl;		// �������ll��Χ������
	uint16* codeLength;			// �������Ӧ�����ֳ���
	uint16* codeLengthCnt;		// �������ȵ��������ֳ��ֵĴ���

	BS* hfmCode;			// �������Ӧ������(�ַ�����)
	BS* nextHfmCode;		// �������ȵ��������ֵ���һ������(�ַ�����)

	Node* codeNode;			// ���ڹ���ԭʼHuffman���Ľڵ�����
	Node root;				// Huffman�����ڵ�
	priority_queue<Node, vector<Node>, Cmp> nodeHeap;  // ����ԭʼHuffman��ʱ��С����
	friend class Deflate;
	friend class Inflate;
};

#endif // !LLHUFFMAN_H
