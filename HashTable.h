#pragma once
#include<iostream>
#include<string>

using namespace std;

namespace First
{
	//关键字的结构体
	template<class K, class V>
	struct HashTableNode
	{
		K _key;//关键字
		V _value;//关键字对应的值
	};

	//存储状态
	enum Status
	{
		EMPTY,
		DELETE,
		EXITS
	};

	//仿函数将关键字转换为整型
	template <class K>
	struct _HashFunc
	{
		size_t operator()(const K& key)
		{
			return key;
		}
	};
	//特化
	template<>
	struct  _HashFunc<string>
	{
		size_t operator()(const string& key)
		{
			size_t iKey = 0;
			char* str = (char*)key.c_str();
			while (*str)
			{
				iKey += *str++;
			}
			return iKey;
		}
	};

	//哈希表的二次探测
	template <class K, class V>
	class HashTable
	{
		typedef HashTableNode<K, V> Node;
	public:
		//构造函数
		HashTable(size_t capacity = 10)
			:_capacity(capacity)
			, _size(0)
			, _tables(new Node[capacity])
			, _status(new Status[capacity])
		{
			//_status初始化为空
			for (size_t i = 0; i < _capacity; i++)
			{
				_status[i] = EMPTY;
			}
		}
		//析构函数
		~HashTable()
		{
			if (_tables)
			{
				delete[] _tables;
			}
			if (_status)
			{
				delete[] _status;
			}
		}
	public:
		//插入
		bool Insert(const K& key, const V& value)
		{
			_CheckCapacity();
			//计算哈希地址
			size_t index = HashFunc(key);
			//记录起始位置
			size_t start = index;

			//二次检测处理冲突
			size_t i = 1;
			while (_status[index] == EXITS)
			{
				//已经存在
				if (_tables[index]._key == key)
				{
					return false;
				}
				index = start + i*i;//二次探测
				index %= _capacity;
				i++;
			}

			_tables[index]._key = key;
			_tables[index]._value = value;
			_status[index] = EXITS;
			_size++;
			return true;
		}
		//删除
		bool Remove(const K& key)
		{
			size_t index = HashFunc(key);
			size_t start = index;
			size_t i = 1;
			while (_status[index] == EXITS)
			{
				if (_tables[index]._key == key)
				{
					_status[index] = DELETE;
					return true;
				}
				index = start + i*i;
				index %= _capacity;
				i++;
				if (index == start)
				{
					return false;
				}
			}
			return false;
		}
		//查找
		Node* Find(const K& key)
		{
			size_t index = HashFunc(key);
			size_t start = index;
			size_t i = 1;
			while (_status[index] == EXITS)
			{
				if (_tables[index]._key == key)
				{
					return &_tables[index];
				}
				index = start + i*i;
				index %= _capacity;
				i++;
				if (index == start)
				{
					return NULL;
				}
			}
			return NULL;
		}
		//打印
		void Print()
		{
			for (size_t i = 0; i < _capacity; i++)
			{

				cout << "[" << _status[i] << "," << _tables[i]._key
					<< "," << _tables[i]._value << "]" << "--";

			}
			cout << endl;
		}
	private:
		//除留取余寻找地址
		size_t HashFunc(const K& key)
		{
			_HashFunc<K> hf;
			return hf(key) % _capacity;
		}
		void Swap(HashTable<K, V>& tmp)
		{
			swap(_tables, tmp._tables);
			swap(_status, tmp._status);
			swap(_capacity, tmp._capacity);
			swap(_size, tmp._size);
		}
		//检查容量，扩容
		void _CheckCapacity()
		{
			//载荷因子大于等于7的时候扩容
			if (_size * 10 / _capacity == 7)
			{
				HashTable<K, V> tmp(2 * _capacity);
				for (size_t i = 0; i < _capacity; i++)
				{
					if (_status[i] == EXITS)
						tmp.Insert(_tables[i]._key, _tables[i]._value);
				}
				Swap(tmp);

			}

		}
	protected:
		Node* _tables;		//哈希表指针
		Status* _status;	//状态指针
		size_t _capacity;	//容量
		size_t _size;		//存储数据的大小
	};


	void TestHashTable()
	{
		HashTable<int, int> ht;
		ht.Insert(89, 89);
		ht.Insert(18, 18);
		ht.Insert(49, 49);
		ht.Insert(58, 58);
		ht.Insert(9, 9);
		ht.Print();
		ht.Remove(9);
		ht.Print();
		ht.Insert(99, 99);
		ht.Print();
		//cout << ht.Find(10)->_key << endl;

	}

	void TestDict()
	{
		HashTable<string, string> dict;
		dict.Insert("dict", "字典");
		dict.Insert("hash", "哈希");
		dict.Insert("function", "函数");
		dict.Insert("arithmetic", "四则运算");
		dict.Insert("carry", "进位");
		dict.Insert("odd", "奇数");
		dict.Insert("even", "偶数");
		dict.Insert("number", "数字");
		dict.Insert("heap", "堆");
		dict.Print();
		HashTableNode<string, string> *ret = dict.Find("hash");
		if (ret)
		{
			cout << "hash的中文为：" << ret->_value << endl;
		}
		else
		{
			cout << "not find" << endl;
		}
	}
}

