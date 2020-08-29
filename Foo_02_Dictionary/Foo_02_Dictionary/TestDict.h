#pragma once
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1;
#include "TestDict.h"
#include<hash_set>

template<class TKey, class TValue>
class Dictionary
{
public:
	virtual ~Dictionary() = default;

	virtual const TValue& Get(const TKey& key) const = 0;
	virtual void Set(const TKey& key, const TValue& value) = 0;
	virtual bool IsSet(const TKey& key) const = 0;
};

template<class TKey>
class NotFoundException : public std::exception
{
public:
	virtual const TKey& GetKey() const noexcept = 0;
};

template<class TKey>
class ElemNotFound : public NotFoundException<TKey>
{
private:
	const TKey except_key;

public:
	ElemNotFound(TKey key) : except_key(key) {};
	const TKey& GetKey() const noexcept;
};

template<class TKey, class TValue>
class RBTree {
private:
	struct list {
		TKey key;
		TValue value;
		list *next;
	};
	bool is_red;								 //��� ����
	RBTree<TKey, TValue> *left;				 //����� ���
	RBTree<TKey, TValue> *right;			 //������ ���
	RBTree<TKey, TValue> *parent;			 //����
	std::size_t hash;						 //�������� ����
	bool is_list;
	list *body;								 //���� (���������� ������ ��� �����)

	void balance();
	void red_parent_and_uncle();
	void low_black_uncle();
	void large_black_uncle();
	void remove_body(list *body);

public:
	RBTree();
	RBTree(RBTree<TKey, TValue> *parent);																			  //����������� ��� �����
	RBTree(TKey key, TValue value, RBTree<TKey, TValue> *head);												  //�����������, ������������ ������� � ������������ ���.
	RBTree(RBTree &a) : left(a.left), right(a.right), hash(a.hash), body(a.body), is_red(is_red), parent(a.parent), is_list(a.is_list) {};
	~RBTree();
	void insert(RBTree<TKey, TValue> *head, TKey key, TValue value);
	const RBTree<TKey, TValue> *find(const TKey &key, size_t &key_hash) const;
	TValue& GetValue(const TKey& key) const;
};

template<class TKey, class TValue>
class TestDict : public Dictionary<TKey, TValue>
{
public:
	

	RBTree<TKey, TValue> head;
public:
	void Set(const TKey& key, const TValue& value);
	bool IsSet(const TKey& key) const;
	const TValue& Get(const TKey& key) const;
	TestDict();
	~TestDict();
};

/*
�������� ����������:
	- ������������� ���� � ���, �.�. ��������, ��� TKey ��� ������� less().
	- ������������ ��� ��� ���������� � ������ ���� "��� - ���������"
	- �� ��������� ����� ����������� ������
	- ������ ������� ������ - ���� "TKey-TValue"

������:
	- ������������� TKey � ���
	- ����� ��������������� ������� � ���
	- ����� ������ ��� � ������
	- ���� � ������ ��� ������ ����, ��������� ����������.

����������:
	- ������������� TKey � ���
	- ���������� �� ������� � ���
	- ����������:
		- �������� ����� ������� � ��������������� ������
	- �� ����������:
		- �������� ����� ������� � ���
		- ������� ����� ������ � ��������� ��� � ���
*/

template<class TKey>
inline const TKey & ElemNotFound<TKey>::GetKey() const noexcept
{
	return except_key;
}

template<class TKey, class TValue>
void TestDict<TKey, TValue>::Set(const TKey & key, const TValue & value)
{
	//���������� �������� � ��� � � ������.
	RBTree<TKey, TValue> tmp(key, value, &head);
}

template<class TKey, class TValue>
bool TestDict<TKey, TValue>::IsSet(const TKey & key) const
{
	size_t tmp = std::hash<TKey>()(key);
	if (head.find(key, tmp) != nullptr)
		return true;
	return false;
}

template<class TKey, class TValue>
const TValue & TestDict<TKey, TValue>::Get(const TKey & key) const
{
	size_t hash = std::hash<TKey>()(key);
	const RBTree<TKey, TValue> *tmp = head.find(key, hash);
	if (tmp == nullptr)
		throw ElemNotFound<TKey>(key);


	return tmp->GetValue(key);
}

template<class TKey, class TValue>
TestDict<TKey, TValue>::TestDict()
{

}

template<class TKey, class TValue>
TestDict<TKey, TValue>::~TestDict()
{
	
}


template<class TKey, class TValue>
void RBTree<TKey, TValue>::balance()
{
	//������ "������� ���� � ����"
	red_parent_and_uncle();
	//������ "׸���� ����, ������� ����"
	low_black_uncle();
	large_black_uncle();
}

template<class TKey, class TValue>
void RBTree<TKey, TValue>::red_parent_and_uncle()
{
	//��������, �� �������� �� ������� ������, � �� �������� �� �� �������.
	if (parent == NULL) {
		if (is_red)
			is_red = false;
		return;
	}
	//��������, ���������� �� � �������� �������. ���� �� ����������, �� ������� ����� ���� ������ ����� - ������ ��� �� �� ��������.
	if (parent->parent == NULL)
		return;
	//��������, �������� �� ���� � ���� ��������. ���� ���, return
	if (!(parent->parent->left->is_red && parent->parent->right->is_red))
		return;
	//�������� "��������" ������ 
	parent->parent->left->is_red = false;
	parent->parent->right->is_red = false;
	parent->parent->is_red = true;
	//����������� �������� ���� �� �������� ��� �������
	parent->parent->balance();
}

template<class TKey, class TValue>
void RBTree<TKey, TValue>::low_black_uncle()
{
	//��������, �� �������� �� ������� ������.
	if (parent == NULL)
		return;

	//��������, ���������� �� � �������� �������.
	if (parent->parent == NULL)
		return;

	//�������� �� ������������ ������� "������� ����, ������ ����"
	if (!(parent->parent->left->is_red != parent->parent->right->is_red && parent->is_red))
		return;

	if (parent == parent->parent->left && this == parent->right) {
		parent->right = this->left;
		this->left->parent = parent;

		this->left = parent;
		parent->parent->left = this;
		this->parent = parent->parent;

		this->left->parent = this;				//� left ��� ����� ������ ����.

		this->left->large_black_uncle();
	}
	else {															   //else ����� ����� ������, ��� � ���������� ��������� ����������� if �������� ���� ����� ����� ����� ����. ���� parent->parent == nullptr
		if (parent == parent->parent->right && this == parent->left) {
			parent->left = this->right;
			this->right->parent = parent;
			this->right = parent;
			parent->parent->right = this;

			this->parent = parent->parent;
			this->right->parent = this;				//� right ��� ����� ������ ����.

			this->right->large_black_uncle();
		}
	}
	return;
}

template<class TKey, class TValue>
void RBTree<TKey, TValue>::large_black_uncle()
{
	if (parent == nullptr)
		return;

	if (parent->parent == nullptr)
		return;

	if (!(parent->parent->left->is_red != parent->parent->right->is_red && parent->is_red))
		return;

	RBTree<TKey, TValue> par(*parent), grpar(*(parent->parent));

	par.is_red = false;
	grpar.is_red = true;

	if (parent == parent->parent->left && this == parent->left) {
		par.parent = parent->parent->parent;					  //"����" ������ ����� ����� �������
		par.right = parent;									  //������ ��� "����" - "���", ����� ������� � ������ ����
		grpar.parent = parent->parent;							  //���� "����" - "����" ����� ������� � ������ ����
																  //
		grpar.left = parent->right;								  //����� ����� "����" ������ ������ ��� ����
																  //��������� parent � ������ ���� ���� ������ �� �����, �.�. "���" ����� ������� � ������ ����.
		parent->parent->right->parent = parent;					  //���� ������ ����� "����", �� ��� ����� ���������� � ������ ����, ������� ������ ��������.
																  //���� ��� �������� ����� ����� "����", ��� ��� ��������� ������ �� ����.
		*(parent->parent) = par;								  //���������� "����" � ������ ����
		*parent = grpar;											  //���������� "����" � ������ ����
		parent = parent->parent;								  //�������� ��������� �� ������, � ������� ����� ��������� ���, ����� - ����
	}
	else {															   //else ����� ����� ������, ��� � ���������� ��������� ����������� if �������� ���� ����� ����� ����� ����. ���� parent->parent == nullptr
		if (parent == parent->parent->right && this == parent->right) {
			par.parent = parent->parent->parent;					  //"����" ������ ����� ����� �������
			par.left = parent;										  //����� ��� "����" - "���", ����� ������� � ������ ����
			grpar.parent = parent->parent;							  //���� "����" - "����" ����� ������� � ������ ����
																	  //
			grpar.right = parent->left;								  //������ ����� "����" ������ ����� ��� ����
																	  //��������� parent � ������ ���� ���� ������ �� �����, �.�. "���" ����� ������� � ������ ����.
			parent->parent->left->parent = parent;					  //���� ������ ����� "����", �� ��� ����� ���������� � ������ ����, ������� ������ ��������.
																	  //���� ��� �������� ����� ����� "����", ��� ��� ��������� ������ �� ����.
			*(parent->parent) = par;								  //���������� "����" � ������ ����
			*parent = grpar;											  //���������� "����" � ������ ����
			parent = parent->parent;								  //�������� ��������� �� ������, � ������� ����� ��������� ���, ����� - ����
		}
	}

	par.body = NULL;
	par.left = par.right = par.parent = NULL;

	grpar.body = NULL;
	grpar.left = grpar.right = grpar.parent = NULL;
}

template<class TKey, class TValue>
void RBTree<TKey, TValue>::remove_body(list * body)
{
	if (body->next != NULL)
		remove_body(body->next);
	delete body;
	return;
}

template<class TKey, class TValue>
const RBTree<TKey, TValue>* RBTree<TKey, TValue>::find(const TKey &key, size_t &key_hash) const
{
	if (is_list)
		return nullptr;

	if (key_hash == this->hash) {
		list *cur = body;
		while (cur->key != key || cur->next != NULL)
			cur = cur->next;

		if (cur->key == key)
			return this;
	}
	else {
		if (key_hash < this->hash)
			return left->find(key, key_hash);
		else
			return right->find(key, key_hash);
	}
	return nullptr;
}

template<class TKey, class TValue>
RBTree<TKey, TValue>::RBTree()
{
	left = right = parent = NULL;
	hash = 0;
	body = NULL;
	is_red = false;
	is_list = true;
}

template<class TKey, class TValue>
RBTree<TKey, TValue>::RBTree(RBTree<TKey, TValue>* parent)
{
	left = right = NULL;
	this->parent = parent;
	hash = 0;
	body = NULL;
	is_red = false;
	is_list = true;
}

template<class TKey, class TValue>
RBTree<TKey, TValue>::RBTree(TKey key, TValue value, RBTree<TKey, TValue> *head)
{
	left = right = parent = NULL;
	hash = std::hash<TKey>()(key);
	//body = NULL;
	is_red = true;
	is_list = false;

	this->insert(head, key, value);
}

template<class TKey, class TValue>
RBTree<TKey, TValue>::~RBTree()
{
	if (left != NULL)
		delete left;
	if (right != NULL)
		delete right;
	if (body != NULL) {
		remove_body(body);
	}
}


template<class TKey, class TValue>
void RBTree<TKey, TValue>::insert(RBTree<TKey, TValue> *head, TKey key, TValue value)
{
	//���� ���������� ����� ��� ����.
	RBTree<TKey, TValue> *cur = head;
	while (1) {
		if (cur->is_list || cur->hash == this->hash) {
			//�����, �������� ���������.
			break;
		}
		if (this->hash < cur->hash)
			cur = cur->left;
		else
			cur = cur->right;
	}

	//������ ����� �������� �������� � ������.
	if (cur->is_list) {

		//������ �����-�������
		left = new RBTree(cur);
		right = new RBTree(cur);

		//��������� ��������� �� ����
		parent = cur->parent;

		//��������� ����
		body = new RBTree<TKey, TValue>::list;
		body->key = key;
		body->value = value;
		body->next = NULL;
		*cur = *this;

		//������������ ������.
		cur->balance();

		//��������� ����� �������� �������� ��� ����� � ������, ����� �������� ���������, ����� ���������� �� ��� ������.
		left = right = parent = NULL;
		body = NULL;
	}
	else {
		RBTree<TKey, TValue>::list *cur_elem = cur->body;
		while (cur_elem->next != NULL && cur_elem->key == key) 			   
			cur_elem = cur_elem->next;

		if (cur_elem->key == key) {						   //���� ����������� ������� � ��� ������������ ������, �������������� �������� �� �����.
			cur_elem->value = value;
		}
		//���� ������� ����� �����, ��������� ����� �������.
		else {
			cur_elem->next = new RBTree<TKey, TValue>::list;
			cur_elem->next->key = key;
			cur_elem->next->value = value;
			cur_elem->next->next = NULL;
		}
		//������������ �� �����, �.�. � ����� ������� �� ��� ��������.
	}

}

template<class TKey, class TValue>
TValue & RBTree<TKey, TValue>::GetValue(const TKey & key) const
{

	list *cur = body;
	while (cur->key != key || cur->next != NULL)
		cur = cur->next;

	if (cur->key != key)
		throw ElemNotFound<TKey>(key);

	return cur->value;
	// TODO: �������� ����� �������� return
}
