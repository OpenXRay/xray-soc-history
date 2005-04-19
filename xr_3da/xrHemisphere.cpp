#include "stdafx.h"
#pragma hdrstop

#include "xrHemisphere.h"

#define HEMI1_LIGHTS	26
#define HEMI2_LIGHTS	91
#define HEMI3_LIGHTS	196

const double hemi_1[HEMI1_LIGHTS][3] = 
{
	{0.00000,	1.00000,	0.00000	},
	{0.52573,	0.85065,	0.00000	},
	{0.16246,	0.85065,	0.50000	},
	{-0.42533,	0.85065,	0.30902	},
	{-0.42533,	0.85065,	-0.30902},
	{0.16246,	0.85065,	-0.50000},
	{0.89443,	0.44721,	0.00000	},
	{0.27639,	0.44721,	0.85065	},
	{-0.72361,	0.44721,	0.52573	},
	{-0.72361,	0.44721,	-0.52573},
	{0.27639,	0.44721,	-0.85065},
	{0.68819,	0.52573,	0.50000	},
	{-0.26287,	0.52573,	0.80902	},
	{-0.85065,	0.52573,	-0.00000},
	{-0.26287,	0.52573,	-0.80902},
	{0.68819,	0.52573,	-0.50000},
	{0.95106,	0.00000,	0.30902	},
	{0.58779,	0.00000,	0.80902	},
	{-0.00000,	0.00000,	1.00000	},
	{-0.58779,	0.00000,	0.80902	},
	{-0.95106,	0.00000,	0.30902	},
	{-0.95106,	0.00000,	-0.30902},
	{-0.58779,	0.00000,	-0.80902},
	{0.00000,	0.00000,	-1.00000},
	{0.58779,	0.00000,	-0.80902},
	{0.95106,	0.00000,	-0.30902}
};
const double hemi_2[HEMI2_LIGHTS][3] = 
{
	{0.00000,	0.00000,	1.00000},
	{0.52573,	0.00000,	0.85065},
	{0.16246,	0.50000,	0.85065},
	{-0.42533,	0.30902,	0.85065},
	{-0.42533,	-0.30902,	0.85065},
	{0.16246,	-0.50000,	0.85065},
	{0.89443,	0.00000,	0.44721},
	{0.27639,	0.85065,	0.44721},
	{-0.72361,	0.52573,	0.44721},
	{-0.72361,	-0.52573,	0.44721},
	{0.27639,	-0.85065,	0.44721},
	{0.68819,	0.50000	,	0.52573},
	{-0.26287,	0.80902	,	0.52573},
	{-0.85065,	-0.00000,	0.52573},
	{-0.26287,	-0.80902,	0.52573},
	{0.68819,	-0.50000,	0.52573},
	{0.95106,	0.30902	,	0.00000},
	{0.58779,	0.80902	,	0.00000},
	{-0.00000,	1.00000	,	0.00000},
	{-0.58779,	0.80902	,	0.00000},
	{-0.95106,	0.30902	,	0.00000},
	{-0.95106,	-0.30902,	0.00000},
	{-0.58779,	-0.80902,	0.00000},
	{0.00000,	-1.00000,	0.00000},
	{0.58779,	-0.80902,	0.00000},
	{0.95106,	-0.30902,	0.00000},
	{0.27327,	0.00000	,	0.96194},
	{0.08444,	0.25989	,	0.96194},
	{-0.22108,	0.16062	,	0.96194},
	{-0.22108,	-0.16062,	0.96194},
	{0.08444,	-0.25989,	0.96194},
	{0.36180,	0.26287	,	0.89443},
	{-0.13820,	0.42533	,	0.89443},
	{-0.44721,	-0.00000,	0.89443},
	{-0.13820,	-0.42533,	0.89443},
	{0.36180,	-0.26287,	0.89443},
	{0.73818,	0.00000	,	0.67461},
	{0.22811,	0.70205	,	0.67461},
	{-0.59720,	0.43389	,	0.67461},
	{-0.59720,	-0.43389,	0.67461},
	{0.22811,	-0.70205,	0.67461},
	{0.63820,	0.26287	,	0.72361},
	{-0.05279,	0.68819	,	0.72361},
	{-0.67082,	0.16246	,	0.72361},
	{-0.36180,	-0.58779,	0.72361},
	{0.44721,	-0.52573,	0.72361},
	{0.63820,	-0.26287,	0.72361},
	{0.44721,	0.52573	,	0.72361},
	{-0.36180,	0.58779	,	0.72361},
	{-0.67082,	-0.16246,	0.72361},
	{-0.05279,	-0.68819,	0.72361},
	{0.82262,	0.25989	,	0.50572},
	{0.00703,	0.86267	,	0.50572},
	{-0.81827,	0.27327	,	0.50572},
	{-0.51275,	-0.69378,	0.50572},
	{0.50138,	-0.70205,	0.50572},
	{0.82262,	-0.25989,	0.50572},
	{0.50138,	0.70205	,	0.50572},
	{-0.51275,	0.69378	,	0.50572},
	{-0.81827,	-0.27327,	0.50572},
	{0.00703,	-0.86267,	0.50572},
	{0.95925,	0.16062	,	0.23245},
	{0.86180,	0.42533	,	0.27639},
	{0.67082,	0.68819	,	0.27639},
	{0.44919,	0.86267	,	0.23245},
	{0.14366,	0.96194	,	0.23245},
	{-0.13820,	0.95106	,	0.27639},
	{-0.44721,	0.85065	,	0.27639},
	{-0.68164,	0.69378	,	0.23245},
	{-0.87046,	0.43389	,	0.23245},
	{-0.94721,	0.16246	,	0.27639},
	{-0.94721,	-0.16246,	0.27639},
	{-0.87046,	-0.43389,	0.23245},
	{-0.68164,	-0.69378,	0.23245},
	{-0.44721,	-0.85065,	0.27639},
	{-0.13820,	-0.95106,	0.27639},
	{0.14366,	-0.96194,	0.23245},
	{0.44919,	-0.86267,	0.23245},
	{0.67082,	-0.68819,	0.27639},
	{0.86180,	-0.42533,	0.27639},
	{0.95925,	-0.16062,	0.23245},
	{0.80902,	0.58779	,	0.00000},
	{0.30902,	0.95106	,	0.00000},
	{-0.30902,	0.95106	,	0.00000},
	{-0.80902,	0.58779	,	0.00000},
	{-1.00000,	-0.00000,	0.00000},
	{-0.80902,	-0.58779,	0.00000},
	{-0.30902,	-0.95106,	0.00000},
	{0.30902,	-0.95106,	0.00000},
	{0.80902,	-0.58779,	0.00000},
	{1.00000,	0.00000	,	0.00000}
};
const double hemi_3[HEMI3_LIGHTS][3] = 
{
	{+0.0000,	+0.0000,	+0.5000},
	{+0.2629,	+0.0000,	+0.4253},
	{+0.0812,	+0.2500,	+0.4253},
	{-0.2127,	+0.1545,	+0.4253},
	{-0.2127,	-0.1545,	+0.4253},
	{+0.0812,	-0.2500,	+0.4253},
	{+0.4472,	+0.0000,	+0.2236},
	{+0.1382,	+0.4253,	+0.2236},
	{-0.3618,	+0.2629,	+0.2236},
	{-0.3618,	-0.2629,	+0.2236},
	{+0.1382,	-0.4253,	+0.2236},
	{+0.3441,	+0.2500,	+0.2629},
	{-0.1314,	+0.4045,	+0.2629},
	{-0.4253,	-0.0000,	+0.2629},
	{-0.1314,	-0.4045,	+0.2629},
	{+0.3441,	-0.2500,	+0.2629},
	{+0.4755,	+0.1545,	+0.0000},
	{+0.2939,	+0.4045,	+0.0000},
	{-0.0000,	+0.5000,	+0.0000},
	{-0.2939,	+0.4045,	+0.0000},
	{-0.4755,	+0.1545,	+0.0000},
	{-0.4755,	-0.1545,	+0.0000},
	{-0.2939,	-0.4045,	+0.0000},
	{+0.0000,	-0.5000,	+0.0000},
	{+0.2939,	-0.4045,	+0.0000},
	{+0.4755,	-0.1545,	+0.0000},
	{+0.0917,	+0.0000,	+0.4915},
	{+0.1804,	+0.0000,	+0.4663},
	{+0.0283,	+0.0872,	+0.4915},
	{+0.0557,	+0.1715,	+0.4663},
	{-0.0742,	+0.0539,	+0.4915},
	{-0.1459,	+0.1060,	+0.4663},
	{-0.0742,	-0.0539,	+0.4915},
	{-0.1459,	-0.1060,	+0.4663},
	{+0.0283,	-0.0872,	+0.4915},
	{+0.0557,	-0.1715,	+0.4663},
	{+0.2106,	+0.0884,	+0.4448},
	{+0.1492,	+0.1730,	+0.4448},
	{-0.0190,	+0.2276,	+0.4448},
	{-0.1184,	+0.1953,	+0.4448},
	{-0.2224,	+0.0523,	+0.4448},
	{-0.2224,	-0.0523,	+0.4448},
	{-0.1184,	-0.1953,	+0.4448},
	{-0.0190,	-0.2276,	+0.4448},
	{+0.1492,	-0.1730,	+0.4448},
	{+0.2106,	-0.0884,	+0.4448},
	{+0.3364,	+0.0000,	+0.3699},
	{+0.3986,	+0.0000,	+0.3019},
	{+0.1040,	+0.3200,	+0.3699},
	{+0.1232,	+0.3791,	+0.3019},
	{-0.2722,	+0.1978,	+0.3699},
	{-0.3225,	+0.2343,	+0.3019},
	{-0.2722,	-0.1978,	+0.3699},
	{-0.3225,	-0.2343,	+0.3019},
	{+0.1040,	-0.3200,	+0.3699},
	{+0.1232,	-0.3791,	+0.3019},
	{+0.3036,	+0.0884,	+0.3873},
	{+0.3311,	+0.1730,	+0.3323},
	{+0.0097,	+0.3161,	+0.3873},
	{-0.0622,	+0.3683,	+0.3323},
	{-0.2976,	+0.1069,	+0.3873},
	{-0.3695,	+0.0547,	+0.3323},
	{-0.1936,	-0.2500,	+0.3873},
	{-0.1662,	-0.3346,	+0.3323},
	{+0.1779,	-0.2614,	+0.3873},
	{+0.2668,	-0.2614,	+0.3323},
	{+0.3036,	-0.0884,	+0.3873},
	{+0.3311,	-0.1730,	+0.3323},
	{+0.1779,	+0.2614,	+0.3873},
	{+0.2668,	+0.2614,	+0.3323},
	{-0.1936,	+0.2500,	+0.3873},
	{-0.1662,	+0.3346,	+0.3323},
	{-0.2976,	-0.1069,	+0.3873},
	{-0.3695,	-0.0547,	+0.3323},
	{+0.0097,	-0.3161,	+0.3873},
	{-0.0622,	-0.3683,	+0.3323},
	{+0.4269,	+0.0872,	+0.2452},
	{+0.3922,	+0.1715,	+0.2584},
	{+0.0490,	+0.4330,	+0.2452},
	{-0.0420,	+0.4260,	+0.2584},
	{-0.3967,	+0.1804,	+0.2452},
	{-0.4181,	+0.0917,	+0.2584},
	{-0.2941,	-0.3215,	+0.2452},
	{-0.2165,	-0.3693,	+0.2584},
	{+0.2149,	-0.3791,	+0.2452},
	{+0.2843,	-0.3200,	+0.2584},
	{+0.3922,	-0.1715,	+0.2584},
	{+0.4269,	-0.0872,	+0.2452},
	{+0.2843,	+0.3200,	+0.2584},
	{+0.2149,	+0.3791,	+0.2452},
	{-0.2165,	+0.3693,	+0.2584},
	{-0.2941,	+0.3215,	+0.2452},
	{-0.4181,	-0.0917,	+0.2584},
	{-0.3967,	-0.1804,	+0.2452},
	{-0.0420,	-0.4260,	+0.2584},
	{+0.0490,	-0.4330,	+0.2452},
	{+0.4728,	+0.0539,	+0.1534},
	{+0.4824,	+0.1060,	+0.0780},
	{+0.4063,	+0.2276,	+0.1819},
	{+0.4508,	+0.1953,	+0.0930},
	{+0.3421,	+0.3161,	+0.1819},
	{+0.3251,	+0.3683,	+0.0930},
	{+0.1974,	+0.4330,	+0.1534},
	{+0.2499,	+0.4260,	+0.0780},
	{+0.0948,	+0.4663,	+0.1534},
	{+0.0482,	+0.4915,	+0.0780},
	{-0.0909,	+0.4568,	+0.1819},
	{-0.0465,	+0.4891,	+0.0930},
	{-0.1949,	+0.4230,	+0.1819},
	{-0.2499,	+0.4230,	+0.0930},
	{-0.3508,	+0.3215,	+0.1534},
	{-0.3279,	+0.3693,	+0.0780},
	{-0.4142,	+0.2343,	+0.1534},
	{-0.4526,	+0.1978,	+0.0780},
	{-0.4625,	+0.0547,	+0.1819},
	{-0.4795,	+0.1069,	+0.0930},
	{-0.4625,	-0.0547,	+0.1819},
	{-0.4795,	-0.1069,	+0.0930},
	{-0.4142,	-0.2343,	+0.1534},
	{-0.4526,	-0.1978,	+0.0780},
	{-0.3508,	-0.3215,	+0.1534},
	{-0.3279,	-0.3693,	+0.0780},
	{-0.1949,	-0.4230,	+0.1819},
	{-0.2499,	-0.4230,	+0.0930},
	{-0.0909,	-0.4568,	+0.1819},
	{-0.0465,	-0.4891,	+0.0930},
	{+0.0948,	-0.4663,	+0.1534},
	{+0.0482,	-0.4915,	+0.0780},
	{+0.1974,	-0.4330,	+0.1534},
	{+0.2499,	-0.4260,	+0.0780},
	{+0.3421,	-0.3161,	+0.1819},
	{+0.3251,	-0.3683,	+0.0930},
	{+0.4063,	-0.2276,	+0.1819},
	{+0.4508,	-0.1953,	+0.0930},
	{+0.4728,	-0.0539,	+0.1534},
	{+0.4824,	-0.1060,	+0.0780},
	{+0.4330,	+0.2500,	+0.0000},
	{+0.3716,	+0.3346,	+0.0000},
	{+0.2034,	+0.4568,	+0.0000},
	{+0.1040,	+0.4891,	+0.0000},
	{-0.1040,	+0.4891,	+0.0000},
	{-0.2034,	+0.4568,	+0.0000},
	{-0.3716,	+0.3346,	+0.0000},
	{-0.4330,	+0.2500,	+0.0000},
	{-0.4973,	+0.0523,	+0.0000},
	{-0.4973,	-0.0523,	+0.0000},
	{-0.4330,	-0.2500,	+0.0000},
	{-0.3716,	-0.3346,	+0.0000},
	{-0.2034,	-0.4568,	+0.0000},
	{-0.1040,	-0.4891,	+0.0000},
	{+0.1040,	-0.4891,	+0.0000},
	{+0.2034,	-0.4568,	+0.0000},
	{+0.3716,	-0.3346,	+0.0000},
	{+0.4330,	-0.2500,	+0.0000},
	{+0.4973,	-0.0523,	+0.0000},
	{+0.4973,	+0.0523,	+0.0000},
	{+0.1208,	+0.0878,	+0.4772},
	{-0.0461,	+0.1420,	+0.4772},
	{-0.1493,	-0.0000,	+0.4772},
	{-0.0461,	-0.1420,	+0.4772},
	{+0.1208,	-0.0878,	+0.4772},
	{+0.2465,	+0.1791,	+0.3965},
	{-0.0941,	+0.2897,	+0.3965},
	{-0.3047,	-0.0000,	+0.3965},
	{-0.0941,	-0.2897,	+0.3965},
	{+0.2465,	-0.1791,	+0.3965},
	{+0.3715,	-0.0881,	+0.3229},
	{+0.3715,	+0.0881,	+0.3229},
	{+0.1986,	+0.3261,	+0.3229},
	{+0.0310,	+0.3805,	+0.3229},
	{-0.2488,	+0.2896,	+0.3229},
	{-0.3523,	+0.1471,	+0.3229},
	{-0.3523,	-0.1471,	+0.3229},
	{-0.2488,	-0.2896,	+0.3229},
	{+0.0310,	-0.3805,	+0.3229},
	{+0.1986,	-0.3261,	+0.3229},
	{+0.4476,	+0.1433,	+0.1707},
	{+0.2746,	+0.3814,	+0.1707},
	{+0.0020,	+0.4700,	+0.1707},
	{-0.2778,	+0.3790,	+0.1707},
	{-0.4463,	+0.1471,	+0.1707},
	{-0.4463,	-0.1471,	+0.1707},
	{-0.2778,	-0.3790,	+0.1707},
	{+0.0020,	-0.4700,	+0.1707},
	{+0.2746,	-0.3814,	+0.1707},
	{+0.4476,	-0.1433,	+0.1707},
	{+0.4936,	+0.0000,	+0.0799},
	{+0.3971,	+0.2885,	+0.0952},
	{+0.1525,	+0.4694,	+0.0799},
	{-0.1517,	+0.4668,	+0.0952},
	{-0.3993,	+0.2901,	+0.0799},
	{-0.4909,	-0.0000,	+0.0952},
	{-0.3993,	-0.2901,	+0.0799},
	{-0.1517,	-0.4668,	+0.0952},
	{+0.1525,	-0.4694,	+0.0799},
	{+0.3971,	-0.2885,	+0.0952},
};

void ECORE_API xrHemisphereBuild	(int quality, float energy, xrHemisphereIterator* iterator, LPVOID param)
{
    // SELECT table
    int		h_count, h_table[3];
    const double (*hemi)[3] = 0;
    switch (quality)
    {
    case 1:	// LOW quality
        h_count		= HEMI1_LIGHTS;
        h_table[0]	= 0;
        h_table[1]	= 1;
        h_table[2]	= 2;
        hemi		= hemi_1;
    break;
    case 2:	// HIGH quality
        h_count		= HEMI2_LIGHTS;
        h_table[0]	= 0;
        h_table[1]	= 2;
        h_table[2]	= 1;
        hemi		= hemi_2;
    break;
    case 3:	// SUPER HIGH quality
        h_count		= HEMI3_LIGHTS;
        h_table[0]	= 0;
        h_table[1]	= 2;
        h_table[2]	= 1;
        hemi		= hemi_3;
    break;
    default:// NO 	
        return;
    }
		
    // Calculate energy
    float total = h_count;
    float E		= 1.f/total;
		
    // Iterate
    for (int i=0; i<h_count; i++)
    {
        float x		=	-float	(hemi[i][h_table[0]]);
        float y		=	-float	(hemi[i][h_table[1]]);
        float z		=	-float	(hemi[i][h_table[2]]);
        float mag	=	_sqrt	(x*x + y*y + z*z);
        x /= mag;	y /= mag;	z /= mag;
        iterator	(x,y,z,E*energy,param);
    }
}

