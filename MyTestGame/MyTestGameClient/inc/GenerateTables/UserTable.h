#pragma once				
class UserTable{ 
public:
struct Row{
	wchar_t* m_Name;
	int m_Age;
	float m_Salary;
	__int64 m_Property;
	int m_Gender;
};
	 static const Row m_Row[9];
	 const static int m_Size;
};
const int UserTable::m_Size = 9;
const UserTable::Row UserTable::m_Row[9]={
{NULL,0,0.0f,0,1,},
{L"ƒ„∫√",0,0.0f,0,0,},
{NULL,0,0.0f,0,1,},
{NULL,0,100.2f,0,1,},
{L"Œ‚≈Ù",0,0.0f,0,1,},
{NULL,0,0.0f,0,1,},
{NULL,0,0.0f,1000,1,},
{NULL,0,0.0f,0,1,},
{NULL,1,0.0f,0,1,},
};
