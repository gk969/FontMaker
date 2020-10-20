#include "StdAfx.h"
#include "Charset.h"

CCharset::CCharset(void)
{
	m_pTable = NULL;
	m_nCount = 0;
}

CCharset::~CCharset(void)
{
}

//ANSI -> UNICODE
static WCHAR ansi_to_unicode(UINT code_page, CHAR* s)
{
	WCHAR output[8];
	MultiByteToWideChar(code_page, MB_USEGLYPHCHARS | MB_PRECOMPOSED, (LPCSTR)s, -1, output, 2);
	return output[0];
}

#define ASCII_CODE_PAGE     28591
#define ASCII_BYTE_BEGIN    0x20
#define ASCII_BYTE_END      0x7E
#define ASCII_TOTAL_COUNT	(ASCII_BYTE_END-ASCII_BYTE_BEGIN+1)

BOOL CCharset::fillAsciiCharset() {
	FILE* fp;
	fp = fopen("ascii.cst", "wb+");
	if (fp == NULL)
	{
		return FALSE;
	}
	int len = ASCII_TOTAL_COUNT * 2;
	m_pTable = (WCHAR*)malloc(len + 2);
	if (m_pTable == NULL)
	{
		return FALSE;
	}
	ZeroMemory(m_pTable, len + 2);

	CHAR s[4];
	WCHAR c;
	memset(s, 0, sizeof(s));
	int ofs = 0;
	for (int i = ASCII_BYTE_BEGIN; i <= ASCII_BYTE_END; i++)
	{
		s[0] = i;
		fwrite(s, 1, 1, fp);
		c = ansi_to_unicode(ASCII_CODE_PAGE, s);
		m_pTable[ofs] = c;
		ofs++;
	}

	fclose(fp);
	m_nCount = len / 2;

	return true;
}

#define GB2312_CODE_PAGE     936
#define GB2312_BYTE1_P1_BEGIN   0xA1
#define GB2312_BYTE1_P1_END     0xA9
#define GB2312_BYTE1_P2_BEGIN   0xB0
#define GB2312_BYTE1_P2_END     0xF7
#define GB2312_BYTE2_BEGIN   0xA1
#define GB2312_BYTE2_END     0xFE
#define GB2312_TOTAL_COUNT	 (GB2312_BYTE1_P1_END-GB2312_BYTE1_P1_BEGIN+1+GB2312_BYTE1_P2_END-GB2312_BYTE1_P2_BEGIN+1)*(GB2312_BYTE2_END-GB2312_BYTE2_BEGIN+1)

BOOL CCharset::fillGb2312Charset() {
	FILE* fp;
	fp = fopen("gb2312.cst", "wb+");
	if (fp == NULL)
	{
		return FALSE;
	}

	int len = GB2312_TOTAL_COUNT * 2;
	m_pTable = (WCHAR*)malloc(len + 2);
	if (m_pTable == NULL)
	{
		return FALSE;
	}
	ZeroMemory(m_pTable, len + 2);

	BYTE h, l;
	CHAR s[4];
	WCHAR c;
	memset(s, 0, sizeof(s));
	int ofs = 0;
	for (h = GB2312_BYTE1_P1_BEGIN; h <= GB2312_BYTE1_P2_END; h++)
	{
		if (h > GB2312_BYTE1_P1_END && h < GB2312_BYTE1_P2_BEGIN) {
			continue;
		}

		for (l = GB2312_BYTE2_BEGIN; l <= GB2312_BYTE2_END; l++)
		{
			s[0] = h;
			s[1] = l;
			fwrite(s, 1, 2, fp);
			c = ansi_to_unicode(GB2312_CODE_PAGE, s);
			m_pTable[ofs] = c;
			ofs++;
		}
	}


	fclose(fp);
	m_nCount = len / 2;

	return true;
}


#define GBK_CODE_PAGE     936
#define GBK_BYTE1_BEGIN   0x81
#define GBK_BYTE1_END     0xFE

#define GBK_BYTE2_BEGIN   0x40
#define GBK_BYTE2_END     0xFE

#define GBK_BYTE2_SKIP    0x7F //GBK要跳过xx7F一条线
#define GBK_TOTAL_COUNT   (GBK_BYTE1_END-GBK_BYTE1_BEGIN+1)*(GBK_BYTE2_END-GBK_BYTE2_BEGIN)

BOOL CCharset::fillGbkCharset() {
	FILE* fp;
	fp = fopen("gbk.cst", "wb+");
	if (fp == NULL)
	{
		return FALSE;
	}

	int len = GBK_TOTAL_COUNT*2;
	m_pTable = (WCHAR*)malloc(len + 2);
	if (m_pTable == NULL)
	{
		return FALSE;
	}
	ZeroMemory(m_pTable, len + 2);
	
	BYTE h, l;
	CHAR s[4];
	WCHAR c;
	memset(s, 0, sizeof(s));
	int ofs = 0;
	for (h = GBK_BYTE1_BEGIN; h <= GBK_BYTE1_END; h++)
	{
		for (l = GBK_BYTE2_BEGIN; l <= GBK_BYTE2_END; l++)
		{
			if (l != GBK_BYTE2_SKIP)
			{
				s[0] = h;
				s[1] = l;
				fwrite(s, 1, 2, fp);
				c = ansi_to_unicode(GBK_CODE_PAGE, s);
				m_pTable[ofs] = c;
				ofs++;
			}
		}
	}

	fclose(fp);
	m_nCount = len / 2;

	return true;
}

//从文件加载
BOOL CCharset::LoadFromFile(LPCTSTR pFileName)
{
	USES_CONVERSION;
	char* name = W2A(pFileName);
	if (strcmp(name, "GBK") == 0) {
		return fillGbkCharset();
	}
	else if (strcmp(name, "GB2312") == 0) {
		return fillGb2312Charset();
	}
	else if (strcmp(name, "ASCII") == 0) {
		return fillAsciiCharset();
	}
	return FALSE;
}

BOOL CCharset::Create(LPCTSTR pString)
{
	m_nCount = (UINT)wcslen(pString);
	m_pTable = _wcsdup(pString);
	return FALSE;
}

void CCharset::Delete()
{
	free(m_pTable);
	m_pTable = NULL;
	m_nCount = 0;
}

WCHAR CCharset::GetChar(UINT index)
{
	if(m_pTable == NULL)
	{
		return L'?';
	}
	if(index < m_nCount)
	{
		return *(m_pTable+index);
	}
	return L'?';
}

UINT CCharset::GetCharCount()
{
	return m_nCount;
}
