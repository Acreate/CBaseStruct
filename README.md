# CDataStruct

#### 介绍
数据结构
#### Visual Studio 调试
![image](https://user-images.githubusercontent.com/42111480/188409104-ca956093-0de5-4093-9b2b-73f391bba1eb.png)
#### clion 调试
![image](https://user-images.githubusercontent.com/42111480/188410008-586e3c9f-b908-4779-875e-6ffb7907550c.png)

![image](https://user-images.githubusercontent.com/42111480/188409875-57951c12-d977-4312-a38c-36524d1855cb.png)


#### 申请内存的设置

```c++
// 每次申请的内存都应该是 4 个单位
setNewMemorySize( 4 );
// 获取每次申请的内存单位个数
size_t newCount = getNewMemorySize() ;
```

#### 数组
```c++
        // 申请一个数组
        ArrayPtr arrayPtr = arrayCreate();
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
	// 自动释放没作用的数组
	size_t arrayGc = arrayGC();
	// 释放全部数组
	arrayManageFree();

```
```c++

/**
 * @brief 数组遍历函数
 * @param resourcePtr 下标的资源
 * @param index 下标
 * @param arraySize 数组大小
 * @param attach 附加数据， 值为 0 (* attach == 0)时， 遍历下一个元素， 值为 -1 (* attach == -1)时，删除该数组，并且结束循环
*/
void userDataCallBack(ResourcePtr* resourcePtr, const size_t index, const size_t arraySize, void* attach) {
	if( index == 100 ) {
		cout << "resourcePtr->strPtr->ptr " << endl;
		cout << resourcePtr->strPtr->ptr << endl;
		resourcePtr->strPtr->ptr = 323;
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
```

#### 映射
```c++
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
```


```c++
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
```

### 字符串
```c++
/**
 * @brief 字符串资源测试
*/
void strTest() {
	BaseType str, str2;
	// 创建字符串
	StrPtr strPtr = strCreate();
	char cStr[] = "123213124";
	// 给字符串赋值
	strCSet( &strPtr, cStr, 222 );
	// 获取 C 字符串
	const char* stdCString = strGetStdCString( &strPtr, 1024, 0 );
	cStr[1] = 0;
	size_t index = 0;
	stdCString = strGetStdCString( &strPtr, 1024, &index );
	size_t charWidth = 0;
	// 获取资源字符串
	size_t getWidth = strGet( &strPtr, &str2, &charWidth );
	// 释放
	strManageFree();
}
```
```c++
#include <fstream>
/**
 * @brief 测试字符串追加的问题
*/
void testStrAppendCStr() {
	// 创建字符串
	StrPtr file = strCreate();
	StrPtr content = strCreate();
	StrPtr testCAppend = strCreate();
	BaseType base;
	std::ofstream oFile( "./file.txt" );
	oFile << "这是一个被写入内容的文件";
	oFile.close();
	strCSet( &file, "./file.txt", 100 );
	size_t width;
	strGet( &file, &base, &width );
	cout << " strGet( &file, &base, &width ); : => " << (char*)base.dataPtr << endl;
	const char* cStrBuff = strGetStdCString( &file, 0, NULL );
	cout << " strGetStdCString( &file, 0, &end ) : => " << cStrBuff << endl;
	BaseType base2;
	size_t end = 0;
	// 追加字符串
	strCAppend( &testCAppend, "./file.txt", 100 );
	cStrBuff = strGetStdCString( &testCAppend, 0, &end );
	cout << " strGetStdCString( &testCAppend, 0, &end ) : => " << cStrBuff << endl;
	strCAppend( &testCAppend, "这是一个文件", 100 );
	cStrBuff = strGetStdCString( &testCAppend, 0, &end );
	cout << " strGetStdCString( &testCAppend, 0, &end ) 1 " << cStrBuff << endl;
	strCAppend( &testCAppend, "cStrBuff = strGetStdCString( &testCAppend, 0, &end );", 100 );
	cStrBuff = strGetStdCString( &testCAppend, 0, NULL );
	cout << "cStrBuff = strGetStdCString( &testCAppend, 0, NULL ); : => " << cStrBuff << endl;
	// 读取文件
	int readFile = strReadFile( &file, &content );
	cStrBuff = strGetStdCString( &content, 0, &end );
	cout << "strReadFile( &file, &content ); : => " << cStrBuff << endl;
	strManageFree();
}
```
