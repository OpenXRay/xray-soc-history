#ifndef _BRAIN_H
#define _BRAIN_H
namespace IQ {

// Target class
enum TCLASS
{
	TCLASS_Primary	=0,
	TCLASS_Secondary=1,
	TCLASS_Optional	=2
};

// Main interface (note <*> means 'controlled' )
struct IQ_Brain 
{
	float		EyeFOV;			// <*> field-of-view
	float		EyeRange;		// <*> smaller - better performance

	Object*		Target;			// <*> selected target or NULL (default)
	Vector		TargetPoint;	// <*> for directed movements
	int			TargetGroup;	// group of target
	float		TargetCost;		// distance to target (only if target valid)

	int			NaviMode;		// <*> Navigation mode
	float		NaviPrediction;	// <*> 0..1  - coeff. of prediction of target movement
	float		NaviSpeed;		// <*> speed of movement
	float		NaviJumpPower;
	Vector		NaviDir;		// <*> direction of model. Only in NAVI_STAY mode

	float		AuralPerception;// <*> 0..1  - coeff. of aural perception

	// Subclasses
	IQ_Timer	Timers	[6];
	IQ_Visible	Visible	[4];	// 4 groups max (primary,secondary,optional)
};

};
#endif