//---------------------------------------------------------------------------
#ifndef ShaderToolsH
#define ShaderToolsH

// refs
class CEditableObject;
class CLibObject;
class CBlender;

struct str_pred : public binary_function<char*, char*, bool>
{
    IC bool operator()(LPCSTR x, LPCSTR y) const
    {	return strcmp(x,y)<0;	}
};
DEFINE_VECTOR(CBlender*,TemplateVec,TemplateIt);
DEFINE_MAP_PRED(LPSTR,CConstant*,ConstantMap,ConstantPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CMatrix*,MatrixMap,MatrixPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CBlender*,BlenderMap,BlenderPairIt,str_pred);

class CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data)=0;
};

class CShaderTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CLibObject*			m_LibObject;
	CEditableObject*	m_EditObject;
	BOOL				m_bModified;

	TemplateVec			m_TemplatePalette;

	ConstantMap			m_OptConstants;
	MatrixMap			m_OptMatrices;
	ConstantMap			m_Constants;
	MatrixMap			m_Matrices;
	BlenderMap			m_Blenders;

	CBlender*			FindBlender			(LPCSTR name);
    LPCSTR				GenerateBlenderName	(LPSTR name, LPCSTR source);

	void 				AddMatrixRef		(LPSTR name);
	CMatrix*			FindMatrix			(LPSTR name, bool bDuplicate);
    LPCSTR				GenerateMatrixName	(LPSTR name);
    LPCSTR				AppendMatrix		(CMatrix* src=0, CMatrix** dest=0);
    void				RemoveMatrix		(LPSTR name);

	void 				AddConstantRef		(LPSTR name);
	CConstant*			FindConstant		(LPSTR name, bool bDuplicate);
    LPCSTR				GenerateConstantName(LPSTR name);
    LPCSTR				AppendConstant		(CConstant* src=0, CConstant** dest=0);
    void				RemoveConstant		(LPSTR name);

friend class CCollapseBlender;
friend class CRefsBlender;
friend class CRemoveBlender;
friend class TfrmShaderProperties;
    void				CollapseMatrix		(LPSTR name);
    void				CollapseConstant	(LPSTR name);
    void				CollapseReferences	();
    void				UpdateMatrixRefs	(LPSTR name);
    void				UpdateConstantRefs	(LPSTR name);
    void				UpdateRefCounters	();

    void 				ParseBlender		(CBlender* B, CParseBlender& P);

	CFS_Memory 			m_BlenderStream;	// ������������ ��������� ���������� ������ ��� �������������
    bool 				m_bUpdateCurrent;	// ���� ������� ������ ���������������  Update____From___()
    bool				m_bCurBlenderChanged;
public:
    CBlender*			m_CurrentBlender;
    CBlender*			AppendBlender		(CLASS_ID cls_id, LPCSTR folder_name, CBlender* parent);
    CBlender* 			CloneBlender		(LPCSTR name);
    void				RemoveBlender		(LPCSTR name);
	void				RenameBlender		(LPCSTR old_full_name, LPCSTR ren_part, int level);

    void				ResetCurrentBlender();
    void				UpdateStreamFromObject();
    void				UpdateObjectFromStream();

    void 				ClearData			();
public:
						CShaderTools		();
    virtual 			~CShaderTools		();

	void				Reload				();
	void				Load				();
	void				Save				();

    void				Render				();
    void				Update				();

    bool				IfModified			();
    bool				IsModified			(){return m_bModified;}
    void				Modified			();

    void				OnCreate			();
    void				OnDestroy			();

    void				ZoomObject			();

    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    void				SelectPreviewObject	(int p);
    void				ResetPreviewObject	();

    // misc
    void				SetCurrentBlender	(CBlender* B);
    void				SetCurrentBlender	(LPCSTR name);
    void				ApplyChanges		();
};
extern CShaderTools	SHTools;
//---------------------------------------------------------------------------
#endif
