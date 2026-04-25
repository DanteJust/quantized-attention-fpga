#ifndef CONSTANTS_INT16_HPP
#define CONSTANTS_INT16_HPP

// Constant: Sequence Length
constexpr int SEQ_LEN = 197;

// Constant: Head Dimension
constexpr int HEAD_DIM = 64;

// Constant: Scale factor for the V matrix
constexpr double V_SCALE_FACTOR = 0.00023860734654590487;

// Selected profile
#define PROFILE_CAT1

/*
==========================
Constants for CAT1 profile
==========================
*/
#ifdef PROFILE_CAT1
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 2.3120110905622228802821868966222e-9;
#endif

/*
==========================
Constants for CAT2 profile
==========================
*/
#ifdef PROFILE_CAT2
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 2.405630650713695555147317430289e-9;
#endif

/*
============================
Constants for BOXER1 profile
============================
*/
#ifdef PROFILE_BOXER1
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 2.2066835920242843865532962424614e-9;
#endif

/*
============================
Constants for BOXER2 profile
============================
*/
#ifdef PROFILE_BOXER2
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 2.0955889823871533051964710914206e-9;
#endif

/*
================================
Constants for RETRIEVER1 profile
================================
*/
#ifdef PROFILE_RETRIEVER1
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 2.5083836279940369840927711697631e-9;
#endif

/*
================================
Constants for RETRIEVER2 profile
================================
*/
#ifdef PROFILE_RETRIEVER2
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
constexpr double COMBINED_SCALE_FACTOR = 2.2033720347080579427510748855013e-9;
#endif

#endif
