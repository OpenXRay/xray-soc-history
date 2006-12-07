#pragma once

struct SCallbackInfo;
/*
namespace boost {
	template<typename Signature, typename Allocator>
	class function;
};
*/
class CUIWindow;

class CUIWndCallback
{
public:
	typedef fastdelegate::FastDelegate2<CUIWindow*,void*,void>				void_function;
private:
//.	typedef boost::function<void(CUIWindow*,void*),std::allocator<void> >	void_function;
	typedef xr_vector<SCallbackInfo*>	CALLBACKS;
	typedef CALLBACKS::iterator			CALLBACK_IT;
private:
			CALLBACKS			m_callbacks;
			SCallbackInfo*		NewCallback			();


public:
	virtual						~CUIWndCallback		();
	virtual void				OnEvent				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void				Register			(CUIWindow* pChild);
			void				AddCallback			(LPCSTR control_id, s16 event, const void_function &f);
			void				AddCallback			(const shared_str& control_id, s16 event, const void_function &f);
};