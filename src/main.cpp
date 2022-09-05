﻿// Sqlite3.cpp: 定义应用程序的入口点。
//

#include "main.h"
#include "DataStruct.h"
using namespace std;
/**
 * @brief 测试数组
*/
void testArray() {
	setNewMemorySize( 4 );
	ArrayPtr arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	// 释放数组
	int isFree = arrayFree( &arrayPtr );
	// 初始化数组大小
	size_t size = arrayMalloc( &arrayPtr, 1024 );
	arrayPtr = arrayCreate();
	size = arrayMalloc( &arrayPtr, 1024 );
	ResourcePtr pr;
	StrPtr strPtr;
	pr.strPtr = &strPtr;
	strPtr.ptr = 22;
	// 设置数组元素
	size = arraySetIndex( &arrayPtr, 100, &pr );
	ResourcePtr pr2;
	// 获取数组元素
	size = arrayGetIndex( &arrayPtr, 100, &pr2 );
	cout << "arrayPtr  start " << endl;
	cout << "pr2  = " << pr2.strPtr->ptr << endl;
	cout << "arrayPtr  end " << endl;

	isFree = arrayFree( &arrayPtr );
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	arrayPtr = arrayCreate();
	// 自动释放没作用的数组
	size_t arrayGc = arrayGC();
	// 释放全部数组
	arrayManageFree();
}

/**
 * @brief 数组遍历函数
 * @param resourcePtr 下标的资源
 * @param arraySize 数组大小
*/
void userDataCallBack(ResourcePtr* resourcePtr, const size_t arraySize) {
	if( arraySize >= 100 ) {
		cout << "resourcePtr->strPtr->ptr " << endl;
		cout << resourcePtr[100].strPtr->ptr << endl;
		resourcePtr[100].strPtr->ptr = 323;
		cout << "change over " << endl;
		arrayCreate();
	}
}

/**
 * @brief 遍历数组
*/
void testForeachArray() {
	ResourcePtr pr;
	StrPtr strPtr;
	ResourcePtr pr2;
	ArrayPtr arrayPtr = arrayCreate();
	size_t size = arrayMalloc( &arrayPtr, 1024 );
	pr.strPtr = &strPtr;
	strPtr.ptr = 22;
	size = arraySetIndex( &arrayPtr, 100, &pr );
	arrayForeach( &arrayPtr, userDataCallBack );
	size = arrayGetIndex( &arrayPtr, 100, &pr2 );
	cout << " pr2.strPtr->ptr" << endl;
	cout << pr2.strPtr->ptr << endl;
	arrayManageFree();
}

/**
 * @brief 测试映射创建与释放
*/
void testMap() {
	// 创建映射对象
	MapPtr mapPtr = mapCreate();
	// 释放映射对象
	size_t mapGc = mapFree( &mapPtr );
	// 释放所有映射对象
	mapGc = mapManageFree();
}

/**
 * @brief 映射插入元素回调函数
 * @param datas 映射列表中的数据,请不要对该指针进行重新分配, 但是它却可以进行重新赋值
 * @param size 引用的 datas 大小, 重置 datas 时, 应当重置该数值
 * @param pair 用户提供的数据, 该数据由用户调用插入方法时传递
 * @result 返回 0 表示结束, 该返回会导致失去插入功能, -1 表示继续循环, 返回 1 时, 将会由 mapInster 进行插入
*/
int mapInsterCallBack(PairPtr* datas, size_t* size, PairPtr* pair) {
	return 1;
}

/**
 * @brief 遍历元素时回调函数
 * @param datas 映射对象的数组
 * @param size 映射对象的数组的大小
*/
void mapForeachCallBack(PairPtr* datas, const size_t size) {
	size_t index = 0;
	for( ; index < size; ++index ) {
		cout << "datas[ " << index << " ] start" << endl;
		cout << datas[index].first.strPtr->ptr << endl;
		cout << datas[index].second.strPtr->ptr << endl;

		cout << "datas[ " << index << " ] end" << endl;
	}
}

/**
 * @brief 测试映射的插入与遍历
*/
void testMapInster() {
	const MapPtr mapPtr = mapCreate();
	for( int index = 0; index < 10000; ++index )
		mapCreate();
	PairPtr pair1, pair2;
	StrPtr resourcePtr1, resourcePtr2;
	pair1.first.strPtr = &resourcePtr1;
	pair1.second.strPtr = &resourcePtr2;
	resourcePtr1.ptr = 33;
	resourcePtr2.ptr = 44;
	// 插入数据
	mapInster( &mapPtr, &pair1, mapInsterCallBack );
	// 遍历数据
	mapGetPairs( &mapPtr, mapForeachCallBack );
	mapManageFree();
}

/**
 * @brief 字符串资源测试
*/
void strTest() {
	BaseType str, str2;
	StrPtr strPtr = strCreate();
	char cStr[] = "123213124";
	str.dataPtr = cStr;
	strCSet( &strPtr, str, 222 );
	const char* stdCString = strGetStdCString( &strPtr, 1024, 0 );
	cStr[1] = 0;
	size_t index = 0;
	stdCString = strGetStdCString( &strPtr, 1024, &index );
	size_t charWidth = 0;
	size_t getWidth = strGet( &strPtr, &str2, &charWidth );

	strManageFree();
}

#include <fstream>

int main() {
	cout << "Hello CMake. " << (1024 << 2) << endl;
	testMapInster();
	testArray();
	testForeachArray();
	strTest();
	StrPtr file = strCreate();
	StrPtr content = strCreate();
	StrPtr testCAppend = strCreate();
	BaseType base;
	base.dataPtr = "./file.txt";
	std::ofstream oFile( "./file.txt" );
	oFile << "这是一个文件";
	oFile.close();
	strCSet( &file, base, 100 );
	const char* cStrBuff = strGetStdCString( &file, 0, NULL );
	cout << " strGetStdCString( &file, 0, &end ) : => " << cStrBuff << endl;
	BaseType base2;
	size_t end = 0;
	base2.dataPtr = "./file.txt";
	strCAppend( &testCAppend, base2, 100 );
	cStrBuff = strGetStdCString( &testCAppend, 0, &end );
	cout << " strGetStdCString( &testCAppend, 0, &end ) : => " << cStrBuff << endl;
	base2.dataPtr = "这是一个文件";
	strCAppend( &testCAppend, base2, 100 );
	cStrBuff = strGetStdCString( &testCAppend, 0, &end );
	cout << " strGetStdCString( &testCAppend, 0, &end ) 1 " << cStrBuff << endl;
	//strCAppend( &testCAppend, base2, 100 );
	//strReadFile( &file, &content );
	//cStrBuff = strGetStdCString( &content, 0, &end );
	//cout << "strReadFile( &file, &content ); : => " << cStrBuff << endl;
	strManageFree();
	_CrtDumpMemoryLeaks();
	return 0;
}
