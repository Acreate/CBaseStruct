/*
 * 该头文件已经实现 C++ 与 C 兼容处理
 */

#ifndef DATASTRUCTS_H_HEAD__FILE__
#define DATASTRUCTS_H_HEAD__FILE__
#pragma once

#include <stddef.h>
#include "DeclspecDll.h"
#include "ExternDataStruct.h"
///////////////////////// 结构体
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief 数据开始下标
 */
#define  ptrResourcesStartIndex 0xffff
/**
 * @brief 数据类型的下标开始
 */
#define  dataStructTypeMakeIndex 0xff
#define  dataStructUserTypeMakeIndex (dataStructTypeMakeIndex >> 4)

/**
 * @brief 数据的下标一般存在于 Ptr.type <br/>
 * 如 : struct _SqlResourcePtr.type <br/>
 * 用于数据类型应该使用 (dataStructTypeMakeIndex >> 4) 即 dataStructUserTypeMakeIndex 开始编号
*/
typedef enum _DataStructTypeMake {
	/**
	 * @brief 字符串类型
	*/
	str = (dataStructTypeMakeIndex + 5),
	/**
	* @brief 数组类型
	*/
	array = (dataStructTypeMakeIndex >> 1) + 1,
	/**
	 * @brief 映射类型
	*/
	map = (dataStructTypeMakeIndex >> 2) + 1
} DataStructTypeMake;

/**
 * @brief 用户数据结构体<br/>
 * 一般情况下，用户的数据体都应该使用这样的结构来实现，并且 ptr 是一个指向的作用<br/>
 * 如 ：(void*)UserDataResourcePtr.ptr 如此来实现他的指向<br/>
 * 如果遇到复杂情况<br/>
 * 比如说 sizeof(类型 ) > sizeof(size_t) 条件成立， 那么可以实现一个数组，使用 UserDataResourcePtr.ptr 当做下标<br/>
*/
dllStatus typedef struct _UserDataResourcePtr {
	size_t ptr;
	/**
	 * @brief 类型符号, @struct DataStructTypeMake
	*/
	size_t type;
} UserDataResourcePtr;

/**
* @brief 资源共用体<br/>
* 在该框架中，应该避免使用指针，除非是在传递参数的情况下，否则应当使用结构体<br/>
* 通常建议使用 @struct UserDataResourcePtr dataResourcePtr 成员变量
*/
dllStatus typedef union _ResourcePtr {
	struct _SqlResourcePtr* resourcePtr;
	struct _StrPtr* strPtr;
	struct _ArrayPtr* arrayPtr;
	struct _MapPtr* mapPtr;
	struct _PairMuch* pairMuch;
	struct _Pair* pair;
	struct _UserDataResourcePtr dataResourcePtr;
} ResourcePtr;

/**
 * @brief 资源结构体引用
*/
dllStatus typedef struct _SqlResourcePtr {
	size_t ptr;
	/**
	 * @brief 类型符号, @struct DataStructTypeMake
	*/
	size_t type;
} SqlResourcePtr;

/**
 * @brief 基础存储类型
*/
dllStatus typedef struct _BaseType {
	/**
	 * @brief 基础类型，使用时候直接使用强制类型转换\n
	 * 因为它是位置的，只能使用客官的内存读取\n
	 * 如 int p = *(int*)data;\n
	 * 有的时候他是一个不清晰的数组指针\n
	 * 如 int* p = (int*)data;\n
	*/
	void* dataPtr;
} BaseType;

/**
 * @brief 字符串资源引用
*/
dllStatus typedef struct _StrPtr {
	size_t ptr;
	/**
	 * @brief 类型符号, @struct DataStructTypeMake
	*/
	size_t type;
} StrPtr;

/**
 * @brief 数组
*/
dllStatus typedef struct _ArrayPtr {
	size_t ptr;
	/**
	 * @brief 类型符号, @struct DataStructTypeMake
	*/
	size_t type;
} ArrayPtr;

/**
 * @brief 映射结构体引用
*/
dllStatus typedef struct _MapPtr {
	size_t ptr;
	/**
	 * @brief 类型符号, @struct DataStructTypeMake
	*/
	size_t type;
} MapPtr;

/**
 * @brief 配对, 一对一
*/
dllStatus typedef struct _Pair {
	ResourcePtr first;
	ResourcePtr second;
} PairPtr;

#ifdef __cplusplus
}
#endif

///////////////////////// 函数

#ifdef __cplusplus
extern "C"{
#endif

/**
* @brief 获取一个 multiple 的倍数
* @param memorySize 内存大小
* @param multiple 倍数
* @return 4 的倍数, 如果 newSize 为 0, 则返回 1024
*/
static inline size_t getMemoryNormalSize(const size_t memorySize, const size_t multiple) {
	size_t mod = memorySize % multiple;
	if( mod )
		return memorySize - mod;
	return memorySize;
}

/**
* @brief 创建内存时设置大小
* @param menoryNewSize 新的大小, 默认为 1024
*/
dllStatus void setNewMemorySize(const size_t menoryNewSize);
/**
 * @brief 获取创建内存时设置的大小
 * @return 内存设置大小
*/
dllStatus size_t getNewMemorySize();

/**
* @brief 创建一个字符串
* @return 字符串引用, 失败返回 NULL
*/
dllStatus StrPtr strCreate();
/**
 * @brief 释放一个字符串
 * @param arrayPtr 释放的字符串
 * @return 成功返回 0, -1 表示类型不匹配, 1 表示找不到
*/
dllStatus int strree(const StrPtr* arrayPtr);
/**
* @brief 释放没有使用的字符串资源
* @return 释放个数
*/
dllStatus size_t strGC();
/**
* @brief 释放整个字符串管理
* @return 释放的字符串资源个数
*/
dllStatus size_t strManageFree();
/**
* @brief 获取字符串长度
* @param ptr 字符串
* @return 长度
*/
dllStatus size_t strSize(const StrPtr* ptr);

/**
 * @brief 获取一个标准的 C 字符串
 * @param ptr 指针
 * @param checkSize 检测的个数，若为 0， 则一直检测
 * @param checkIndex 开始检测的位置，若存在结束符，则存储该位置
 * @return 失败返回 NULL
*/
dllStatus const char* strGetStdCString(const StrPtr* ptr, size_t checkSize, size_t* checkIndex);
/**
 * @brief 设置一个标准的 C 字符串
 * @param ptr 字符串资源
 * @param data 字符串数据
 * @param dataSize 设置打字符串数据长度，若长度位置下不存在结束符，则设置为结束符，长度之前出现结束符，则按照最先出现长度设置
 * @return 字符串的长度
*/
dllStatus size_t strCSet(const StrPtr* ptr, const BaseType data, size_t dataSize);
/**
 * @brief 获取字符串，它是跨平台的
 * @param ptr 字符串资源
 * @param data 获取到的字符串指针
 * @param charWidth 字符串指针存储的每个字符大小
 * @return 整个字符串占用的字符个数
*/
dllStatus size_t strGet(const StrPtr* ptr, const BaseType* data, size_t* charWidth);

/**
* @brief 创建一个数组
* @return 数组引用, 失败返回 NULL
*/
dllStatus ArrayPtr arrayCreate();
/**
 * @brief 释放一个数组
 * @param arrayPtr 释放的数组
 * @return 成功返回 0, -1 表示类型不匹配, 1 表示找不到
*/
dllStatus int arrayFree(const ArrayPtr* arrayPtr);
/**
* @brief 释放没有使用的数组资源
* @return 释放个数
*/
dllStatus size_t arrayGC();
/**
* @brief 释放整个数组管理
* @return 释放的数组资源个数
*/
dllStatus size_t arrayManageFree();
/**
* @brief 获取数组大小
* @param ptr 数组
* @return 大小
*/
dllStatus size_t arraySize(const ArrayPtr* ptr);
/**
* @brief 获取下标
* @param ptr 数组资源
* @param index 下标
* @param pResourcePtr 返回引用, 数据获取成功时, 将会存储在该结构体中
* @return 下标, 成功将会与 index 相等, 否则失败, 并且返回数组长度
*/
dllStatus size_t arrayGetIndex(const ArrayPtr* ptr, const size_t index, ResourcePtr* pResourcePtr);
/**
 * @brief 设置下标
 * @param ptr 数组资源
 * @param index 下标
 * @param pResourcePtr 需要设置的值
 * @return 成功将会与 index 相等, 否则失败, 并且返回数组长度
*/
dllStatus size_t arraySetIndex(const ArrayPtr* ptr, const size_t index, const ResourcePtr* pResourcePtr);

/**
 * @brief 用户数据回调函数
 * @param resourcePtr 数据指针
 * @param arraySize 数组大小
*/
typedef void (*UserDataCallBack)(ResourcePtr* resourcePtr, const size_t arraySize);
/**
 * @brief 遍历数组
 * @param ptr 遍历数组对象
 * @param userDataCallBack 处理函数,<br/>
 *       附加数据设置 0 时表示处理完成, 设置为 -1 时, 表示删除对象
 * @return 遍历个数
*/
dllStatus size_t arrayForeach(const ArrayPtr* ptr, UserDataCallBack userDataCallBack);
/**
 * @brief 数组申请长度
 * @param ptr 数组资源
 * @param size 申请长度
 * @return 新的长度, 失败返回旧的长度
*/
dllStatus size_t arrayMalloc(const ArrayPtr* ptr, const size_t size);

/**
 * @brief 创建一个映射
 * @return 成功返回对象, 失败返回 NULL
*/
dllStatus MapPtr mapCreate();
/**
 * @brief 释放一个映射
 * @param arrayPtr 释放的数组
 * @return 成功返回 0, -1 表示类型不匹配, 1 表示找不到
*/
dllStatus int mapFree(const MapPtr* arrayPtr);
/**
* @brief 释放没有使用的映射资源
* @return 释放个数
*/
dllStatus size_t mapGC();
/**
* @brief 释放整个映射管理
* @return 释放的数组资源个数
*/
dllStatus size_t mapManageFree();

/**
 * @brief 映射插入元素回调函数
 * @param datas 映射列表中的数据,请不要对该指针进行重新分配, 但是它却可以进行重新赋值
 * @param size 引用的 datas 大小, 重置 datas 时, 应当重置该数值
 * @param pair 用户提供的数据, 该数据由用户调用插入方法时传递
 * @result 返回 0 表示结束, 该返回会导致失去插入功能, -1 表示继续循环, 返回 1 时, 将会由 mapInster 进行插入
*/
typedef int (*MapInsterCallBack)(PairPtr* datas, size_t* size, PairPtr* pair);
/**
 * @brief 遍历元素时回调函数
 * @param datas 映射对象的数组
 * @param size 映射对象的数组的大小
*/
typedef void (*MapForeachCallBack)(PairPtr* datas, const size_t size);
/**
 * @brief 对映射进行插入一个数据行为
 * @param mapPtr 映射
 * @param pair 数据
 * @param mapInsterCallBack 回调过程
 * @return 映射大小
*/
dllStatus size_t mapInster(const MapPtr* mapPtr, PairPtr* pair, const MapInsterCallBack mapInsterCallBack);
/**
 * @brief 获取指定的映射集中的所有元素
 * @param mapPtr 获取指定的映射集
 * @param mapInsterCallBack 发现元素时将会调用该方法
 * @return 遍历个数，如果查找指定的映射级，那么它将会不等于0
*/
dllStatus size_t mapGetPairs(const MapPtr* mapPtr, const MapForeachCallBack mapInsterCallBack);
#ifdef __cplusplus
}
#endif

#endif // DATASTRUCTS_H_HEAD__FILE__
