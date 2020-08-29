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
	bool is_red;								 //Тип узла
	RBTree<TKey, TValue> *left;				 //Левый сын
	RBTree<TKey, TValue> *right;			 //Правый сын
	RBTree<TKey, TValue> *parent;			 //Папа
	std::size_t hash;						 //Значение хэша
	bool is_list;
	list *body;								 //Тело (существует только для листа)

	void balance();
	void red_parent_and_uncle();
	void low_black_uncle();
	void large_black_uncle();
	void remove_body(list *body);

public:
	RBTree();
	RBTree(RBTree<TKey, TValue> *parent);																			  //Конструктор для листа
	RBTree(TKey key, TValue value, RBTree<TKey, TValue> *head);												  //Конструктор, встраивающий элемент в существующее КЧД.
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
Принципы реализации:
	- Преобразовать ключ в хэш, т.к. возможно, для TKey нет функции less().
	- Использовать кчд для сохранения в памяти пары "хэш - указатель"
	- По указателю лежит односвязный список
	- Каждый элемент списка - пара "TKey-TValue"

Доступ:
	- Преобразовать TKey в хэш
	- Найти соответствующий элемент в кчд
	- Найти нужную пру в списке
	- Если в списке нет нужной пары, выбросить исключение.

Добавление:
	- Преобразовать TKey в хэш
	- Существует ли элемент в КЧД
	- Существует:
		- Добавить новый элемент в соответствующий список
	- Не существует:
		- Добавить новый элемент в КЧД
		- Создать новый список и привязать его к КЧД
*/

template<class TKey>
inline const TKey & ElemNotFound<TKey>::GetKey() const noexcept
{
	return except_key;
}

template<class TKey, class TValue>
void TestDict<TKey, TValue>::Set(const TKey & key, const TValue & value)
{
	//Добавление значения в кчд и в список.
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
	//Случай "Красные дядя и папа"
	red_parent_and_uncle();
	//Случай "Чёрный дядя, красный папа"
	low_black_uncle();
	large_black_uncle();
}

template<class TKey, class TValue>
void RBTree<TKey, TValue>::red_parent_and_uncle()
{
	//Проверка, не является ли элемент корнем, и не является ли он красным.
	if (parent == NULL) {
		if (is_red)
			is_red = false;
		return;
	}
	//Проверка, существует ли у элемента дедушка. Если не существует, то элемент может быть любого цвета - правил КЧД он не нарушает.
	if (parent->parent == NULL)
		return;
	//Проверка, являются ли папа и дядя красными. Если нет, return
	if (!(parent->parent->left->is_red && parent->parent->right->is_red))
		return;
	//Смещение "красного" уровня 
	parent->parent->left->is_red = false;
	parent->parent->right->is_red = false;
	parent->parent->is_red = true;
	//рекурсивная проверка того же свойства для дедушки
	parent->parent->balance();
}

template<class TKey, class TValue>
void RBTree<TKey, TValue>::low_black_uncle()
{
	//Проверка, не является ли элемент корнем.
	if (parent == NULL)
		return;

	//Проверка, существует ли у элемента дедушка.
	if (parent->parent == NULL)
		return;

	//Проверка на соответствие условию "Красный папа, чёрный дядя"
	if (!(parent->parent->left->is_red != parent->parent->right->is_red && parent->is_red))
		return;

	if (parent == parent->parent->left && this == parent->right) {
		parent->right = this->left;
		this->left->parent = parent;

		this->left = parent;
		parent->parent->left = this;
		this->parent = parent->parent;

		this->left->parent = this;				//В left уже лежит бывший папа.

		this->left->large_black_uncle();
	}
	else {															   //else здесь нужен потому, что в результате выполнеия предыдущего if активный узел может стать сыном узла. огда parent->parent == nullptr
		if (parent == parent->parent->right && this == parent->left) {
			parent->left = this->right;
			this->right->parent = parent;
			this->right = parent;
			parent->parent->right = this;

			this->parent = parent->parent;
			this->right->parent = this;				//В right уже лежит бывший папа.

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
		par.parent = parent->parent->parent;					  //"отец" должен стать сыном прадеда
		par.right = parent;									  //Правый сын "отца" - "дед", будет помещён в ячейку отца
		grpar.parent = parent->parent;							  //отец "деда" - "отец" будет помещён в ячейку деда
																  //
		grpar.left = parent->right;								  //левым сыном "деда" станет правый сын отца
																  //Указатель parent в правом сыне отца менять не нужно, т.к. "дед" будет помещён в ячейку отца.
		parent->parent->right->parent = parent;					  //Дядя станет сыном "деда", но дед будет находиться в ячейке отца, поэтому меняем укаатель.
																  //Дядя уже является правм сыном "деда", так что указатель менять не надо.
		*(parent->parent) = par;								  //Записываем "отца" в ячейку деда
		*parent = grpar;											  //записываем "деда" в ячейку отца
		parent = parent->parent;								  //Изменяем указатель на ячейку, в которой ранее находился дед, нынче - отец
	}
	else {															   //else здесь нужен потому, что в результате выполнеия предыдущего if активный узел может стать сыном узла. огда parent->parent == nullptr
		if (parent == parent->parent->right && this == parent->right) {
			par.parent = parent->parent->parent;					  //"отец" должен стать сыном прадеда
			par.left = parent;										  //Левый сын "отца" - "дед", будет помещён в ячейку отца
			grpar.parent = parent->parent;							  //отец "деда" - "отец" будет помещён в ячейку деда
																	  //
			grpar.right = parent->left;								  //Правым сыном "деда" станет левый сын отца
																	  //Указатель parent в правом сыне отца менять не нужно, т.к. "дед" будет помещён в ячейку отца.
			parent->parent->left->parent = parent;					  //Дядя станет сыном "деда", но дед будет находиться в ячейке отца, поэтому меняем укаатель.
																	  //Дядя уже является правм сыном "деда", так что указатель менять не надо.
			*(parent->parent) = par;								  //Записываем "отца" в ячейку деда
			*parent = grpar;											  //записываем "деда" в ячейку отца
			parent = parent->parent;								  //Изменяем указатель на ячейку, в которой ранее находился дед, нынче - отец
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
	//Ищем подходящее место для узла.
	RBTree<TKey, TValue> *cur = head;
	while (1) {
		if (cur->is_list || cur->hash == this->hash) {
			//Налши, начинаем заполнять.
			break;
		}
		if (this->hash < cur->hash)
			cur = cur->left;
		else
			cur = cur->right;
	}

	//Запись копии текущего элемента в дерево.
	if (cur->is_list) {

		//Создаём листы-сыновья
		left = new RBTree(cur);
		right = new RBTree(cur);

		//Сохраняем указатель на отца
		parent = cur->parent;

		//Заполняем тело
		body = new RBTree<TKey, TValue>::list;
		body->key = key;
		body->value = value;
		body->next = NULL;
		*cur = *this;

		//Балансировка дерева.
		cur->balance();

		//Поскольку копия текущего элемента уже лежит в дереве, нужно очистить указаткли, чтобы деструктор не снёс дерево.
		left = right = parent = NULL;
		body = NULL;
	}
	else {
		RBTree<TKey, TValue>::list *cur_elem = cur->body;
		while (cur_elem->next != NULL && cur_elem->key == key) 			   
			cur_elem = cur_elem->next;

		if (cur_elem->key == key) {						   //Если добавляется элемент с уже существующим ключом, перезаписываем значение по ключу.
			cur_elem->value = value;
		}
		//Если дошлидо конца сиска, добавляем новый элемент.
		else {
			cur_elem->next = new RBTree<TKey, TValue>::list;
			cur_elem->next->key = key;
			cur_elem->next->value = value;
			cur_elem->next->next = NULL;
		}
		//Балансировка не нужна, т.к. в итоге элемент не был добавлен.
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
	// TODO: вставьте здесь оператор return
}
