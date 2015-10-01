#ifndef ZL_BASE_PREPROCESS_H
#define ZL_BASE_PREPROCESS_H

/// Helper macro to join 2 tokens
#define ZL_PP_JOIN(X, Y) ZL_PP_DO_JOIN(X, Y)
#define ZL_PP_DO_JOIN(X, Y) ZL_PP_DO_JOIN2(X, Y)
#define ZL_PP_DO_JOIN2(X, Y) X##Y

/// Converts the parameter X to a string after macro replacement
/// on X has been performed.
/// example: ZL_PP_STRINGIZE(UCHAR_MAX) -> "255"
#define ZL_PP_STRINGIZE(X) ZL_PP_DO_STRINGIZE(X)
#define ZL_PP_DO_STRINGIZE(X) #X


#endif  /* ZL_BASE_PREPROCESS_H */