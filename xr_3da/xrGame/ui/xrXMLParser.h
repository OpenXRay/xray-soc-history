#ifndef xrXMLParserH
#define xrXMLParserH
#pragma once

#ifdef XRXMLPARSER_EXPORTS
	#define XRXMLPARSER_API __declspec(dllexport)
#else
	#define XRXMLPARSER_API __declspec(dllimport)
	#pragma comment(lib,"x:\\xrXMLParser.lib")
#endif


struct XRXMLPARSER_API StructSampleExport
{
};

XRXMLPARSER_API void FunctionSampleExport();


// UIXml.h: interface for the CUIXml class.
//
//
// ��� ������ ���������� �� XML �����, ��������� Chilkat XML
//
//////////////////////////////////////////////////////////////////////

#include "CkXml.h"
#include "CkString.h"
#include "CkSettings.h"

#pragma once


typedef CkXml XML_NODE;


class XRXMLPARSER_API CUIXml  
{

public:
	CUIXml();
	virtual ~CUIXml();

	bool Init(LPCSTR path, const char* xml_filename);
	
	char* Read(const char *path, int index,  const char*  default_str_val = "" );
	char* Read(XML_NODE* node,  const char*  default_str_val = "" );
	
	int   ReadInt(const char *path, int index,  int default_int_val = 0);
	int   ReadInt(XML_NODE* node,  int default_int_val = 0);

	char* ReadAttrib(const char *path,  int index, 
						const char *attrib, const char*  default_str_val = "");
	char* ReadAttrib(XML_NODE* node,
						const char *attrib, const char*  default_str_val = "");

	int   ReadAttribInt(const char *path, int index,  
							const char *attrib, int default_int_val = 0);
	int   ReadAttribInt(XML_NODE* node,
							const char *attrib, int default_int_val = 0);



	//���������� ���������� ����� � ������� ������
	int GetNodesNum(const char *path, int index, const char* tag_name);
	
	
	//������������� �� XML ������ 
	//���� �������� � ����� PARENT:CHILD:CHIDLS_CHILD
	//node_index - �����, ���� ����� � ����� ������ ���������
	XML_NODE* NavigateToNode(const char* path, int node_index = 0);


protected:
	XML_NODE m_root;
};

#endif //xrXMLParserH