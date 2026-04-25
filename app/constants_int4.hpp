#ifndef CONSTANTS_INT4_HPP
#define CONSTANTS_INT4_HPP

// Constant: Sequence Length
constexpr int SEQ_LEN = 197;

// Constant: Head Dimension
constexpr int HEAD_DIM = 64;

// Selected profile
#define PROFILE_CAT1

/*
==========================
Constants for CAT1 profile
==========================
*/
#ifdef PROFILE_CAT1
// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.6062701940536499;

// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 0.01644395056800651578308939904673;
#endif

/*
==========================
Constants for CAT2 profile
==========================
*/
#ifdef PROFILE_CAT2
// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.6004654765129089;

// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 0.01772848281225691703597969186376;
#endif

/*
============================
Constants for BOXER1 profile
============================
*/
#ifdef PROFILE_BOXER1
// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.600008487701416;

// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 0.01728349510998872237042247288628;
#endif

/*
============================
Constants for BOXER2 profile
============================
*/
#ifdef PROFILE_BOXER2
// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.5143698453903198;

// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 0.01266929629967505941717417357722;
#endif

/*
================================
Constants for RETRIEVER1 profile
================================
*/
#ifdef PROFILE_RETRIEVER1
// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.6352397799491882;

// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 0.01818477894725145759510733114439;
#endif

/*
================================
Constants for RETRIEVER2 profile
================================
*/
#ifdef PROFILE_RETRIEVER2
// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.5564109683036804;

// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 0.01472097706683717080707389643067;
#endif

#endif
