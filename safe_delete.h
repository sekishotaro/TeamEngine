#pragma once

template <class T>
inline void safe_delete(T*& p)
{
	//�v�f�̍폜
	delete p;
	p = nullptr;
}
