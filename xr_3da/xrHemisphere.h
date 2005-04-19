#ifndef xrHemisphereH
#define xrHemisphereH

typedef void __stdcall		xrHemisphereIterator(float x, float y, float z, float energy, LPVOID param);

void	ECORE_API xrHemisphereBuild	(int quality, float energy, xrHemisphereIterator* it, LPVOID param);

#endif //xrHemisphereH
