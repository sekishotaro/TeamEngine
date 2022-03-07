#pragma once

template <class T>
inline void safe_delete(T*& p)
{
	//—v‘f‚Ìíœ
	delete p;
	p = nullptr;
}
