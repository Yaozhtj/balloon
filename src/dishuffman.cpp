#include "dishuffman.h"
#include <bitset>

DisHuffman::DisHuffman(){
	// ��ʼ����������distance�Ĺ�ϵ
	disToCode = new uint8[512]{
		 0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
		 8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
		11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
		12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
		13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
		13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
		18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
		23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
		26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
		26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
		27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
		27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
	};
	// ��ʼ��extrabits(��չλ)
	codeExtraBits = new uint16[D_CODENUM]{ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };
	// ��ʼ��codeBeginDis,ÿ�������뿪ʼ��distanceֵ
	codeBeginDis = new uint16[D_CODENUM]{ 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577 };
	// ��ʼ��codeNode
	codeNode = new Node[D_CODENUM];
	for (int32 i = 0; i < D_CODENUM; i++) {
		codeNode[i] = new TreeNode;
		codeNode[i]->code = i;
	}
	// ��ʼ��codeLength
	codeLength = new uint16[D_CODENUM];
	for (int32 i = 0; i < D_CODENUM; i++) codeLength[i] = 0;
	// ��ʼ��codeLengthCnt
	codeLengthCnt = new uint16[D_MAX_CODELENGTH];
	for (int32 i = 0; i < D_MAX_CODELENGTH; i++) codeLengthCnt[i] = 0;
	// ��ʼ��hfmCodeInt
	hfmCodeInt = new uint32[D_CODENUM];
	for (int32 i = 0; i < D_CODENUM; i++) hfmCodeInt[i] = 0;
	// ��ʼ��hfmCode
	hfmCode = new BS[D_CODENUM];
	// ��ʼ��nextHfmCodeInt
	nextHfmCodeInt = new uint32[D_MAX_CODELENGTH];
	for (int32 i = 0; i < D_MAX_CODELENGTH; i++) nextHfmCodeInt[i] = 0;
	// ��ʼ�������ڵ�
	root = NULL;
}

DisHuffman::~DisHuffman(){
	delete[] disToCode;
	delete[] codeBeginDis;
	delete[] codeExtraBits;
	delete[] codeLength;
	for (int32 i = 0; i < D_CODENUM; i++) delete codeNode[i];
	delete[] codeNode;
	delete[] codeLengthCnt;
	delete[] hfmCode;
	delete[] nextHfmCodeInt;
	delete[] hfmCodeInt;
}

void DisHuffman::AddFrequency(uint16 d){
	// ����distanceΪ���Ӧ�����������Ƶ��
	codeNode[(d < 256) ? disToCode[d] : disToCode[256 + (d >> 7)]]->frequency++;
	return;
}

void DisHuffman::CreatNormalTree(){
	// ���������Ӧ�Ľڵ���뵽���ȶ���
	for (int32 i = 0; i < D_CODENUM; i++) {
		if(codeNode[i]->frequency > 0)
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

	return;
}

void DisHuffman::CreatDisHfmTree(){
	// ��ȡ�������ȵı�������
	for (int32 i = 0; i < D_CODENUM; i++) {
		codeLengthCnt[codeLength[i]]++;
	}
	// ��ȡ�����������Huffman����
	GetHfmCode();
	// �½����ڵ�
	root = new TreeNode;
	// ����Huffman��
	for (uint32 i = 0; i < D_CODENUM; i++) {
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
		}
	}
	return;
}

void DisHuffman::GetCodeLength(Node p, uint16 l){
	if (p) {
		if (p->code >= 0) {
			codeLength[p->code] = l;
			codeLengthCnt[l] ++;
		}
		GetCodeLength(p->left, l + 1);
		GetCodeLength(p->right, l + 1);
	}
	return ;
}

void DisHuffman::GetHfmCode(){
	// ����ÿ�����ȵ���������������ߵı���
	int32 code = 0;
	for (int32 i = 1; i < D_MAX_CODELENGTH; i++) {
		code = (code + codeLengthCnt[i - 1]) << 1;
		nextHfmCodeInt[i] = code;
	}
	// �õ�ÿ�������������
	for (int32 i = 0; i < D_CODENUM;  i++) {
		if (codeLength[i] > 0) {
			hfmCodeInt[i] = nextHfmCodeInt[codeLength[i]];
			nextHfmCodeInt[codeLength[i]] ++;
		}
	}
	// �õ�ÿ����������ַ���������
	for (int32 i = 0; i < D_CODENUM; i ++) {
		hfmCode[i]=IntToBS(hfmCodeInt[i], codeLength[i], false);
	}

	// debug
	/*cout << "Code Frequency: " << endl;
	for (int32 i = 0; i < D_CODENUM; i++) {
		cout << i << "-" << codeNode[i]->frequency << endl;
	}
	cout << "Code HfmCodeInt: " << endl;
	for (int32 i = 0; i < D_CODENUM; i++) {
		cout << i << "-" << codeLength[i] << "-" << bitset<sizeof(uint32) * 8>(hfmCodeInt[i]) << endl;
	}*/
	/*cout << "Code HfmCodeBS: " << endl;
	for (int32 i = 0; i < D_CODENUM; i++) {
		cout << i << "-" << codeLength[i] << "-" << hfmCode[i] << endl;
	}*/
	return;
}

void DisHuffman::ResetHfm(){
	// �������������
	memset(codeLength, 0, sizeof(uint16) * D_CODENUM);
	memset(codeLengthCnt, 0, sizeof(uint16) * D_MAX_CODELENGTH);
	// ���Huffman��������
	for (int32 i = 0; i < D_CODENUM; i++) hfmCode[i] = "";
	memset(hfmCodeInt, 0, sizeof(uint32) * D_CODENUM);
	memset(nextHfmCodeInt, 0, sizeof(uint32) * D_MAX_CODELENGTH);
	while (!nodeHeap.empty()) nodeHeap.pop();
	// ���Huffman��
	DestroyTree(root);
	root = NULL;
	for (int32 i = 0; i < D_CODENUM; i++) {
		codeNode[i]->frequency = 0;
		codeNode[i]->code = i;
		codeNode[i]->left = codeNode[i]->right = NULL;
	}
	return;
}

void DisHuffman::DestroyTree(Node p){
	if (p) {
		DestroyTree(p->left);
		DestroyTree(p->right);
		if(p->code < 0) delete p;
	}
	return;
}
