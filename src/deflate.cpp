#include "deflate.h"
Deflate::Deflate()
	:window(new uint8[2 * WSIZE]), lzHash(new HashTable(WSIZE)), 
	disHuffman(new DisHuffman), llHuffman(new LlHuffman){
	fBuff = new uint8[F_BUFFSIZE];	fBuffCnt = 0;
	lBuff = new uint8[L_BUFFSIZE];	lBuffCnt = 0;
	dBuff = new uint16[D_BUFFSIZE];	dBuffCnt = 0;
	oBuff = new uint8[O_BUFFSIZE_D];	oBuffCnt = 0;
	fO = NULL;
	fI = NULL;
}

Deflate::~Deflate() {
	delete[] window;
	delete lzHash;
	delete disHuffman;
	delete llHuffman;
	delete[] fBuff;
	delete[] lBuff;
	delete[] dBuff;
	delete[] oBuff;
}

/*=========================================================
���ܣ�	����ѹ�����������ļ�ͨ��Deflate�㷨ѹ��
������	ͨ�����������е����л�������ȡ���ַ�������������ַ�����
		���ַ�����������ϣ��ͬʱ����ƥ���������õ�ǰ��ͨ����ϣ
		���ƥ���������ƥ�䣬��<���ȣ�����>���滻�ô������Ҳ�
		��ƥ��Ͳ����滻������������һ�ַ������ٽ�ÿ��λ�ö�Ӧ��
		�����Ϣ���Ƿ񱻡�<���ȣ�����>�ԡ��滻�ˣ�д�뵽������...
=========================================================*/
void Deflate::Compress(string fileName, string newFileName) {
	// ���ļ�
	fO = fopen(newFileName.c_str(), "wb");
	assert(fO);
	fI = fopen(fileName.c_str(), "rb");
	assert(fI);

	// ��ȡԭʼ�ļ���С����������ļ�
	fseek(fI, 0, SEEK_END);
	uint64 fileSize = ftell(fI);
	fwrite(&fileSize, sizeof(uint64), 1, fO);
	fseek(fI, 0, SEEK_SET);

	// �ȴ��ļ��ж�ȡ64K���ֽ�
	auto lookAhead = fread(window, 1, WSIZE * 2, fI);

	// ��ʼ����ϣ��ַ
	uint16 hashAdress = 0;
	lzHash->HashFunction(hashAdress, window[0]);
	lzHash->HashFunction(hashAdress, window[1]);

	uint8 fData = 0;			// 8bits�����Ϣ
	uint8 fBitCnt = 0;			// �����Ϣbitλ��
	uint8 oData = 0;			// 8bits�����Ϣ
	uint8 oBitCnt = 0;			// �����Ϣbitλ��
	
	uint16 strStart = 0;		// ��ǰƥ����ҵ�λ��
	uint16 matchHead = 0;		// ��ǰ�ַ���ƥ����ҵ�ͷλ�ã���Ϊ0��û���ҵ�ƥ�䴮
	uint16 tMatchLength = 0;	// ��ǰƥ�䳤��
	uint16 tMatchDistance = 0;	// ��ǰƥ�����

	while (lookAhead) {
		// �������ַ��ַ����е����һ���ַ��Լ����ϣ��ַ
		lzHash->Insert(matchHead, window[strStart + 2], strStart, hashAdress);
		tMatchLength = 0;
		tMatchDistance = 0;
		// ����ȡ��ƥ��ͷ��Ϊ0�������ƥ�����е��ƥ�䳤��
		if (matchHead) {
			tMatchLength = GetMaxMatchLength(matchHead, tMatchDistance, strStart, lookAhead);
		}
		// ��֤�Ƿ��ҵ���ƥ�䴮
		if (tMatchLength >= MIN_MATCHLENGTH) {
			// ��<���ȣ�����>��д�뻺����
			lBuff[lBuffCnt++] = tMatchLength - 3;
			dBuff[dBuffCnt++] = tMatchDistance;
			// ͳ��distance lengthƵ�����ڹ���Huffman��
			disHuffman->AddFrequency(tMatchDistance - 1);
			llHuffman->AddFrequency(tMatchLength - 3, true);
			// д���ǻ�����
			WriteFlag(fData, fBitCnt, true, oData, oBitCnt);
			// �滻�󽫱��滻�Ĵ����������ַ��ַ������뵽��ϣ����
			for (int32 i = 0; i < tMatchLength - 1; i++) {
				strStart++;
				lzHash->Insert(matchHead, window[strStart + 2], strStart, hashAdress);
			}
			strStart++;
			lookAhead -= tMatchLength;
		}
		else {
			// ˵��û���ҵ���Чƥ�䴮�����ַ�ԭ�������ѹ�������ļ���
			lBuff[lBuffCnt++] = window[strStart];
			// ͳ��literalƵ�����ڹ���Huffman��
			llHuffman->AddFrequency(window[strStart], false);
			strStart++;
			lookAhead--;
			// д�����ļ�
			WriteFlag(fData, fBitCnt, false, oData, oBitCnt);
		}
		// �����л������ĳ���С����С����ʱ�����ڻ���
		if (lookAhead <= MIN_LOOKAHEAD) {
			MoveWindow(lookAhead, strStart);
		}
	}

	// �����ı��λ������8bit��ֱ������д�뻺����
	if (fBitCnt) {
		fData <<= (8 - fBitCnt);
		fBuff[fBuffCnt++] = fData;
		fData = 0;
		fBitCnt = 0;
	}
	// д�����һ��
	OutHuffman(oData, oBitCnt);
	if (oBuffCnt) {
		fwrite(oBuff, sizeof(uint8), oBuffCnt, fO);
	}

	fclose(fO);
	return;
}

/*=========================================================
���ܣ�	������ƥ�䴮
����ֵ��	���ƥ�䳤��
������	ͨ��ƥ�������ϲ��ҵõ��ƥ�䳤�ȵ�ƥ�䴮������Ҫͨ����
		��ƥ������Է�ֹ������ѭ��
=========================================================*/
uint16 Deflate::GetMaxMatchLength(uint16 matchHead, uint16& tMatchDistance, uint16 strStart, uint32 lookAhead) {
	uint16 nowMatchLength = 0;				// ÿ��ƥ��ĳ���
	uint16 tMatchStart = 0;					// ��ǰƥ���ڲ��һ������е���ʼλ��
	uint8 maxMatchTimes = MAX_MATCH_TIMES;	// ����ƥ�����
	uint16 maxMatchLenght = 0;				// ���ƥ�䳤��

	while (matchHead > 0 && maxMatchTimes--) {
		uint8* pStart = window + strStart;	
		uint8* pEnd = pStart + ((lookAhead <= MAX_MATCHLENGTH) ? (lookAhead - 1) : MAX_MATCHLENGTH);
		uint8* pMatchStart = window + matchHead;
		nowMatchLength = 0;
		// ̫Զ��ƥ�����
		if (strStart - matchHead > WSIZE - MIN_LOOKAHEAD) {
			break;
		}
		// ���ַ�����ƥ��
		while (pStart < pEnd && *pStart == *pMatchStart) {
			pStart++;
			pMatchStart++;
			nowMatchLength++;
		}
		// һ��ƥ������ˣ��жϲ���¼�ƥ��
		if (nowMatchLength > maxMatchLenght) {
			maxMatchLenght = nowMatchLength;
			tMatchStart = matchHead;
		}
		// ƥ��������һ��ƥ��λ��
		matchHead = lzHash->prev[matchHead & HASHMASK];
	}

	// ��������ƥ���Ӧ�ľ���
	tMatchDistance = strStart - tMatchStart;
	return maxMatchLenght;
}

/*=========================================================
���ܣ�	��������
����ֵ��	void
������	��������������ƶ�32K
=========================================================*/
void Deflate::MoveWindow(size_t& lookAhead, uint16& strStart) {
	// ���ж��Ƿ񵽴����ļ�ĩ��
	if (strStart >= WSIZE) {
		// ���Ҵ��е������������Ҵ���0
		memcpy(window, window + WSIZE, WSIZE);
		memset(window + WSIZE, 0, WSIZE);
		strStart -= WSIZE;
		// ���¹�ϣ��
		lzHash->Updata();
		// ���Ҵ�����ǰ����������������
		if (!feof(fI)) {
			lookAhead += fread(window + WSIZE, 1, WSIZE, fI);
		}
	}
}


/*=========================================================
���ܣ�	д������Ϣ
����ֵ��	void
������	ͨ���ж��ַ��Ƿ�<���ȣ�����>���滻�����򻺳���fBuffд���
		����Ϣ����0��ʾδ���滻����1��ʾ�ѱ��滻
=========================================================*/
void Deflate::WriteFlag(uint8& fData, uint8& fBitCnt, bool hasEncode, uint8& oData, uint8& oBitCnt) {
	// ������һλ����Ϊlength�����¼1
	fData <<= 1;
	if (hasEncode)	fData |= 1;
	fBitCnt++;
	// �������8bitʱ����fDataд�뻺����
	if (fBitCnt == 8) {
		fBuff[fBuffCnt++] = fData;
		fData = 0;
		fBitCnt = 0;
	}
	// ����ÿ�������fBuff��С�ﵽ8k����lBuff��С�ﵽ64k��
	if (fBuffCnt == F_BUFFSIZE) {
		OutHuffman(oData, oBitCnt);
	}
	return ;
}

/*=========================================================
���ܣ�	���Huffman�����ļ���������
����ֵ��	void
������	���ÿ����Ϣд�뻺�����������Huffman�������
=========================================================*/
void Deflate::OutHuffman(uint8& oData, uint8& oBitCnt){
	// ��256��������
	llHuffman->codeNode[256]->frequency++;
	// �õ�distance����lenght/literal����Huffman����
	disHuffman->CreatNormalTree();
	disHuffman->GetCodeLength(disHuffman->root, 0);
	disHuffman->GetHfmCode();
	llHuffman->CreatNormalTree();
	llHuffman->GetCodeLength(llHuffman->root, 0);
	llHuffman->GetHfmCode();

	/*double avg = 0, fre = 0;
	for (int32 i = 0; i < D_CODENUM; i++) {
		avg +=((double)disHuffman->codeLength[i] + (double)disHuffman->codeExtraBits[i]) * disHuffman->codeNode[i]->frequency;
		fre += (double)disHuffman->codeNode[i]->frequency;
	}
	avg /= fre;
	cout <<  "distance tree: " << avg << endl;*/

	/*avg = 0, fre = 0;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		if (i >= 257) {
			avg += ((double)llHuffman->codeLength[i] + (double)llHuffman->codeExtraBits[i - 257]) * llHuffman->codeNode[i]->frequency;
		}
		else {
			avg += (double)llHuffman->codeLength[i]  * llHuffman->codeNode[i]->frequency;
		}
		fre += (double)llHuffman->codeNode[i]->frequency;
	}
	avg /= fre;
	cout << "l tree: " << avg << endl;*/

	// д�뽨��Huffman������Ҫ����Ϣ
	fwrite(llHuffman->codeLength, sizeof(uint16), LL_CODENUM, fO);
	fwrite(disHuffman->codeLength, sizeof(uint16), D_CODENUM, fO);
	// ѭ��flag��ÿһλ����Huffman����д�������������
	uint32 ll = 0, d = 0, cnt = 0;
	for (uint32 i = 0; i < fBuffCnt; i++) {
		uint8 f = fBuff[i];
		for (uint32 j = 0; j < 8; j++) {
			if (f & 0x80) {
				// ��length�ı���д��
				uint8 length = lBuff[ll++];
				uint16 lengthToCode = llHuffman->lToCode[length];
				BS lCode = llHuffman->hfmCode[257 + lengthToCode];
				for (auto it = lCode.begin(); it != lCode.end(); it++) {
					if (*it == '1') OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
				}
				uint32 extra = length - llHuffman->codeBeginLl[lengthToCode];
				for (int32 k = 0; k < llHuffman->codeExtraBits[lengthToCode]; k++) {
					if (extra & 1) OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
					extra >>= 1;
				}
				// ������д��distance�ı���
				uint16 distance = dBuff[d++];
				uint16 distanceToCode = (distance - 1 < 256) ? disHuffman->disToCode[distance - 1] : disHuffman->disToCode[256 + ((distance - 1) >> 7)];
				BS dCode = disHuffman->hfmCode[distanceToCode];
				for (auto it = dCode.begin(); it != dCode.end(); it++) {
					if (*it == '1') OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
				}
				extra = distance - disHuffman->codeBeginDis[distanceToCode];
				for (int32 k = 0; k < disHuffman->codeExtraBits[distanceToCode]; k++) {
					if (extra & 1) OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
					extra >>= 1;
				}
			}
			else {
				// ��literal����д��
				uint8 literal = lBuff[ll ++];
				BS lCode = llHuffman->hfmCode[literal];
				for (auto it = lCode.begin(); it != lCode.end(); it++) {
					if (*it == '1') OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
				}
			}
			// ��ֹ���һ���fBuff�����һ���ֽ�δ��8�ֽھ����룬������Ҫ��lBuffΪ׼����
			cnt++;
			if (cnt == lBuffCnt) break;
			f <<= 1;
		}
	}
	// д��256��ʾ�ÿ�Ľ���
	BS endCode = llHuffman->hfmCode[256];
	for (auto it = endCode.begin(); it != endCode.end(); it++) {
		if (*it == '1') OutBit(oData, oBitCnt, 1);
		else OutBit(oData, oBitCnt, 0);
	}
	// ��������8λ��Ҳֱ��д��
	if (oBitCnt) {
		oData <<= (8 - oBitCnt);
		oBuff[oBuffCnt++] = oData;
		oData = 0;
		oBitCnt = 0;
	}
	if (oBuffCnt) {
		fwrite(oBuff, sizeof(uint8), oBuffCnt, fO);
		oBuffCnt = 0;
	}
	// ����Huffman��
	disHuffman->ResetHfm();
	llHuffman->ResetHfm();
	// ���û�����
	fBuffCnt = 0;
	dBuffCnt = 0;
	lBuffCnt = 0;
	return;
}

/*=========================================================
���ܣ�	���һ��bit��������
����ֵ��	void
������	���һ��bitλ�����������������8λʱ���һ���ֽ�
=========================================================*/
void Deflate::OutBit(uint8& oData, uint8& oBitCnt, bool bit){
	oData <<= 1;
	if (bit) oData |= 1;
	oBitCnt++;
	if (oBitCnt == 8) {
		oBuff[oBuffCnt++] = oData;
		if (oBuffCnt == O_BUFFSIZE_D) {
			fwrite(oBuff, sizeof(uint8), O_BUFFSIZE_D, fO);
			oBuffCnt = 0;
		}
		oData = 0;
		oBitCnt = 0;
	}
	return;
}
