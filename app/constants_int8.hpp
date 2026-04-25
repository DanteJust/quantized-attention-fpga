#ifndef CONSTANTS_INT8_HPP
#define CONSTANTS_INT8_HPP

// Constant: Sequence Length
constexpr int SEQ_LEN = 197;

// Constant: Head Dimension
constexpr int HEAD_DIM = 64;

// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.06156257539987564;

// Selected profile
#define PROFILE_CAT1

/*
==========================
Constants for CAT1 profile
==========================
*/
#ifdef PROFILE_CAT1
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 1.5390610230285507744658809156135e-4;
#endif

/*
==========================
Constants for CAT2 profile
==========================
*/
#ifdef PROFILE_CAT2
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 1.6013818123861243339945074126263e-4;
#endif

/*
============================
Constants for BOXER1 profile
============================
*/
#ifdef PROFILE_BOXER1
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 1.4689465432812537918593138851975e-4;
#endif

/*
============================
Constants for BOXER2 profile
============================
*/
#ifdef PROFILE_BOXER2
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 1.3949930767950379526719127102297e-4;
#endif

/*
================================
Constants for RETRIEVER1 profile
================================
*/
#ifdef PROFILE_RETRIEVER1
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 1.6697823331099891797989592532758e-4;
#endif

/*
================================
Constants for RETRIEVER2 profile
================================
*/
#ifdef PROFILE_RETRIEVER2
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 1.4667421556566022355612532237501e-4;
#endif

#endif
