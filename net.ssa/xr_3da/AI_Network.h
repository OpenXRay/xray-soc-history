#pragma once

class ENGINE_API CAI_Network
{
private:
	BYTE*	matrix;
	int		size;
	int		pitch;
public:
	CAI_Network() : matrix(0) {	}
	~CAI_Network() {
		delete [] matrix;
	}

	void	INIT_by_count	(int count)
	{
		pitch	= (count%8)?(count/8 + 1):(count/8);
		size	= pitch*count; // x*y
		delete [] matrix;
		matrix = new BYTE[size];
	}
	IC void	set_1(int a, int b)
	{
		BYTE* P		= matrix+((a/8)+b*pitch);
		int shift	= (a%8);
		*P			|= (1<<shift);
	}
	IC void	set_0(int a, int b)
	{
		BYTE* P		= matrix+((a/8)+b*pitch);
		int shift	= (a%8);
		*P			&= ~(1<<shift);
	}
	IC void	set(int a, int b, BOOL value)
	{
		BYTE* P		= matrix+((a/8)+b*pitch);
		int shift	= (a%8);
		*P			= (*P)&(~(1<<shift)) | (value<<shift);
	}
	IC BOOL	get(int a, int b)
	{
		BYTE* P		= matrix+((a/8)+b*pitch);
		int shift	= (a%8);
		return (*P)&(1<<shift);
	}
	IC void	mark_connected(int a,int b)
	{
		set_1(a,b); set_1(b,a);
	}
	IC void mark_disconnected(int a, int b)
	{
		set_0(a,b); set_0(b,a);
	}
	IC DWORD mem_size()		{ return size;		}
	IC BYTE* mem_pointer()	{ return matrix;	}
};
