#ifndef DECLSPECDLL_H_HEAD__FILE__
#define DECLSPECDLL_H_HEAD__FILE__
#pragma once

//判断是导出还是导入
#ifdef DllExport
//创建DLL需要导出
#define dllStatus __declspec( dllexport )
#else
//提供给调用者需要导入
#define dllStatus 
#endif

#endif // DECLSPECDLL_H_HEAD__FILE__
