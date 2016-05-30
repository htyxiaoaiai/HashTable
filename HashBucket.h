#pragma once
#include<iostream>
#include<string>
#include<vector>

namespace Second
{
	//哈希桶节点
	template<class K, class V>
	struct HashBucketNode
	{
	public:
		//构造函数
		HashBucketNode(const K& key, const V& value)
			:_key(key)
			, _value(value)
			, _next(NULL)
		{}
	public:
		K _key;		//关键字
		V _value;	//数值
		HashBucketNode<K, V>* _next;	//指向下一个节点的指针
	};


	// BKDR Hash Function
	unsigned int BKDRHash(char *str)
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;

		while (*str)
		{
			hash = hash * seed + (*str++);
		}

		return (hash & 0x7FFFFFFF);
	}

	//仿函数将关键字转换为整型的key
	template<class K>
	struct ConversToInt
	{
		size_t operator()(const K& key)
		{
			return key;
		}
	};

	//特化
	template<>
	struct ConversToInt<string>
	{
		size_t operator()(const string& key)
		{
			return BKDRHash((char*)key.c_str());
		}
	};
	template<>
	struct ConversToInt<char*>
	{
		size_t operator()(const char* key)
		{
			return BKDRHash((char*)key);
		}
	};


	//哈希桶
	template<class K, class V>
	class HashBucket
	{
		typedef HashBucketNode<K, V> Node;
		typedef HashBucket<K, V> Table;
	public:
		//构造函数
		//无参
		HashBucket()
			:_size(0)
		{}
		//有参构造函数
		HashBucket(size_t capacity)
			:_size(0)
		{
			_tables.resize(_GetNextPrime(capacity));
		}

		//拷贝构造
		HashBucket(const Table& t)
			:_size(t._size)
		{
			_tables.resize(t._tables.size());
			for (size_t i = 0; i < t._tables.size(); i++)
			{
				Node* cur = t._tables[i];
				while (cur)
				{
					Insert(cur->_key, cur->_value);
					cur = cur->_next;
				}
			}
		}
		//赋值运算符的重载
		Table& operator=(Table t)
		{
			_tables.swap(t._tables);
			swap(_size, t._size);
			return *this;
		}

		//析构函数
		~HashBucket()
		{
			for (size_t i = 0; i < _tables.size(); i++)
			{
				Node* cur = _tables[i];
				while (cur)
				{
					Node* del = cur;
					cur = cur->_next;
					delete del;
				}
				_tables[i] = NULL;
			}
		}
	public:
		//插入
		bool Insert(const K& key, const V& value)
		{
			//检查是否需要增容
			_CheckCapacity();
			size_t index = _HashFunc(key, _tables.size());
			Node* cur = _tables[index];

			//检出是否已经存在
			while (cur)
			{
				if (cur->_key == key)
				{
					return false;
				}
				cur = cur->_next;
			}

			//头插
			Node* newNode = new Node(key, value);
			newNode->_next = _tables[index];
			_tables[index] = newNode;
			_size++;
			return true;
		}

		//删除
		bool Remove(const K& key)
		{
			size_t index = _HashFunc(key, _tables.size());
			Node* cur = _tables[index];

			if (cur == NULL)
			{
				return false;
			}

			//头删
			if (cur->_key == key)
			{
				_tables[index] = cur->_next;
				delete cur;
				return true;
			}

			//非头结点
			Node* prev = cur;
			cur = cur->_next;
			while (cur)
			{
				if (cur->_key == key)
				{
					prev->_next = cur->_next;
					delete cur;
					return true;
				}
				prev = cur;
				cur = cur->_next;
			}

			return false;
		}
		//查找
		Node* Find(const K& key)
		{
			size_t index = _HashFunc(key, _tables.size());
			Node* cur = _tables[index];

			while (cur)
			{
				if (cur->_key == key)
				{
					return cur;
				}
				cur = cur->_next;
			}
			return NULL;
		}
		//打印
		void PrintTable()
		{
			for (size_t i = 0; i < _tables.size(); i++)
			{
				Node* cur = _tables[i];
				printf("_tables[%d]->", i);
				while (cur)
				{
					cout << "[" << cur->_key << "," << cur->_value << "]" << "->";
					cur = cur->_next;
				}
				cout << "NULL" << endl;
			}
		}
	private:
		//计算哈希地址
		size_t _HashFunc(const K& key, size_t cappcity)
		{
			ConversToInt<K> hf;
			return hf(key) % cappcity;
		}
		//获取下一个素数
		size_t _GetNextPrime(size_t capacity)
		{
			// 使用素数表对齐做哈希表的容量，降低哈希冲突
			const int _PrimeSize = 28;
			static const unsigned long _PrimeList[_PrimeSize] =
			{
				53ul,         97ul,         193ul,       389ul,       769ul,
				1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
				49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
				1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
				50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
				1610612741ul, 3221225473ul, 4294967291ul
			};

			//返回比容量大的较大的素数
			for (size_t i = 0; i < _PrimeSize; i++)
			{
				if (_PrimeList[i] > capacity)
				{
					return _PrimeList[i];
				}
			}
			//找到最后素数表里没有比容量大的素数，则直接返回最大的素数
			return _PrimeList[_PrimeSize - 1];
		}
		//检查是否需要扩容
		void _CheckCapacity()
		{
			//需要扩容
			if (_size == _tables.size())
			{
				//求扩容的容量
				size_t nextPrime = _GetNextPrime(_tables.size());
				//开辟一个nextPrime大小的顺序表
				vector<Node*> newTable(nextPrime);

				//将原来的数据拷贝并扩容
				for (size_t i = 0; i < _tables.size(); i++)
				{
					size_t index = _HashFunc(_tables[i]->_key, newTable.size());
					Node* cur = _tables[i];

					while (cur)
					{
						//摘节点
						Node* tmp = cur;
						cur = cur->_next;

						//头插
						tmp->_next = newTable[index];
						newTable[index] = tmp;
					}
					_tables[i] = NULL;
				}
				//交换
				this->_tables.swap(newTable);
			}
		}
	protected:
		vector<Node*> _tables; //存储数据的哈希桶
		size_t _size;	//哈希桶中存放的数据个数
	};


	void TestHashBucket()
	{
		HashBucket<int, int> hb(10);
		hb.Insert(1, 1);
		hb.Insert(2, 2);
		hb.Insert(3, 3);
		hb.Insert(54, 54);
		hb.Insert(109, 109);
		hb.Insert(56, 56);
		hb.PrintTable();

		HashBucket<int, int> hb1;
		hb1 = hb;
		hb1.PrintTable();
	}
	void TestFind()
	{
		HashBucket<int, int> hb(10);
		hb.Insert(1, 1);
		hb.Insert(2, 2);
		hb.Insert(3, 3);
		hb.Insert(54, 54);
		hb.Insert(109, 109);
		hb.Insert(56, 56);
		hb.PrintTable();

		HashBucketNode<int, int>* ret;
		ret = hb.Find(56);
		if (ret)
		{
			ret->_value = 1000000;
		}
		else
		{
			cout << "not find" << endl;
		}
		hb.PrintTable();
	}

	void TestRemove()
	{
		HashBucket<int, int> hb(10);
		hb.Insert(1, 1);
		hb.Insert(2, 2);
		hb.Insert(3, 3);
		hb.Insert(54, 54);
		hb.Insert(109, 109);
		hb.Insert(56, 56);
		hb.PrintTable();
		hb.Remove(0);
		hb.PrintTable();
	}

	void TestExpend()
	{
		HashBucket<int, int> hb(10);
		for (size_t i = 0; i < 53; i++)
		{
			hb.Insert(i, i);
		}
		hb.Insert(54, 54);
		hb.Insert(55, 55);
		hb.Insert(98, 98);
		hb.PrintTable();
	}

	void TestDict()
	{
		HashBucket<string, string> dict;
		dict.Insert("助手", "assist");
		dict.Insert("删除", "delete、remove、erase");
		dict.Insert("土豪", "peter");
		dict.Insert("有钱人", "yang");
		dict.PrintTable();

		HashBucketNode<string, string>* ret;
		ret = dict.Find("土豪");
		if (ret)
		{
			cout << ret->_key << "的英文为：" << ret->_value << endl;
		}
		else
		{
			cout << "not Find" << endl;
		}
	}
}