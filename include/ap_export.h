
#ifndef AP_EXPORT_H
#define AP_EXPORT_H

// clang-format off
#ifdef APS_STATIC_LIB
#define APS_EXPORT
#define APS_NO_EXPORT
#else
// Define APS_EXPORT for any platform
#if defined _WIN32 || defined __CYGWIN__
  #ifdef aps_EXPORTS
    #ifdef __GNUC__
      #define APS_EXPORT __attribute__ ((dllexport))
    #else
      #define APS_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define APS_EXPORT __attribute__ ((dllimport))
    #else
      #define APS_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define APS_EXPORT __attribute__ ((visibility ("default")))
  #else
    #define APS_EXPORT
  #endif
#endif
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef APS_NO_DEPRECATED
#define APS_NO_DEPRECATED
#endif
#endif
// clang-format on

#endif /* AP_EXPORT_H */
