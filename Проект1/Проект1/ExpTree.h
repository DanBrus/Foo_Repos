#pragma once
#define _ITERATOR_DEBUG_LEVEL 0
#include <functional>
#include <string>
#include <map>

typedef std::map<std::string, float> var_data;

class ExpTree
{
private:
	//Интерфейс для классов-выражений
	class IExp{

	public:
		virtual float calculate(float a, float b, var_data calc_data, ExpTree *container) = 0;
	};

	//Класс-значение
	class Value : public IExp {
	private:
		float val;
	public:
		Value(std::string s_val) : val(std::stof(s_val)) {};
		float calculate(float a, float b, var_data calc_data, ExpTree *container) override;
	};

	//Класс-переменная
	class Variable : public IExp {
	private:
		std::string name;
	public:
		Variable(std::string str_name) : name(str_name) {};
		float calculate(float a, float b, var_data calc_data, ExpTree *container) override;
	};

	//Класс-знак
	class Sign : public IExp {
		typedef float(ExpTree::Sign:: *func)(float, float);
	public:
		Sign(char c);
		func sign_func;
		
	private:
		
		char sign;

		float sum(float a, float b);
		float dif(float a, float b);
		float mult(float a, float b);
		float div(float a, float b);
		float power(float a, float b);
		float root(float a, float b);
	public:
		float calculate(float a, float b, var_data calc_data, ExpTree *container) override;
	};

private:
	bool calc_err;
	IExp *body;																//Указатель на класс-выражение узла
	ExpTree *left_son, *right_son;											//Указатели на сыновей узла

	bool make_left_son(std::string &expr);
	bool make_right_son(std::string &expr);
	void del_sons();

	int priority(char c);													//Вернуть приоритет символа
	bool find_sign(std::string::iterator &a);								//Найти знак с минимальным приоритетом
	bool skip_brackets(std::string::iterator &a);							//Пропустить скобки (итератор останавливается на закрывающей скобке)
	bool find_brackets(std::string::iterator &a);							//Найти скобки (итератор останавливается на открывающей скобке)
	bool prefix(std::string &expr, std::string::iterator &i_expr);			//Разделить строку по знаку, если знак в префиксной форме
	bool suffix(std::string &expr, std::string::iterator &i_expr);			//Разделить строку по знаку, если знак в суффиксной форме
	bool unsigned_(std::string &expr, std::string::iterator &i_expr);		//Разделить строку по скобке при записи без знака
	bool sintax(std::string &expr);											//Определить корректность введённого числа или переменной
public:
	void set_err();
	bool get_err();
	void clean_err();
	ExpTree();
	ExpTree(std::string expr													//Строка для заполнения дерева
		);
	ExpTree(ExpTree &a) : calc_err(a.calc_err), body(a.body), left_son(a.left_son), right_son(a.right_son) {};
	~ExpTree();
	bool fill																//Функция заполнения дерева
	(std::string expr														//Строка для заполнения дерева
	);
	void free();															//Очистить дерево
	float calculate(var_data calc_data);
};

