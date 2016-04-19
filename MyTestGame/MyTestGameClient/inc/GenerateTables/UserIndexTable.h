#pragma once				
class UserIndexTable{ 
public:
static const void* s_pColounm[5];
	static const int m_1[9];
	static const int m_2[9];
	static const int m_3[9];
	static const int m_4[9];
	static const int m_5[9];
	 const static int m_Size;
	 template<typename T>
	 static bool GetCell(int x, int y, T& res){
if (ISINRANGE(x, 0, 8) && ISINRANGE(y, 0, 4)){
res = ((T*)s_pColounm[y])[x];
return true;}else{
return false; }
}
};
const int UserIndexTable::m_Size = 9;
const void* UserIndexTable::s_pColounm[5]={(void*)m_1, (void*)m_2, (void*)m_3, (void*)m_4, (void*)m_5, };
const int UserIndexTable::m_1[9] = {0, 0, 0, 1, 1, 0, 0, 0, 1, };
const int UserIndexTable::m_2[9] = {1, 1, 0, 0, 1, 0, 1, 0, 1, };
const int UserIndexTable::m_3[9] = {1, 1, 1, 0, 1, 0, 1, 0, 1, };
const int UserIndexTable::m_4[9] = {1, 1, 1, 0, 1, 0, 1, 0, 0, };
const int UserIndexTable::m_5[9] = {1, 1, 1, 0, 0, 0, 1, 1, 1, };
