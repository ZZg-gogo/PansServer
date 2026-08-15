#ifndef __PANS_INCLUDE_PANS_EXPORT_H__
#define __PANS_INCLUDE_PANS_EXPORT_H__


#if defined(_WIN32) && defined(PANS_SHARED_LIBRARY)

    #if defined(PANS_BUILDING_LIBRARY)  //我是正在编译这个DLL 导出到 pans.dll
        #define PANS_API __declspec(dllexport)
    #else //我正在使用这个DLL
        #define PANS_API __declspec(dllimport)
    #endif

#elif defined(__GNUC__) && defined(PANS_SHARED_LIBRARY) 
    #define PANS_API __attribute__((visibility("default")))
#else
    #define PANS_API
#endif






#endif // __PANS_INCLUDE_PANS_EXPORT_H__