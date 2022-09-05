#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "DataStruct.h"

#include <stdbool.h>

#include "math.h"

/**
 * @brief 字符串管理对象
*/
typedef struct _StringManagment {
	/**
	 * @brief 字符串 id 资源
	*/
	StrPtr* ids;
	/**
	 * @brief 字符串宽度
	*/
	size_t* dataWidth;
	/**
	 * @brief 字符串数据
	*/
	BaseType* datas;
	/**
	 * @brief 字符串长度
	*/
	size_t* strLen;
	/**
	 * @brief 保存的字符串资源个数\n
	 * 保存最大的 StrPtr 数组\n
	 * 也许它指向了 NULL 的 StrPtr 对象\n
	*/
	size_t size;
	/**
	 * @brief 保存的字符串资源最高下标\n
	 * 它并不代表全部使用\n
	 * 应用在遍历场合\n
	 * 当 strSize == strCurrentIndex 时，表示资源已经被使用完毕，不能二次使用则需要重新申请\n
	 * 由于数组总是从 0 开始, 所以 currentUserIndex 永远指向下一个\n
	*/
	size_t currentUserIndex;
} StringManagment;

/**
 * @brief 数组管理对象
*/
typedef struct _ArrayManagment {
	/**
	 * @brief 资源id
	*/
	ArrayPtr* ids;
	/**
	 * @brief 数组资源的大小
	 * @brief @struct ArrayPtr
	*/
	size_t* arrayPtrSizes;
	/**
	 * @brief 资源数据
	*/
	ResourcePtr** datas;
	/**
	 * @brief  数组大小
	 * @struct ArrayManagment 
	*/
	size_t size;
	/**
	 * @brief 当前使用的下标, 它记录最后使用的下标, 从 1 开始, 0 表示全部资源被释放\n
	 * 它并不代表全部使用\n
	 * 应用在遍历场合\n
	 * 当 strSize == strCurrentIndex 时，表示资源已经被使用完毕，不能二次使用则需要重新申请\n
	 * 由于数组总是从 0 开始, 所以 currentUserIndex 永远指向下一个\n
	*/
	size_t currentUserIndex;
} ArrayManagment;

/**
 * @brief 数组管理对象
*/
typedef struct _MapManagment {
	/**
	 * @brief 资源id
	*/
	MapPtr* ids;
	/**
	 * @brief 数组资源的大小
	*/
	size_t* mapPtrSizes;
	/**
	 * @brief 资源数据
	*/
	PairPtr** datas;
	/**
	 * @brief MapPtr 的最大下表, 当 @code{ currentDatasUserIndex == mapPtrSize }	 成立时, datas 存储的内存用尽
	*/
	size_t* currentDatasUserIndex;
	/**
	 * @brief struct _MapManagment 大小
	*/
	size_t size;
	/**
	 * @brief 当前使用的下标, 它记录最后使用的下标, 从 1 开始, 0 表示全部资源被释放\n
	 * 它并不代表全部使用\n
	 * 应用在遍历场合\n
	 * 当 strSize == strCurrentIndex 时，表示资源已经被使用完毕，不能二次使用则需要重新申请\n
	 * 由于数组总是从 0 开始, 所以 currentUserIndex 永远指向下一个<br/>
	*/
	size_t currentUserIndex;
} MapManagment;

/**
 * @brief 每次内存不足时扩展容量
*/
static size_t newCreateSize = 0;

/**
 * @brief 保存字符串的内存块
*/
static StringManagment ptrStringMenoryManagment = {NULL, NULL, NULL, 0, 0, 0};

/**
 * @brief 保存数组的内存块
*/
static ArrayManagment ptrArrayMenoryManagment = {NULL, NULL, NULL, 0, 0};
/**
 * @brief 保存映射的内存块
*/
static MapManagment ptrMapMenoryManagment = {NULL, NULL, NULL, NULL, 0, 0};

/**
 * @brief 兼容编译平台
 */
#ifndef min
#define min __min
#endif

/**
 * @brief 用于获取字符串集管理的最小长度
 */
#define getMinStrForeachIndex( ) min( ptrStringMenoryManagment.currentUserIndex, ptrStringMenoryManagment.size )
/**
 * @brief 用于获取数组管理的最小长度
 */
#define getMinArrayForeachIndex( ) min( ptrArrayMenoryManagment.currentUserIndex, ptrArrayMenoryManagment.size )
/**
 * @brief 用于获取映射集管理的最小长度
 */
#define getMinMapForeachIndex( ) min( ptrMapMenoryManagment.currentUserIndex, ptrMapMenoryManagment.size )
/**
 * @brief 字符串资源大小
 */
#define  sizeStrPtrSize  sizeof( StrPtr )

/**
 * @brief 一个数组资源的指针内存占用大小
*/
#define  sizeArrayPtrSize  sizeof( ArrayPtr )
/**
 * @brief 一个映射资源的指针内存占用大小
*/
#define  sizeMapPtrSize  sizeof( MapPtr )
/**
 * @brief 一个 size_t 资源的指针内存占用大小
*/
#define  sizeSize_tTypeSize   sizeof( size_t )
/**
 * @brief 共用体引用大小
 */
#define sizeResourcePtrSize sizeof( ResourcePtr)

/**
 * @brief 共用体引用指针大小
 */
#define sizeResourcePtrPtrSize sizeof( ResourcePtr*)
/**
 * @brief 基础数据类型大小，他在字符串当中显示为数据大小
 */
#define sizeBaseTypePtrSize sizeof( BaseType)

/*
 * @brief 映射指针的大小
 */
#define sizePairPtrPtrSize sizeof( PairPtr*)
/**
 * @brief 映射对象的大小
 */
#define sizePairPtrSize sizeof( PairPtr)

void setNewMemorySize(const size_t menoryNewSize) {
	if( menoryNewSize == 0 )
		newCreateSize = 1024;
	else
		newCreateSize = getMemoryNormalSize( menoryNewSize, 4 );
}

size_t getNewMemorySize() {
	return newCreateSize;
}

/**
 * @brief 清空内存,设置为0
*/
static void strMemoryReset(const size_t count) {
	size_t index = 0;
	for( ; index < count; ++index ) {
		ptrStringMenoryManagment.ids[index].ptr = ptrStringMenoryManagment.dataWidth[index] = ptrStringMenoryManagment.strLen[index] = 0;
		ptrStringMenoryManagment.ids[index].type = str;
		ptrStringMenoryManagment.datas[index].dataPtr = NULL;
	}
}

/**
 * @brief 把 StrPtr* 内存拷贝到数组中
 * @param ids ArrayPtr 数组资源的指针数组
 * @param strWidth 指向字符串宽度资源的指针数组
 * @param strLens 指向长度数组资源的指针数组
 * @param datas 指向  BaseType*  数组资源的指针数组
 * @param size 指针数组资源长度
*/
static void strMemoryCopy(const StrPtr* ids, size_t* strWidth, size_t* strLens, BaseType* datas, size_t size) {
	int index = 0;
	for( ; index < size; ++index ) {
		ptrStringMenoryManagment.ids[index] = ids[index];
		ptrStringMenoryManagment.dataWidth[index] = strWidth[index];
		ptrStringMenoryManagment.strLen[index] = strLens[index];
		ptrStringMenoryManagment.datas[index] = datas[index];
	}
}

/**
 * @brief 删除指定下标的数据
 * @param index 删除的下标
*/
static void strRemove(const size_t index) {
	// 大小与数据任意为 0, 则该数据不存在使用情况
	if( ptrStringMenoryManagment.datas[index].dataPtr != NULL )
		free( ptrStringMenoryManagment.datas[index].dataPtr );
	ptrStringMenoryManagment.datas[index].dataPtr = NULL;
	ptrStringMenoryManagment.strLen[index] = ptrStringMenoryManagment.dataWidth[index] = ptrStringMenoryManagment.ids[index].ptr = 0;
}

StrPtr strCreate() {
	StrPtr defaultStrPtr = {0, 0};
	// 遍历下标
	int index = 0;
	// 没有被初始化, 需要进入一个初次申请的程序
	if( !newCreateSize )
		newCreateSize = 1024;
	if( !ptrStringMenoryManagment.size ) {
		size_t memroySize = sizeStrPtrSize * newCreateSize;
		ptrStringMenoryManagment.ids = malloc( memroySize );
		memroySize = sizeSize_tTypeSize * newCreateSize;
		ptrStringMenoryManagment.strLen = malloc( memroySize );
		ptrStringMenoryManagment.dataWidth = malloc( memroySize );
		memroySize = sizeBaseTypePtrSize * newCreateSize;
		ptrStringMenoryManagment.datas = malloc( memroySize );
		ptrStringMenoryManagment.size = newCreateSize;
		strMemoryReset( newCreateSize );
		ptrStringMenoryManagment.ids[ptrStringMenoryManagment.currentUserIndex].ptr = ptrResourcesStartIndex;
		ptrStringMenoryManagment.currentUserIndex = 1;
		return ptrStringMenoryManagment.ids[0];
	}
	// 查找自由资源
	for( ; index < ptrStringMenoryManagment.size; ++index ) {
		if( ptrStringMenoryManagment.ids[index].ptr == 0 ) {
			ptrStringMenoryManagment.ids[index].ptr = ptrResourcesStartIndex + index;
			if( index >= ptrStringMenoryManagment.currentUserIndex )
				ptrStringMenoryManagment.currentUserIndex = index + 1;
			return ptrStringMenoryManagment.ids[index];
		}
	}
	// 已经存在内存, 并且资源耗尽
	if( ptrStringMenoryManagment.currentUserIndex == ptrStringMenoryManagment.size ) {
		// 保存旧资源
		StrPtr* oldIds = ptrStringMenoryManagment.ids;
		size_t* oldStrLen = ptrStringMenoryManagment.strLen;
		size_t* oldStartIndex = ptrStringMenoryManagment.dataWidth;
		BaseType* odlDatas = ptrStringMenoryManagment.datas;
		// 开始创建新的资源
		size_t newSize = ptrStringMenoryManagment.size + newCreateSize;
		ptrStringMenoryManagment.ids = malloc( sizeStrPtrSize * newSize );
		if( ptrStringMenoryManagment.ids != NULL ) {
			ptrStringMenoryManagment.dataWidth = malloc( sizeSize_tTypeSize * newSize );
			if( ptrStringMenoryManagment.dataWidth != NULL ) {
				ptrStringMenoryManagment.strLen = malloc( sizeSize_tTypeSize * newSize );
				if( ptrStringMenoryManagment.strLen != NULL ) {
					ptrStringMenoryManagment.datas = malloc( sizeBaseTypePtrSize * newSize );
					if( ptrStringMenoryManagment.datas != NULL ) {
						// 新资源清0
						strMemoryReset( newSize );
						strMemoryCopy( oldIds, oldStartIndex, oldStrLen, odlDatas, ptrStringMenoryManagment.size );
						ptrStringMenoryManagment.size = newSize;
						free( oldIds );
						free( oldStartIndex );
						free( oldStrLen );
						free( odlDatas );
						// 查找自由资源
						for( ; index < ptrStringMenoryManagment.size; ++index ) {
							if( ptrStringMenoryManagment.ids[index].ptr == 0 ) {
								ptrStringMenoryManagment.ids[index].ptr = ptrResourcesStartIndex + index;
								if( index >= ptrStringMenoryManagment.currentUserIndex )
									ptrStringMenoryManagment.currentUserIndex = index + 1;
								return ptrStringMenoryManagment.ids[index];
							}
						}
					} else {
						// 释放没意义的内存
						free( ptrStringMenoryManagment.ids );
						free( ptrStringMenoryManagment.dataWidth );
						free( ptrStringMenoryManagment.strLen );
						// 恢复以前的内存
						ptrStringMenoryManagment.strLen = oldStrLen;
						ptrStringMenoryManagment.dataWidth = oldStartIndex;
						ptrStringMenoryManagment.ids = oldIds;
						ptrStringMenoryManagment.datas = odlDatas;
					}
				} else {
					// 释放没意义的内存
					free( ptrStringMenoryManagment.ids );
					free( ptrStringMenoryManagment.dataWidth );
					// 恢复以前的内存
					ptrStringMenoryManagment.dataWidth = oldStartIndex;
					ptrStringMenoryManagment.strLen = oldStrLen;
					ptrStringMenoryManagment.ids = oldIds;
				}
			} else {
				// 释放没意义的内存
				free( ptrStringMenoryManagment.ids );
				// 恢复以前的内存
				ptrStringMenoryManagment.dataWidth = oldStartIndex;
				ptrStringMenoryManagment.ids = oldIds;
			}
		} else {
			// 恢复以前的内存
			ptrStringMenoryManagment.ids = oldIds;
		}
		return defaultStrPtr;
	}
	return defaultStrPtr;
}

int strFree(const StrPtr* strPtr) {
	size_t index = strPtr->ptr - ptrResourcesStartIndex, minIndex = getMinStrForeachIndex();
	if( strPtr->type != array )
		return -1;
	if( index < minIndex && strPtr->ptr == ptrStringMenoryManagment.ids[index].ptr ) {
		if( ptrStringMenoryManagment.datas[index].dataPtr != NULL ) {
			free( ptrStringMenoryManagment.datas[index].dataPtr );
			ptrStringMenoryManagment.datas[index].dataPtr = NULL;
		}
		ptrStringMenoryManagment.dataWidth[index] = ptrStringMenoryManagment.strLen[index] = ptrStringMenoryManagment.ids[index].ptr = 0;
		return 0;
	}
	return 1;
}

size_t strGC() {
	size_t index = 0, count = 0, minIndex = getMinStrForeachIndex();
	for( ; index < minIndex; ++index ) {
		if( ptrStringMenoryManagment.ids[index].ptr ) {
			if( ptrStringMenoryManagment.datas[index].dataPtr != NULL ) {
				free( ptrStringMenoryManagment.datas[index].dataPtr );
				ptrStringMenoryManagment.datas[index].dataPtr = NULL;
			}
			ptrStringMenoryManagment.dataWidth[index] = ptrStringMenoryManagment.strLen[index] = ptrStringMenoryManagment.ids[index].ptr = 0;
			++count;
			// 数据莫名其妙被使用的情况下, 应该给它释放
		} else if( ptrStringMenoryManagment.datas[index].dataPtr != NULL ) {
			free( ptrStringMenoryManagment.datas[index].dataPtr );
			ptrStringMenoryManagment.datas[index].dataPtr = NULL;
		}
	}
	return count;
}

size_t strManageFree() {
	size_t index = 0, count = 0, minIndex = getMinStrForeachIndex();
	for( ; index < minIndex; ++index ) {
		// 数据莫名其妙被使用的情况下, 应该给它释放
		if( ptrStringMenoryManagment.datas[index].dataPtr != NULL )
			free( ptrStringMenoryManagment.datas[index].dataPtr );
		// 当它被使用了,应该给它计数
		if( ptrStringMenoryManagment.ids[index].ptr )
			++count;
	}
	free( ptrStringMenoryManagment.ids );
	free( ptrStringMenoryManagment.strLen );
	free( ptrStringMenoryManagment.dataWidth );
	free( ptrStringMenoryManagment.datas );
	ptrStringMenoryManagment.ids = NULL;
	ptrStringMenoryManagment.dataWidth = ptrStringMenoryManagment.strLen = NULL;
	ptrStringMenoryManagment.datas = NULL;
	ptrStringMenoryManagment.size = ptrStringMenoryManagment.currentUserIndex = 0;
	return count;
}

size_t strSize(const StrPtr* ptr) {
	size_t index = ptr->ptr - ptrResourcesStartIndex, minIndex = getMinStrForeachIndex();;
	if( index < minIndex )
		return ptrStringMenoryManagment.strLen[index];
	return 0;
}

size_t strCSet(const StrPtr* ptr, const BaseType data, size_t dataSize) {
	size_t index = ptr->ptr - ptrResourcesStartIndex, minIndex = getMinStrForeachIndex();;
	if( index < minIndex ) {
		// 获取最小长度
		char* charData = data.dataPtr;
		size_t foreachChar = 0;
		for( ; foreachChar < dataSize; ++foreachChar ) {
			if( charData[foreachChar] == '\0' ) {
				dataSize = foreachChar;
				break;
			}
		}
		const size_t dataWidth = sizeof( char ), memsetSize = dataSize;
		char* newData = malloc( (memsetSize + 1) * dataWidth );
		if( newData ) {
			foreachChar = 0;
			for( ; foreachChar < dataSize; ++foreachChar )
				newData[foreachChar] = charData[foreachChar];
			newData[dataSize] = '\0';
			ptrStringMenoryManagment.strLen[index] = dataSize;
			ptrStringMenoryManagment.dataWidth[index] = sizeof( char );
			if( ptrStringMenoryManagment.datas[index].dataPtr )
				free( ptrStringMenoryManagment.datas[index].dataPtr );
			ptrStringMenoryManagment.datas[index].dataPtr = newData;
		}
		return ptrStringMenoryManagment.strLen[index];
	}

	return 0;
}

// todo : 内存泄露， 无法释放 ？
size_t strCAppend(const StrPtr* ptr, const BaseType data, size_t dataSize) {
	size_t index = ptr->ptr - ptrResourcesStartIndex, minIndex = getMinStrForeachIndex();;
	if( index < minIndex ) {
		// 获取最小长度
		char* charData = data.dataPtr;
		size_t foreachChar = 0;
		for( ; foreachChar < dataSize; ++foreachChar ) {
			if( charData[foreachChar] == '\0' ) {
				dataSize = foreachChar;
				break;
			}
		}
		// data 存在字符串可被追加
		if( dataSize > 0 ) {
			// 字符宽度
			const size_t dataWidth = sizeof( char ),
						// 新的字符串大小,但是不包含 \0
						memsetSize = (ptrStringMenoryManagment.strLen[index] + dataSize);
			// 旧的字符串
			char* oldData = malloc( (memsetSize + 1) * dataWidth );
			// 如果无法申请，则重置
			if( oldData != NULL ) {
				// 新字符串的当前下标
				size_t maxIndex = ptrStringMenoryManagment.strLen[index],
						// 目标字符串的下标
						current = 0;
				foreachChar = 0;
				for( ; foreachChar < maxIndex; ++foreachChar )
					oldData[foreachChar] = ((char*)ptrStringMenoryManagment.datas[index].dataPtr)[foreachChar];
				// 拷贝 data 源
				for( ; current < dataSize; ++foreachChar, ++current )
					oldData[foreachChar] = ((char*)data.dataPtr)[current];
				oldData[foreachChar] = '\0';
				ptrStringMenoryManagment.strLen[index] = memsetSize;
				ptrStringMenoryManagment.dataWidth[index] = sizeof( char );
				if( ptrStringMenoryManagment.datas[index].dataPtr != NULL )
					free( ptrStringMenoryManagment.datas[index].dataPtr );
				ptrStringMenoryManagment.datas[index].dataPtr = oldData;
			}
			return ptrStringMenoryManagment.strLen[index];
		}

	}

	return 0;
}

size_t strGet(const StrPtr* ptr, BaseType* data, size_t* charWidth) {
	size_t index = ptr->ptr - ptrResourcesStartIndex, minIndex = getMinStrForeachIndex();;
	if( index < minIndex ) {
		*data = ptrStringMenoryManagment.datas[index];
		*charWidth = ptrStringMenoryManagment.dataWidth[index];
		return ptrStringMenoryManagment.strLen[index];
	}
	return 0;
}

const char* strGetStdCString(const StrPtr* ptr, size_t checkSize, size_t* checkIndex) {
	size_t index = ptr->ptr - ptrResourcesStartIndex, minIndex = getMinStrForeachIndex();
	char* result = NULL;

	if( index < minIndex ) {
		// 标准 C 的获取
		if( sizeof( char ) == ptrStringMenoryManagment.dataWidth[index] ) {
			size_t* currentIndex;
			size_t foreachIndex = 0;
			BaseType base = ptrStringMenoryManagment.datas[index];
			if( checkIndex )
				currentIndex = checkIndex;
			else
				currentIndex = &foreachIndex;
			if( checkSize )
				checkSize = checkSize + *currentIndex;
			do {
				if( *((char*)base.dataPtr + *currentIndex) == '\0' ) {
					result = (char*)base.dataPtr;
					break;
				}
				++*currentIndex;
			} while( checkSize != *currentIndex ) ;
		}

	}
	return result;

}

/**
 * @brief 清空内存,设置为0
*/
static void arrayMemoryReset(const size_t count) {
	size_t index = 0;
	for( ; index < count; ++index ) {
		ptrArrayMenoryManagment.ids[index].ptr = ptrArrayMenoryManagment.arrayPtrSizes[index] = 0;
		ptrArrayMenoryManagment.ids[index].type = array;
		ptrArrayMenoryManagment.datas[index] = NULL;
	}
}

/**
 * @brief 把 array 与 arrayPtrSize 内存拷贝到数组中
 * @param array ArrayPtr 数组资源的指针数组
 * @param arrayPtrSize 指向 ArrayPtr 数组资源的指针数组
 * @param datas 指向  ResourcePtr*  数组资源的指针数组
 * @param size 指针数组资源长度
*/
static void arrayMemoryCopy(const ArrayPtr* array, size_t* arrayPtrSize, ResourcePtr** datas, size_t size) {
	int index = 0;
	for( ; index < size; ++index ) {
		ptrArrayMenoryManagment.ids[index] = array[index];
		ptrArrayMenoryManagment.arrayPtrSizes[index] = arrayPtrSize[index];
		ptrArrayMenoryManagment.datas[index] = datas[index];
	}

}

/**
 * @brief 删除指定下标的数据
 * @param index 删除的下标
*/
static void arrayRemove(const size_t index) {
	// 大小与数据任意为 0, 则该数据不存在使用情况
	if( ptrArrayMenoryManagment.datas[index] != NULL )
		free( ptrArrayMenoryManagment.datas[index] );
	ptrArrayMenoryManagment.datas[index] = NULL;
	ptrArrayMenoryManagment.ids[index].ptr = 0;
	ptrArrayMenoryManagment.arrayPtrSizes[index] = 0;
}

int strReadFile(const StrPtr* ptrFileName, StrPtr* ptrFileContent) {
	// 其中出现一个无效资源，那么就会被直接返回
	if( strValid( ptrFileName ) || strValid( ptrFileContent ) )
		return 1;
	size_t endIndex = 0;
	const char* filePath = strGetStdCString( ptrFileName, 0, &endIndex );
	if( filePath ) {
		FILE* file = fopen( filePath, "r" );
		if( file ) {

			char *buff = malloc( sizeof( char ) * newCreateSize ), *oldBuff = NULL;
			size_t readBinCount = 0, writeBuffCount = 0, currentSize = newCreateSize;
			BaseType basePtr;
			basePtr.dataPtr = buff;
			do {
				readBinCount = fread( buff, sizeof( char ), newCreateSize, file );
				strCAppend( ptrFileContent, basePtr, readBinCount );

			} while( readBinCount == newCreateSize );

			free( buff );
			fclose( file );
		}
	}
	return 1;
}

int strValid(const StrPtr* checkPtr) {
	size_t ptr = checkPtr->ptr - ptrResourcesStartIndex, minPtr = getMinStrForeachIndex();
	if( ptr < minPtr )
		return 0;
	return 1;
}

ArrayPtr arrayCreate() {
	ArrayPtr defaultArrayPtr = {0, 0};
	// 遍历下标
	int index = 0;
	// 没有被初始化, 需要进入一个初次申请的程序
	if( !newCreateSize )
		newCreateSize = 1024;
	if( !ptrArrayMenoryManagment.size ) {
		size_t memroySize = sizeArrayPtrSize * newCreateSize;
		ptrArrayMenoryManagment.ids = malloc( memroySize );
		memroySize = sizeSize_tTypeSize * newCreateSize;
		ptrArrayMenoryManagment.arrayPtrSizes = malloc( memroySize );
		memroySize = sizeResourcePtrPtrSize * newCreateSize;
		ptrArrayMenoryManagment.datas = malloc( memroySize );
		ptrArrayMenoryManagment.size = newCreateSize;
		arrayMemoryReset( newCreateSize );
		ptrArrayMenoryManagment.ids[ptrArrayMenoryManagment.currentUserIndex].ptr = ptrResourcesStartIndex;
		ptrArrayMenoryManagment.currentUserIndex = 1;
		return ptrArrayMenoryManagment.ids[0];
	}
	// 查找自由资源
	for( ; index < ptrArrayMenoryManagment.size; ++index ) {
		if( ptrArrayMenoryManagment.ids[index].ptr == 0 ) {
			ptrArrayMenoryManagment.ids[index].ptr = ptrResourcesStartIndex + index;

			if( index >= ptrArrayMenoryManagment.currentUserIndex )
				ptrArrayMenoryManagment.currentUserIndex = index + 1;
			return ptrArrayMenoryManagment.ids[index];
		}
	}
	// 已经存在内存, 并且资源耗尽
	if( ptrArrayMenoryManagment.currentUserIndex == ptrArrayMenoryManagment.size ) {
		// 保存旧资源
		ArrayPtr* oldIds = ptrArrayMenoryManagment.ids;
		size_t* oldArrayPtrSize = ptrArrayMenoryManagment.arrayPtrSizes;
		ResourcePtr** oldDatas = ptrArrayMenoryManagment.datas;
		// 开始创建新的资源
		size_t newSize = ptrArrayMenoryManagment.size + newCreateSize;
		ptrArrayMenoryManagment.ids = malloc( sizeArrayPtrSize * newSize );
		if( ptrArrayMenoryManagment.ids != NULL ) {
			ptrArrayMenoryManagment.arrayPtrSizes = malloc( sizeSize_tTypeSize * newSize );
			if( ptrArrayMenoryManagment.arrayPtrSizes != NULL ) {
				ptrArrayMenoryManagment.datas = malloc( sizeResourcePtrPtrSize * newSize );
				if( ptrArrayMenoryManagment.datas != NULL ) {
					// 新资源清0
					arrayMemoryReset( newSize );
					arrayMemoryCopy( oldIds, oldArrayPtrSize, oldDatas, ptrArrayMenoryManagment.size );
					ptrArrayMenoryManagment.size = newSize;
					free( oldIds );
					free( oldArrayPtrSize );
					free( oldDatas );
					// 查找自由资源
					for( ; index < ptrArrayMenoryManagment.size; ++index ) {
						if( ptrArrayMenoryManagment.ids[index].ptr == 0 ) {
							ptrArrayMenoryManagment.ids[index].ptr = ptrResourcesStartIndex + index;

							if( index >= ptrArrayMenoryManagment.currentUserIndex )
								ptrArrayMenoryManagment.currentUserIndex = index + 1;
							return ptrArrayMenoryManagment.ids[index];
						}
					}
				} else {
					// 释放没意义的内存
					free( ptrArrayMenoryManagment.ids );
					free( ptrArrayMenoryManagment.arrayPtrSizes );
					// 恢复以前的内存
					ptrArrayMenoryManagment.arrayPtrSizes = oldArrayPtrSize;
					ptrArrayMenoryManagment.ids = oldIds;
					ptrArrayMenoryManagment.datas = oldDatas;
				}
			} else {
				// 释放没意义的内存
				free( ptrArrayMenoryManagment.ids );
				// 恢复以前的内存
				ptrArrayMenoryManagment.arrayPtrSizes = oldArrayPtrSize;
				ptrArrayMenoryManagment.ids = oldIds;
			}
		} else {
			// 恢复以前的内存
			ptrArrayMenoryManagment.ids = oldIds;
		}
		return defaultArrayPtr;
	}
	return defaultArrayPtr;
}

int arrayFree(const ArrayPtr* arrayPtr) {
	size_t index = arrayPtr->ptr - ptrResourcesStartIndex, minIndex = getMinArrayForeachIndex();
	if( arrayPtr->type != array )
		return -1;
	if( index < minIndex ) {
		if( ptrArrayMenoryManagment.datas[index] != NULL ) {
			free( ptrArrayMenoryManagment.datas[index] );
			ptrArrayMenoryManagment.datas[index] = NULL;
		}
		ptrArrayMenoryManagment.arrayPtrSizes[index] = 0;
		ptrArrayMenoryManagment.ids[index].ptr = 0;
		return 0;
	}
	return 1;
}

size_t arrayGC() {
	size_t index = 0, count = 0, minIndex = getMinArrayForeachIndex();

	for( ; index < minIndex; ++index ) {
		if( ptrArrayMenoryManagment.ids[index].ptr ) {
			if( ptrArrayMenoryManagment.datas[index] != NULL ) {
				free( ptrArrayMenoryManagment.datas[index] );
				ptrArrayMenoryManagment.datas[index] = NULL;
			}
			ptrArrayMenoryManagment.ids[index].ptr = 0;
			ptrArrayMenoryManagment.arrayPtrSizes[index] = 0;
			++count;
			// 数据莫名其妙被使用的情况下, 应该给它释放
		} else if( ptrArrayMenoryManagment.datas[index] != NULL ) {
			free( ptrArrayMenoryManagment.datas[index] );
			ptrArrayMenoryManagment.datas[index] = NULL;
		}
	}
	return count;
}

size_t arrayManageFree() {
	size_t index = 0, count = 0, minIndex = getMinArrayForeachIndex();
	for( ; index < minIndex; ++index ) {
		// 数据莫名其妙被使用的情况下, 应该给它释放
		if( ptrArrayMenoryManagment.datas[index] != NULL )
			free( ptrArrayMenoryManagment.datas[index] );
		// 当它被使用了,应该给它计数
		if( ptrArrayMenoryManagment.ids[index].ptr )
			++count;
	}
	free( ptrArrayMenoryManagment.ids );
	free( ptrArrayMenoryManagment.arrayPtrSizes );
	free( ptrArrayMenoryManagment.datas );
	ptrArrayMenoryManagment.ids = NULL;
	ptrArrayMenoryManagment.arrayPtrSizes = NULL;
	ptrArrayMenoryManagment.datas = NULL;
	ptrArrayMenoryManagment.size = ptrArrayMenoryManagment.currentUserIndex = 0;
	return count;
}

size_t arraySize(const ArrayPtr* arrayPtr) {
	size_t index = arrayPtr->ptr - ptrResourcesStartIndex, minIndex = getMinArrayForeachIndex();
	if( index < minIndex )
		return ptrArrayMenoryManagment.arrayPtrSizes[index];
	return 0;
}

size_t arraySetIndex(const ArrayPtr* arrayPtr, const size_t index, const ResourcePtr* pResourcePtr) {
	size_t foreachIndex = arrayPtr->ptr - ptrResourcesStartIndex, minIndex = getMinArrayForeachIndex(), arraySize = 0;
	if( foreachIndex < minIndex ) {
		arraySize = ptrArrayMenoryManagment.arrayPtrSizes[foreachIndex];
		if( index < arraySize )
			ptrArrayMenoryManagment.datas[foreachIndex][index] = *pResourcePtr;
	}
	return arraySize;
}

size_t arrayForeach(const ArrayPtr* arrayPtr, UserDataCallBack userDataCallBack) {
	size_t foreachIndex = arrayPtr->ptr - ptrResourcesStartIndex, minIndex = getMinArrayForeachIndex(), arraySize = 0;
	if( foreachIndex < minIndex ) {
		userDataCallBack( ptrArrayMenoryManagment.datas[foreachIndex], ptrArrayMenoryManagment.arrayPtrSizes[foreachIndex] );
		return ptrArrayMenoryManagment.arrayPtrSizes[foreachIndex];
	}
	return arraySize;
}

size_t arrayMalloc(const ArrayPtr* arrayPtr, const size_t size) {
	size_t foreachIndex = arrayPtr->ptr - ptrResourcesStartIndex, minIndex = getMinArrayForeachIndex(), arraySize = 0;
	if( foreachIndex < minIndex ) {
		// 创建 ResourcePtr 数组
		ResourcePtr* newBuff = malloc( sizeResourcePtrSize * size );
		arraySize = ptrArrayMenoryManagment.arrayPtrSizes[foreachIndex];
		// 如果存在数据, 则需要拷贝
		if( arraySize > 0 ) {
			size_t minCount = min( arraySize, size ), index = 0;
			for( ; index < minCount; ++index )
				newBuff[index] = ptrArrayMenoryManagment.datas[foreachIndex][index];
		}
		if( ptrArrayMenoryManagment.datas[foreachIndex] != NULL )
			free( ptrArrayMenoryManagment.datas[foreachIndex] );
		arraySize = ptrArrayMenoryManagment.arrayPtrSizes[foreachIndex] = size;
		ptrArrayMenoryManagment.datas[foreachIndex] = newBuff;
	}
	return arraySize;
}

size_t arrayGetIndex(const ArrayPtr* arrayPtr, const size_t index, ResourcePtr* pResourcePtr) {
	size_t foreachIndex = arrayPtr->ptr - ptrResourcesStartIndex, minIndex = getMinArrayForeachIndex(), arraySize = 0;
	if( foreachIndex < minIndex ) {
		arraySize = ptrArrayMenoryManagment.arrayPtrSizes[foreachIndex];
		if( index < arraySize ) {
			*pResourcePtr = ptrArrayMenoryManagment.datas[foreachIndex][index];
			arraySize = index;
		}
	}
	return arraySize;
}

/**
 * @brief 映射集重置
 * @param startIndex 重置开始下标
 * @param endIndex 结束下标(该下标元素不会被赋值)
*/
static void mapMemoryReset(size_t startIndex, const size_t endIndex) {
	for( ; startIndex < endIndex; ++startIndex ) {
		ptrMapMenoryManagment.currentDatasUserIndex[startIndex] = ptrMapMenoryManagment.ids[startIndex].ptr = ptrMapMenoryManagment.mapPtrSizes[startIndex] = 0;
		ptrMapMenoryManagment.datas[startIndex] = NULL;
		ptrMapMenoryManagment.ids[startIndex].type = map;
	}
}

/**
 * @brief 拷贝指定数据到映射集当中, 会从下标 0 开始拷贝
 * @param map MapPtr 数组
 * @param odlMapPtrSize 大小数组, MapPtr 数组中每个元素的大小
 * @param pair 映射对象数组数据(二维数组)
 * @param oldCurrentDatasUserIndex 使用下标, MapPtr 数组中 映射对象数组数据 使用大小
 * @param size 拷贝个数
*/
static void mapMemoryCopy(MapPtr* map, const size_t* odlMapPtrSize, PairPtr** pair, const size_t* oldCurrentDatasUserIndex, const size_t size) {
	size_t index = 0;
	for( ; index < size; ++index ) {
		ptrMapMenoryManagment.ids[index].ptr = map[index].ptr;
		ptrMapMenoryManagment.mapPtrSizes[index] = odlMapPtrSize[index];
		ptrMapMenoryManagment.currentDatasUserIndex[index] = oldCurrentDatasUserIndex[index];
		ptrMapMenoryManagment.datas[index] = pair[index];
		ptrMapMenoryManagment.ids[index].type = map[index].type;
	}
}

MapPtr mapCreate() {

	/**
	* @brief 一个默认的映射指针资源， 它总是被设置为0
	*/
	MapPtr defaultMapPtr = {0, 0};
	// 遍历下标
	size_t index = 0;
	// 没有被初始化, 需要进入一个初次申请的程序
	if( !newCreateSize )
		newCreateSize = 1024;
	if( !ptrMapMenoryManagment.size ) {
		size_t memroySize = sizeMapPtrSize * newCreateSize;
		ptrMapMenoryManagment.ids = malloc( memroySize );
		memroySize = sizeSize_tTypeSize * newCreateSize;
		ptrMapMenoryManagment.mapPtrSizes = malloc( memroySize );

		ptrMapMenoryManagment.currentDatasUserIndex = malloc( memroySize );
		memroySize = sizePairPtrPtrSize * newCreateSize;
		ptrMapMenoryManagment.datas = malloc( memroySize );
		ptrMapMenoryManagment.size = newCreateSize;
		mapMemoryReset( 0, newCreateSize );
		ptrMapMenoryManagment.ids[ptrMapMenoryManagment.currentUserIndex].ptr = ptrResourcesStartIndex;
		ptrMapMenoryManagment.currentUserIndex = 1;
		return ptrMapMenoryManagment.ids[0];
	}
	// 查找自由资源
	for( ; index < ptrMapMenoryManagment.size; ++index ) {
		if( ptrMapMenoryManagment.ids[index].ptr == 0 ) {
			ptrMapMenoryManagment.ids[index].ptr = ptrResourcesStartIndex + index;

			if( index >= ptrMapMenoryManagment.currentUserIndex )
				ptrMapMenoryManagment.currentUserIndex = index + 1;
			return ptrMapMenoryManagment.ids[index];
		}
	}

	// 已经存在内存, 并且资源耗尽
	if( ptrMapMenoryManagment.currentUserIndex == ptrMapMenoryManagment.size ) {
		// 保存旧资源
		MapPtr* oldIds = ptrMapMenoryManagment.ids;
		size_t* oldMapPtrSize = ptrMapMenoryManagment.mapPtrSizes;
		size_t* oldCurrentDatasUserIndex = ptrMapMenoryManagment.currentDatasUserIndex;
		PairPtr** odlDatas = ptrMapMenoryManagment.datas;
		// 开始创建新的资源
		size_t newSize = ptrMapMenoryManagment.size + newCreateSize;
		ptrMapMenoryManagment.ids = malloc( sizeArrayPtrSize * newSize );
		if( ptrMapMenoryManagment.ids != NULL ) {
			ptrMapMenoryManagment.mapPtrSizes = malloc( sizeSize_tTypeSize * newSize );
			if( ptrMapMenoryManagment.mapPtrSizes != NULL ) {
				ptrMapMenoryManagment.currentDatasUserIndex = malloc( sizeSize_tTypeSize * newSize );
				if( ptrMapMenoryManagment.currentDatasUserIndex != NULL ) {
					ptrMapMenoryManagment.datas = malloc( sizeResourcePtrPtrSize * newSize );
					if( ptrMapMenoryManagment.datas != NULL ) {
						// 新资源清0
						mapMemoryReset( ptrMapMenoryManagment.size, newSize );
						mapMemoryCopy( oldIds, oldMapPtrSize, odlDatas, oldCurrentDatasUserIndex, ptrMapMenoryManagment.size );
						ptrMapMenoryManagment.size = newSize;
						free( oldIds );
						free( oldMapPtrSize );
						free( oldCurrentDatasUserIndex );
						free( odlDatas );
						// 查找自由资源
						index = ptrMapMenoryManagment.currentUserIndex;
						for( ; index < ptrMapMenoryManagment.size; ++index ) {
							if( ptrMapMenoryManagment.ids[index].ptr == 0 ) {
								ptrMapMenoryManagment.ids[index].ptr = ptrResourcesStartIndex + index;
								if( index >= ptrMapMenoryManagment.currentUserIndex )
									ptrMapMenoryManagment.currentUserIndex = index + 1;
								return ptrMapMenoryManagment.ids[index];
							}
						}
					} else {
						// 释放没意义的内存
						free( ptrMapMenoryManagment.ids );
						free( ptrMapMenoryManagment.mapPtrSizes );
						free( ptrMapMenoryManagment.currentDatasUserIndex );
						// 恢复以前的内存
						ptrMapMenoryManagment.mapPtrSizes = oldMapPtrSize;
						ptrMapMenoryManagment.ids = oldIds;
						ptrMapMenoryManagment.datas = odlDatas;
						ptrMapMenoryManagment.currentDatasUserIndex = oldCurrentDatasUserIndex;
					}

				} else {
					// 释放没意义的内存
					free( ptrMapMenoryManagment.ids );
					free( ptrMapMenoryManagment.mapPtrSizes );
					// 恢复以前的内存
					ptrMapMenoryManagment.mapPtrSizes = oldMapPtrSize;
					ptrMapMenoryManagment.ids = oldIds;
					ptrMapMenoryManagment.currentDatasUserIndex = oldCurrentDatasUserIndex;
				}
			} else {
				// 释放没意义的内存
				free( ptrMapMenoryManagment.ids );
				// 恢复以前的内存
				ptrMapMenoryManagment.mapPtrSizes = oldMapPtrSize;
				ptrMapMenoryManagment.ids = oldIds;
			}
		} else
		// 恢复以前的内存
			ptrMapMenoryManagment.ids = oldIds;
		return defaultMapPtr;
	}
	return defaultMapPtr;
}

int mapFree(const MapPtr* mapPtr) {
	size_t index = mapPtr->ptr - ptrResourcesStartIndex, minIndex = getMinMapForeachIndex();
	if( mapPtr->type != map )
		return -1;
	if( index < minIndex && mapPtr->ptr == ptrMapMenoryManagment.ids[index].ptr ) {
		if( ptrMapMenoryManagment.datas[index] != NULL ) {
			free( ptrMapMenoryManagment.datas[index] );
			ptrMapMenoryManagment.datas[index] = NULL;
		}
		ptrMapMenoryManagment.mapPtrSizes[index] = 0;
		ptrMapMenoryManagment.ids[index].ptr = 0;
		return 0;
	}
	return 1;
}

size_t mapGC() {
	size_t index = 0, count = 0, minIndex = getMinMapForeachIndex();
	for( ; index < minIndex; ++index ) {
		if( ptrMapMenoryManagment.ids[index].ptr ) {
			// 大小与数据任意为 0, 则该数据不存在使用情况
			if( ptrMapMenoryManagment.datas[index] != NULL ) {
				free( ptrMapMenoryManagment.datas[index] );
				ptrMapMenoryManagment.datas[index] = NULL;
			}
			ptrMapMenoryManagment.ids[index].ptr = 0;
			ptrMapMenoryManagment.mapPtrSizes[index] = 0;
			++count;
			// 数据莫名其妙被使用的情况下, 应该给它释放
		} else if( ptrMapMenoryManagment.datas[index] != NULL ) {
			free( ptrMapMenoryManagment.datas[index] );
			ptrMapMenoryManagment.datas[index] = NULL;
		}
	}
	return count;
}

size_t mapManageFree() {
	size_t index = 0, count = 0, minIndex = getMinMapForeachIndex();
	for( ; index < minIndex; ++index ) {
		// 数据莫名其妙被使用的情况下, 应该给它释放
		if( ptrMapMenoryManagment.datas[index] != NULL )
			free( ptrMapMenoryManagment.datas[index] );
		// 当它被使用了,应该给它计数
		if( ptrMapMenoryManagment.ids[index].ptr )
			++count;
	}
	free( ptrMapMenoryManagment.ids );
	free( ptrMapMenoryManagment.mapPtrSizes );
	free( ptrMapMenoryManagment.datas );
	free( ptrMapMenoryManagment.currentDatasUserIndex );
	ptrMapMenoryManagment.ids = NULL;
	ptrMapMenoryManagment.mapPtrSizes = NULL;
	ptrMapMenoryManagment.datas = NULL;
	ptrMapMenoryManagment.currentDatasUserIndex = NULL;
	ptrMapMenoryManagment.size = ptrArrayMenoryManagment.currentUserIndex = 0;
	return count;
}

/**
 * @brief 拷贝映射对象数据
 * @param des 目标
 * @param src 源
 * @param count 拷贝个数
*/
static void mapCopyPairPtrPtr(PairPtr* des, PairPtr* src, const size_t count) {
	size_t index = 0;
	for( ; index < count; ++index )
		des[index] = src[index];
}

size_t mapInster(const MapPtr* mapPtr, PairPtr* pair, const MapInsterCallBack mapInsterCallBack) {
	size_t index = mapPtr->ptr - ptrResourcesStartIndex, count = 0, minIndex = getMinMapForeachIndex();
	// 当它被使用了,应该给它计数
	if( index < minIndex ) {
		PairPtr* datas = ptrMapMenoryManagment.datas[index];
		size_t* size = ptrMapMenoryManagment.mapPtrSizes + index;
		count = ptrMapMenoryManagment.currentDatasUserIndex[index];
		const int result = mapInsterCallBack( datas, size, pair );
		if( result ) {
			// 提交给代码进行赋值
			if( result == 1 ) {
				size_t currentMax = ptrMapMenoryManagment.mapPtrSizes[index];
				// 判定是否已经使用全部
				if( currentMax >= count ) {
					PairPtr* oldDatas = ptrMapMenoryManagment.datas[index];
					currentMax = sizePairPtrSize * (currentMax + newCreateSize);
					ptrMapMenoryManagment.datas[index] = malloc( currentMax );
					if( ptrMapMenoryManagment.datas != NULL ) {
						mapCopyPairPtrPtr( ptrMapMenoryManagment.datas[index], oldDatas, ptrMapMenoryManagment.currentDatasUserIndex[index] );
						count = ptrMapMenoryManagment.mapPtrSizes[index] = ptrMapMenoryManagment.mapPtrSizes[index] + newCreateSize;
						free( oldDatas );
					} else
					// 使用旧的内存
						ptrMapMenoryManagment.datas[index] = oldDatas;
				}
				// 如果无法申请内存, 则直接结束
				if( ptrMapMenoryManagment.currentDatasUserIndex[index] < ptrMapMenoryManagment.mapPtrSizes[index] ) {
					// 赋值
					ptrMapMenoryManagment.datas[index][ptrMapMenoryManagment.currentDatasUserIndex[index]] = *pair;
					ptrMapMenoryManagment.currentDatasUserIndex[index] = ptrMapMenoryManagment.currentDatasUserIndex[index] + 1;
				}
			}
		}
	}

	return count;
}

size_t mapGetPairs(const MapPtr* mapPtr, const MapForeachCallBack mapForeachCallBack) {
	size_t index = mapPtr->ptr - ptrResourcesStartIndex, count = 0, minIndex = getMinMapForeachIndex();
	// 当它被使用了,应该给它计数
	if( index < minIndex ) {
		const size_t mapPtrSize = min( ptrMapMenoryManagment.mapPtrSizes[ index ], ptrMapMenoryManagment.currentDatasUserIndex[ index ] );
		mapForeachCallBack( ptrMapMenoryManagment.datas[index], mapPtrSize );
		++count;
	}
	return count;
}
