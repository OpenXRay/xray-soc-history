///////////////////////////////////////////////////////////////
// map_location.h
// ��������� ��� ������� �� �����
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/uistring.h"
#include "infoportiondefs.h"


enum EMapLocationFlags
{
	eMapLocationPDAContact		= 0x01,
	eMapLocationInfoPortion		= 0x02,
	eMapLocationScript			= 0x04
};



struct SMapLocation
{
	SMapLocation();

	ref_str LevelName();

	//���� �������
	flags32 type_flags;

	//��� ������
	ref_str level_name;
	//���������� �� ������
	float x;
	float y;
	//��� �������
	ref_str name;
	//����� ��������
	ref_str text;

	//������������ �� ������� � �������
	bool attached_to_object;
	//id ������� �� ������
	u16 object_id;
	//������ ����� ����������
	INFO_INDEX info_portion_id;

	//������� � ��������� ������
	int icon_x, icon_y, icon_width, icon_height;
	
	//����� �� ���������� ������ (������� �� ���� �����)
	bool marker_show;
	//���� ������� ������� � ������ �� ���������
	u32 icon_color;
};

DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);