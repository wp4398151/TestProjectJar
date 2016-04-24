#pragma once				
class RoleTable{ 
public:
struct Row{
	wchar_t* m_TextureFile;
	int m_StartPosX;
	int m_StartPosY;
	int m_Speed;
};
	 static const Row m_Row[1];
	 const static int m_Size;
};
const int RoleTable::m_Size = 1;
const RoleTable::Row RoleTable::m_Row[1]={
{L"SingleTile.jpg",0,0,100,},
};
