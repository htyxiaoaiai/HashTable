#pragma once
#include<iostream>
#include<string>

using namespace std;

namespace First
{
	//�ؼ��ֵĽṹ��
	template<class K, class V>
	struct HashTableNode
	{
		K _key;//�ؼ���
		V _value;//�ؼ��ֶ�Ӧ��ֵ
	};

	//�洢״̬
	enum Status
	{
		EMPTY,
		DELETE,
		EXITS
	};

	//�º������ؼ���ת��Ϊ����
	template <class K>
	struct _HashFunc
	{
		size_t operator()(const K& key)
		{
			return key;
		}
	};
	//�ػ�
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

	//��ϣ��Ķ���̽��
	template <class K, class V>
	class HashTable
	{
		typedef HashTableNode<K, V> Node;
	public:
		//���캯��
		HashTable(size_t capacity = 10)
			:_capacity(capacity)
			, _size(0)
			, _tables(new Node[capacity])
			, _status(new Status[capacity])
		{
			//_status��ʼ��Ϊ��
			for (size_t i = 0; i < _capacity; i++)
			{
				_status[i] = EMPTY;
			}
		}
		//��������
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
		//����
		bool Insert(const K& key, const V& value)
		{
			_CheckCapacity();
			//�����ϣ��ַ
			size_t index = HashFunc(key);
			//��¼��ʼλ��
			size_t start = index;

			//���μ�⴦���ͻ
			size_t i = 1;
			while (_status[index] == EXITS)
			{
				//�Ѿ�����
				if (_tables[index]._key == key)
				{
					return false;
				}
				index = start + i*i;//����̽��
				index %= _capacity;
				i++;
			}

			_tables[index]._key = key;
			_tables[index]._value = value;
			_status[index] = EXITS;
			_size++;
			return true;
		}
		//ɾ��
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
		//����
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
		//��ӡ
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
		//����ȡ��Ѱ�ҵ�ַ
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
		//�������������
		void _CheckCapacity()
		{
			//�غ����Ӵ��ڵ���7��ʱ������
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
		Node* _tables;		//��ϣ��ָ��
		Status* _status;	//״ָ̬��
		size_t _capacity;	//����
		size_t _size;		//�洢���ݵĴ�С
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
		dict.Insert("dict", "�ֵ�");
		dict.Insert("hash", "��ϣ");
		dict.Insert("function", "����");
		dict.Insert("arithmetic", "��������");
		dict.Insert("carry", "��λ");
		dict.Insert("odd", "����");
		dict.Insert("even", "ż��");
		dict.Insert("number", "����");
		dict.Insert("heap", "��");
		dict.Print();
		HashTableNode<string, string> *ret = dict.Find("hash");
		if (ret)
		{
			cout << "hash������Ϊ��" << ret->_value << endl;
		}
		else
		{
			cout << "not find" << endl;
		}
	}
}

