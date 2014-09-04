// for vector text

struct VECTORGLYPH {
	int numlines;
	unsigned char ls[8];
	unsigned char le[8];
};

// coordinates of predefined points in vector glyph
Vector glyphPoints[16] = {
	Vector( -0.4f, -0.5f,   0 ),
	Vector(  0,    -0.5f,   0 ),
	Vector(  0.4f, -0.5f,   0 ),
	Vector(  0.4f, -0.25f,  0 ),
	Vector(  0.4f,  0,      0 ),
	Vector(  0.4f,  0.25f,  0 ),
	Vector(  0.4f,  0.5f,   0 ),
	Vector(  0,     0.5f,   0 ),
	Vector( -0.4f,  0.5f,   0 ),
	Vector(  0,     0,      0 ),
	Vector( -0.4f,  0,      0 ),
	Vector( -0.4f,  0.25f,  0 ),
	Vector(  0.4f,  0.75f,  0 ),
	Vector(  0.4f,  1.0f,   0 ),
	Vector(  0,     1.0f,   0 ),
	Vector( -0.4f,  1.0f,   0 )
};

// Glyph vertex map:
//    0   1   2
//    .   .   3
//    10  9   4
//    11  .   5
// __ 8   7   6 __baseline
//    .   .  12
//    15 14  13

// vector glyph data references index of predefined points in glyphPoints above
VECTORGLYPH vecGlyph[112] = {
	{ 0,	{	0, 0, 0, 0, 0, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// [] (32)
	{ 2,	{ 	1, 7, 0, 0, 0, 0, 0, 0	},	{	9, 6, 0, 0, 0, 0, 0, 0	} },	// !
	{ 2,	{ 	0, 1, 0, 0, 0, 0, 0, 0	}, 	{	10, 9,0, 0, 0, 0, 0, 0	} },	// "
	{ 4, 	{ 	1, 2, 0,10, 0, 0, 0, 0	},	{	8, 7, 3, 5, 0, 0, 0, 0	} },	// #
	{ 0, 	{ 	0, 0, 0, 0, 0, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// $
	{ 7,	{ 	2, 1,10, 0, 0, 4, 6, 0	},	{	8, 7, 4, 1,10, 6, 7, 0	} },	// %
	{ 0,	{ 	0, 0, 0, 0, 0, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// &
	{ 1, 	{ 	1, 0, 0, 0, 0, 0, 0, 0	},	{	9, 0, 0, 0, 0, 0, 0, 0	} },	// '
	{ 0,	{ 	0, 0, 0, 0, 4, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// (
	{ 0, 	{ 	0, 0, 0, 0, 0, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// )
	{ 0,	{ 	0, 0, 0, 0, 0, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// *
	{ 2,	{ 	1,10, 0, 0, 0, 0, 0, 0	},	{	7, 4, 0, 0, 0, 0, 0, 0	} },	// +
	{ 1,	{ 	7, 0, 0, 0, 0, 0, 0, 0	},	{	15,0, 0, 0, 0, 0, 0, 0	} },	// ,
	{ 1,	{ 	10,0, 0, 0, 0, 0, 0, 0	},	{	4, 0, 0, 0, 0, 0, 0, 0	} },	// -
	{ 1,	{ 	8, 0, 0, 0, 0, 0, 0, 0	},	{	8, 0, 0, 0, 0, 0, 0, 0	} },	// .
	{ 1,	{ 	2, 0, 0, 0, 0, 0, 0, 0	},	{	8, 0, 0, 0, 0, 0, 0, 0	} },	// /
	{ 4,	{ 	0, 2, 6, 8, 0, 0, 0, 0	},	{	2, 6, 8, 0, 0, 0, 0, 0	} },	// 0 (48)
	{ 1,	{ 	1, 0, 0, 0, 0, 0, 0, 0	},	{	7, 0, 0, 0, 0, 0, 0, 0	} },	// 1
	{ 5,	{ 	0, 2, 4,10, 8, 0, 0, 0	},	{	2, 4,10, 8, 6, 0, 0, 0	} },	// 2
	{ 4, 	{ 	0, 2, 6,10, 0, 0, 0, 0	},	{	2, 6, 8, 4, 0, 0, 0, 0	} },	// 3
	{ 3, 	{ 	0, 10,2, 0, 0, 0, 0, 0	},	{	10,4, 6, 0, 0, 0, 0, 0	} },	// 4
	{ 5,	{ 	2, 0,10, 4, 6, 0, 0, 0	},	{	0,10, 4, 6, 8, 0, 0, 0	} },	// 5
	{ 5,	{ 	2, 0, 8, 6, 4, 0, 0, 0	},	{	0, 8, 6, 4,10, 0, 0, 0	} },	// 6
	{ 2, 	{ 	0, 2, 0, 0, 0, 0, 0, 0	},	{	2, 6, 0, 0, 0, 0, 0, 0	} },	// 7
	{ 5,	{ 	0, 2, 6, 8, 4, 0, 0, 0	},	{	2, 6, 8, 0,10, 0, 0, 0	} },	// 8
	{ 4, 	{ 	6, 2, 0,10, 0, 0, 0, 0	},	{	2, 0,10, 4, 0, 0, 0, 0	} },	// 9
	{ 1,	{ 	8, 0, 0, 0, 0, 0, 0, 0	},	{	7, 0, 0, 0, 0, 0, 0, 0	} },	// : (.)
	{ 6,	{ 	5, 9, 11,7, 0, 2, 0, 0	},	{	9,11, 7, 5,11, 5, 0, 0	} },	// ; ship glyph
	{ 4,	{ 	0, 2, 6, 8, 0, 0, 0, 0	},	{	2, 6, 8, 0, 0, 0, 0, 0	} },	// 0
	{ 4,	{ 	0, 2, 6, 8, 0, 0, 0, 0	},	{	2, 6, 8, 0, 0, 0, 0, 0	} },	// 0
	{ 4,	{ 	0, 2, 6, 8, 0, 0, 0, 0	},	{	2, 6, 8, 0, 0, 0, 0, 0	} },	// 0
	{ 4,	{ 	0, 2, 6, 8, 0, 0, 0, 0	},	{	2, 6, 8, 0, 0, 0, 0, 0	} },	// 0
	{ 4,	{ 	0, 2, 6, 8, 0, 0, 0, 0	},	{	2, 6, 8, 0, 0, 0, 0, 0	} },	// 0
	{ 5,	{ 	8,10, 1, 4, 4, 0, 0, 0	},	{	10,1, 4, 6,10, 0, 0, 0	} },	// A (65)
	{ 8,	{ 	0,10, 8, 0, 1, 3, 9, 5	},	{	8, 4, 6, 1, 3, 4, 5, 6	} },	// B
	{ 3,	{ 	2, 0, 8, 0, 0, 0, 0, 0	},	{	0, 8, 6, 0, 0, 0, 0, 0	} },	// C
	{ 6,	{ 	0, 8, 7, 5, 3, 1, 0, 0	},	{	8, 7, 5, 3, 1, 0, 0, 0	} },	// D
	{ 4,	{ 	2, 0, 8, 9, 0, 0, 0, 0	},	{	0, 8, 6,10, 0, 0, 0, 0	} },	// E
	{ 3,	{ 	2, 0, 9, 0, 0, 0, 0, 0	},	{	0, 8,10, 0, 0, 0, 0, 0	} },	// F
	{ 5,	{ 	2, 0, 8, 6, 4, 0, 0, 0	},	{	0, 8, 6, 4, 9, 0, 0, 0	} },	// G
	{ 3,	{ 	0, 2, 4, 0, 0, 0, 0, 0	},	{	8, 6,10, 0, 0, 0, 0, 0	} },	// H
	{ 3,	{ 	0, 8, 1, 0, 0, 0, 0, 0	},	{	2, 6, 7, 0, 0, 0, 0, 0	} },	// I
	{ 3,	{ 	2, 6, 8, 0, 0, 0, 0, 0	},	{	6, 8,10, 0, 0, 0, 0, 0	} },	// J
	{ 4,	{ 	0,10, 9, 9, 0, 0, 0, 0	},	{	8, 9, 2, 6, 0, 0, 0, 0	} },	// K
	{ 2,	{ 	0, 8, 0, 0, 0, 0, 0, 0	},	{	8, 6, 0, 0, 0, 0, 0, 0	} },	// L
	{ 4,	{ 	8, 0, 9, 2, 0, 0, 0, 0	},	{	0, 9, 2, 6, 0, 0, 0, 0	} },	// M
	{ 3,	{ 	8, 0, 6, 0, 0, 0, 0, 0	},	{	0, 6, 2, 0, 0, 0, 0, 0	} },	// N
	{ 4,	{ 	0, 2, 6, 8, 0, 0, 0, 0	},	{	2, 6, 8, 0, 0, 0, 0, 0	} },	// O
	{ 4,	{ 	8, 0, 2, 4, 0, 0, 0, 0	},	{	0, 2, 4,10, 0, 0, 0, 0	} },	// P
	{ 6, 	{ 	0, 2, 4, 7, 8, 9, 0, 0	},	{	2, 4, 7, 8, 0, 6, 0, 0	} },	// Q
	{ 5,	{ 	8, 0, 2, 4, 9, 0, 0, 0	},	{	0, 2, 4,10, 6, 0, 0, 0	} },	// R
	{ 5,	{ 	2, 0,10, 4, 6, 0, 0, 0	},	{	0,10, 4, 6, 8, 0, 0, 0	} },	// S
	{ 2,	{ 	0, 1, 0, 0, 0, 0, 0, 0	},	{	2, 7, 0, 0, 0, 0, 0, 0	} },	// T
	{ 3,	{ 	0, 8, 6, 0, 0, 0, 0, 0	},	{	8, 6, 2, 0, 0, 0, 0, 0	} },	// U
	{ 4,	{ 	0,10, 7, 4, 0, 0, 0, 0	},	{	10,7, 4, 2, 0, 0, 0, 0	} },	// V
	{ 4,	{ 	0, 8, 9, 6, 0, 0, 0, 0	},	{	8, 9, 6, 2, 0, 0, 0, 0	} },	// W
	{ 2,	{ 	0, 8, 0, 0, 0, 0, 0, 0	},	{	6, 2, 0, 0, 0, 0, 0, 0	} },	// X
	{ 3,	{ 	0, 9, 9, 0, 0, 0, 0, 0	},	{	9, 2, 7, 0, 0, 0, 0, 0	} },	// Y
	{ 3,	{ 	0, 2, 8, 0, 0, 0, 0, 0	},	{	2, 8, 6, 0, 0, 0, 0, 0	} },	// Z
	{ 3,	{ 	2, 1, 7, 0, 0, 0, 0, 0	},	{	1, 7, 6, 0, 0, 0, 0, 0	} },	// [
	{ 1,	{ 	0, 0, 0, 0, 0, 0, 0, 0	},	{	6, 0, 0, 0, 0, 0, 0, 0	} },	// "\"
	{ 3,	{ 	0, 1, 7, 0, 0, 0, 0, 0	},	{	1, 7, 8, 0, 0, 0, 0, 0	} },	// ]
	{ 0,	{ 	0, 0, 0, 0, 0, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// ^
	{ 1,	{ 	8, 0, 0, 0, 0, 0, 0, 0	},	{	6, 0, 0, 0, 0, 0, 0, 0	} },	// _
	{ 0,	{ 	0, 0, 0, 0, 0, 0, 0, 0	},	{	0, 0, 0, 0, 0, 0, 0, 0	} },	// `
	{ 4,	{ 	4, 9, 8, 6, 0, 0, 0, 0	},	{	9, 8, 6, 4, 0, 0, 0, 0	} },	// a (97)
	{ 4,	{ 	0, 8, 6, 4,10, 0, 0, 0	},	{	8, 6, 4,10, 0, 0, 0, 0	} },	// b
	{ 3,	{ 	4,10, 8, 0, 0, 0, 0, 0	},	{	10,8, 6, 0, 0, 0, 0, 0	} },	// c
	{ 4,	{ 	2, 6, 8,10, 0, 0, 0, 0	},	{	6, 8,10, 4, 0, 0, 0, 0	} },	// d
	{ 5,	{ 	11,5, 4,10, 8, 0, 0, 0	},	{	5, 4,10, 8, 6, 0, 0, 0	} },	// e
	{ 3,	{ 	2, 1,10, 0, 0, 0, 0, 0	},	{	1, 7, 4, 0, 0, 0, 0, 0	} },	// f
	{ 5,	{ 	6, 8,10, 4,13, 0, 0, 0	},	{	8,10, 4,13,14, 0, 0, 0	} },	// g
	{ 3,	{ 	0,10, 4, 0, 0, 0, 0, 0	},	{	8, 4, 6, 0, 0, 0, 0, 0	} },	// h
	{ 3,	{ 	0, 9,10, 0, 0, 0, 0, 0	},	{	1, 7, 9, 0, 0, 0, 0, 0	} },	// i
	{ 3,	{ 	1, 4,13, 0, 0, 0, 0, 0	},	{	2,13,14, 0, 0, 0, 0, 0	} },	// j
	{ 3,	{ 	0, 4,11, 0, 0, 0, 0, 0	},	{	8,11, 6, 0, 0, 0, 0, 0	} },	// k
	{ 2,	{ 	0, 1, 0, 0, 0, 0, 0, 0	},	{	1, 7, 0, 0, 0, 0, 0, 0	} },	// l
	{ 4,	{ 	8,10, 4, 9, 0, 0, 0, 0	},	{	10,4, 6, 7, 0, 0, 0, 0	} },	// m
	{ 4,	{ 	8,11, 9, 4, 0, 0, 0, 0	},	{	10,9, 4, 6, 0, 0, 0, 0	} },	// n
	{ 4,	{ 	4, 6, 8,10, 0, 0, 0, 0	},	{	6, 8,10, 4, 0, 0, 0, 0	} },	// o
	{ 4,	{ 	15,10,4, 6, 0, 0, 0, 0	},	{	10,4, 6, 8, 0, 0, 0, 0	} },	// p
	{ 4,	{ 	13,4,10, 8, 0, 0, 0, 0	},	{	4,10, 8, 6, 0, 0, 0, 0	} },	// q
	{ 3,	{ 	8,11, 9, 0, 0, 0, 0, 0	},	{	10,9, 4, 0, 0, 0, 0, 0	} },	// r
	{ 5,	{ 	4,10,11, 5, 6, 0, 0, 0	},	{	10,11,5, 6, 8, 0, 0, 0	} },	// s
	{ 3,	{ 	4, 1, 7, 0, 0, 0, 0, 0	},	{	10,7, 6, 0, 0, 0, 0, 0	} },	// t
	{ 3,	{ 	10,8, 6, 0, 0, 0, 0, 0	},	{	8, 6, 4, 0, 0, 0, 0, 0	} },	// u
	{ 2,	{ 	10,7, 0, 0, 0, 0, 0, 0	},	{	7, 4, 0, 0, 0, 0, 0, 0	} },	// v
	{ 4,	{ 	10,8, 6, 9, 0, 0, 0, 0	},	{	8, 6, 4, 7, 0, 0, 0, 0	} },	// w
	{ 2,	{ 	10,8, 0, 0, 0, 0, 0, 0	},	{	6, 4, 0, 0, 0, 0, 0, 0	} },	// x
	{ 4,	{ 	10,8, 4,13, 0, 0, 0, 0	},	{	8, 6,13,14, 0, 0, 0, 0	} },	// y
	{ 3,	{ 	10,4, 8, 0, 0, 0, 0, 0	},	{	4, 8, 6, 0, 0, 0, 0, 0	} }		// z
};