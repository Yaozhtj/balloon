#include "llhuffman.h"

LlHuffman::LlHuffman(){
	// ��ʼ��lToCode
	lToCode = new uint16[256]{
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
		13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
		17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
		19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
		23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
		25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
		26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
		26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
		27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
	};
	// ��ʼ��codeNode
	codeNode = new Node[LL_CODENUM];
	for (int32 i = 0; i < LL_CODENUM; i++) {
		codeNode[i] = new TreeNode;
		codeNode[i]->code = i;
	}
	// ��ʼ��codeExtraBits
	codeExtraBits = new uint16[LL_CODENUM - 257]{
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
	// ��ʼ��codeBeginLl
	codeBeginLl = new uint16[LL_CODENUM - 257]{ 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
		64, 80, 96, 112, 128, 160, 192, 224, 255 };
	// ��ʼ��codeLength
	codeLength = new uint16[LL_CODENUM];
	for (int32 i = 0; i < LL_CODENUM; i++) codeLength[i] = 0;
	// ��ʼ��codeLengthCnt
	codeLengthCnt = new uint16[LL_MAX_CODELENGTH];
	for (int32 i = 0; i < LL_MAX_CODELENGTH; i ++)	codeLengthCnt[i] = 0;
	// ��ʼ��hfmCode
	hfmCode = new BS[LL_CODENUM];
	// ��ʼ��nextHfmCode
	nextHfmCode = new BS[LL_MAX_CODELENGTH];
	// ��ʼ�������ڵ�
	root = NULL;
}

LlHuffman::~LlHuffman(){
	delete[] lToCode;
	delete[] codeBeginLl;
	delete[] codeExtraBits;
	for (int32 i = 0; i < LL_CODENUM; i++) delete codeNode[i];
	delete[] codeNode;
	delete[] codeLength;
	delete[] codeLengthCnt;
	delete[] hfmCode;
	delete[] nextHfmCode;
}

void LlHuffman::AddFrequency(uint8 l, bool f){
	// length��literal
	if (f)	codeNode[257 + lToCode[l]]->frequency++;
	else codeNode[l]->frequency++;
	return ;
}

void LlHuffman::CreatNormalTree(){
	// ���������Ӧ�Ľڵ���뵽���ȶ���
	for (int32 i = 0; i < LL_CODENUM; i++) {
		if (codeNode[i]->frequency > 0)
			nodeHeap.push(codeNode[i]);
	}
	// ����ԭʼ��huffman��
	while (!nodeHeap.empty()) {
		Node p = nodeHeap.top();
		nodeHeap.pop();
		if (nodeHeap.empty()) {
			root = p;
			break;
		}
		Node q = nodeHeap.top();
		nodeHeap.pop();
		Node newNode = new TreeNode;
		newNode->frequency = p->frequency + q->frequency;
		if (p->frequency <= q->frequency) {
			newNode->left = p;
			newNode->right = q;
		}
		else {
			newNode->left = q;
			newNode->right = p;
		}
		nodeHeap.push(newNode);
	}
	return ;
}

void LlHuffman::CreatLlHfmTree(){
	// ��ȡ�������ȵı�������
	for (int32 i = 0; i < LL_CODENUM; i++) {
		codeLengthCnt[codeLength[i]]++;
	}
	// ��ȡ�����������Huffman����
	GetHfmCode();
	// �½����ڵ�
	root = new TreeNode;
	// ����Huffman��
	for (uint32 i = 0; i < LL_CODENUM; i++) {
		if (codeLength[i]) {
			Node p = root;
			BS code = hfmCode[i];
			for (auto it = code.begin(); it != code.end(); it++) {
				if (*it == '0') {
					if (it + 1 == code.end()) p->left = codeNode[i];
					else if (!p->left)	p->left = new TreeNode;
					p = p->left;
				}
				else {
					if (it + 1 == code.end()) p->right = codeNode[i];
					else if (!p->right)	p->right = new TreeNode;
					p = p->right;
				}
			}
			p->code = i;
		}
	}
	return;
}

void LlHuffman::GetCodeLength(Node p, uint16 l){
	if (p) {
		if (p->code >= 0) {
			codeLength[p->code] = l;
			codeLengthCnt[l] ++;
		}
		GetCodeLength(p->left, l + 1);
		GetCodeLength(p->right, l + 1);
	}
	return;
}

void LlHuffman::GetHfmCode(){
	// ����ÿ�����ȵ���������������ߵı���
	BS code = "0";
	nextHfmCode[1] = "0";
	for (int32 i = 2; i < LL_MAX_CODELENGTH; i++) {
		code = BSAdd(code, IntToBS(codeLengthCnt[i - 1], 0, true));
		BSShiftLeft(code, 1);
		nextHfmCode[i] = code;
	}
	// �õ�ÿ�������������
	for (int32 i = 0; i < LL_CODENUM; i++) {
		if (codeLength[i] > 0) {
			hfmCode[i] = nextHfmCode[codeLength[i]];
			nextHfmCode[codeLength[i]]  = BSAdd(nextHfmCode[codeLength[i]], IntToBS(1, 0, true));
		}
	}
	// debug
	/*cout << "Code Frequency: " << endl;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		cout << i << "-" << codeNode[i]->frequency << endl;
	}*/
	/*cout << "Code HfmCodeBS: " << endl;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		cout << i << "-" << codeLength[i] << "-" << hfmCode[i] << endl;
	}*/
	return;
}

void LlHuffman::ResetHfm(){
	// �������������
	memset(codeLength, 0, sizeof(uint16) * LL_CODENUM);
	memset(codeLengthCnt, 0, sizeof(uint16) * LL_MAX_CODELENGTH);
	// ���Huffman��������
	for (int32 i = 0; i < LL_CODENUM; i++) hfmCode[i] = "";
	for(int32 i = 0; i < LL_MAX_CODELENGTH; i ++) nextHfmCode[i] = "";
	while (!nodeHeap.empty()) nodeHeap.pop();
	// ���Huffman��
	DestroyTree(root);
	root = NULL;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		codeNode[i]->code = i;
		codeNode[i]->frequency = 0;
		codeNode[i]->left = codeNode[i]->right = NULL;
	}
	return;
}

void LlHuffman::DestroyTree(Node p){
	if (p) {
		DestroyTree(p->left);
		DestroyTree(p->right);
		if(p->code < 0) delete p;
	}
	return;
}
