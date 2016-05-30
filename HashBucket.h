#pragma once
#include<iostream>
#include<string>
#include<vector>

namespace Second
{
	//��ϣͰ�ڵ�
	template<class K, class V>
	struct HashBucketNode
	{
	public:
		//���캯��
		HashBucketNode(const K& key, const V& value)
			:_key(key)
			, _value(value)
			, _next(NULL)
		{}
	public:
		K _key;		//�ؼ���
		V _value;	//��ֵ
		HashBucketNode<K, V>* _next;	//ָ����һ���ڵ��ָ��
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

	//�º������ؼ���ת��Ϊ���͵�key
	template<class K>
	struct ConversToInt
	{
		size_t operator()(const K& key)
		{
			return key;
		}
	};

	//�ػ�
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


	//��ϣͰ
	template<class K, class V>
	class HashBucket
	{
		typedef HashBucketNode<K, V> Node;
		typedef HashBucket<K, V> Table;
	public:
		//���캯��
		//�޲�
		HashBucket()
			:_size(0)
		{}
		//�вι��캯��
		HashBucket(size_t capacity)
			:_size(0)
		{
			_tables.resize(_GetNextPrime(capacity));
		}

		//��������
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
		//��ֵ�����������
		Table& operator=(Table t)
		{
			_tables.swap(t._tables);
			swap(_size, t._size);
			return *this;
		}

		//��������
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
		//����
		bool Insert(const K& key, const V& value)
		{
			//����Ƿ���Ҫ����
			_CheckCapacity();
			size_t index = _HashFunc(key, _tables.size());
			Node* cur = _tables[index];

			//����Ƿ��Ѿ�����
			while (cur)
			{
				if (cur->_key == key)
				{
					return false;
				}
				cur = cur->_next;
			}

			//ͷ��
			Node* newNode = new Node(key, value);
			newNode->_next = _tables[index];
			_tables[index] = newNode;
			_size++;
			return true;
		}

		//ɾ��
		bool Remove(const K& key)
		{
			size_t index = _HashFunc(key, _tables.size());
			Node* cur = _tables[index];

			if (cur == NULL)
			{
				return false;
			}

			//ͷɾ
			if (cur->_key == key)
			{
				_tables[index] = cur->_next;
				delete cur;
				return true;
			}

			//��ͷ���
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
		//����
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
		//��ӡ
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
		//�����ϣ��ַ
		size_t _HashFunc(const K& key, size_t cappcity)
		{
			ConversToInt<K> hf;
			return hf(key) % cappcity;
		}
		//��ȡ��һ������
		size_t _GetNextPrime(size_t capacity)
		{
			// ʹ���������������ϣ������������͹�ϣ��ͻ
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

			//���ر�������Ľϴ������
			for (size_t i = 0; i < _PrimeSize; i++)
			{
				if (_PrimeList[i] > capacity)
				{
					return _PrimeList[i];
				}
			}
			//�ҵ������������û�б����������������ֱ�ӷ�����������
			return _PrimeList[_PrimeSize - 1];
		}
		//����Ƿ���Ҫ����
		void _CheckCapacity()
		{
			//��Ҫ����
			if (_size == _tables.size())
			{
				//�����ݵ�����
				size_t nextPrime = _GetNextPrime(_tables.size());
				//����һ��nextPrime��С��˳���
				vector<Node*> newTable(nextPrime);

				//��ԭ�������ݿ���������
				for (size_t i = 0; i < _tables.size(); i++)
				{
					size_t index = _HashFunc(_tables[i]->_key, newTable.size());
					Node* cur = _tables[i];

					while (cur)
					{
						//ժ�ڵ�
						Node* tmp = cur;
						cur = cur->_next;

						//ͷ��
						tmp->_next = newTable[index];
						newTable[index] = tmp;
					}
					_tables[i] = NULL;
				}
				//����
				this->_tables.swap(newTable);
			}
		}
	protected:
		vector<Node*> _tables; //�洢���ݵĹ�ϣͰ
		size_t _size;	//��ϣͰ�д�ŵ����ݸ���
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
		dict.Insert("����", "assist");
		dict.Insert("ɾ��", "delete��remove��erase");
		dict.Insert("����", "peter");
		dict.Insert("��Ǯ��", "yang");
		dict.PrintTable();

		HashBucketNode<string, string>* ret;
		ret = dict.Find("����");
		if (ret)
		{
			cout << ret->_key << "��Ӣ��Ϊ��" << ret->_value << endl;
		}
		else
		{
			cout << "not Find" << endl;
		}
	}
}