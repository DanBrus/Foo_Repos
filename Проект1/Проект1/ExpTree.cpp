#include "ExpTree.h"


ExpTree::Sign::Sign(char c)
{
	switch (c) {					 //���������� ����������� �������
	case '+':
		sign_func = &ExpTree::Sign::sum;
		break;
	case '-':
		sign_func = &ExpTree::Sign::dif;
		break;
	case '*':
		sign_func = &ExpTree::Sign::mult;
		break;
	case '/':
		sign_func = &ExpTree::Sign::div;
		break;
	case '|':
		sign_func = &ExpTree::Sign::root;
		break;
	case '^':
		sign_func = &ExpTree::Sign::power;
		break;
	}
	sign = c;
}

float ExpTree::Sign::sum(float a, float b)
{
	return a + b;
}

float ExpTree::Sign::dif(float a, float b)
{
	return a - b;
}

float ExpTree::Sign::mult(float a, float b)
{
	return a*b;
}

float ExpTree::Sign::div(float a, float b)
{
	return a / b;
}

float ExpTree::Sign::power(float a, float b)
{
	return pow(a, b);
}

float ExpTree::Sign::root(float a, float b)
{
	return pow(a, 1/b);
}

float ExpTree::Sign::calculate(float a, float b, var_data calc_data, ExpTree *container)			 //��������� ����������� �������
{
	return (this->*sign_func)(a, b);
}


bool ExpTree::fill(std::string expr)
{
	if (body != NULL) free();							//���� ������ ��� ���������, ��������
	std::string::iterator i_expr = expr.begin();
	if (find_sign(i_expr)) return true;					//���� ���� � ����������� �����������.. ������� ������ ������, �.�. ������ ����� ���������� � �������������� �����.
	if (*i_expr != '\0') {
		if (i_expr == expr.begin()) {								 //���� ����� � ���� �������� ������ � ������, ���������, ���������� �� ����������� ������������.
			std::string::iterator tmp = expr.begin();
			bool mult = false;
			while (*tmp != '\0') {									 //��������, ���������� �� � ��������� ���� ������ 1 ������ �����, ��� ����� ��� �� ��� ��������������� �������.
				find_brackets(tmp);
				if (*tmp == '(' && priority(*(tmp - 1)) == 3) {
					mult = true;
					break;
				}
				skip_brackets(tmp);
			}

			if (mult) {												 //���� ����������, ����������� �����.
				if (unsigned_(expr, tmp)) {
					del_sons();
					return true;
				}
			}
			else
				if (prefix(expr, i_expr)) {							//���� �� ����������, ���������� �����
					del_sons();
					return true;
				}
		}
		else
			if (suffix(expr, i_expr)) {					//���� �����, � ���� �� �������� ������ � ������, ���������� �����
				del_sons();
				return true;
			}
	}
	else {												//�� ����� ����� ��� ������
		find_brackets(i_expr = expr.begin());			//���� ������ 
		if (*i_expr == '(') {				 			//���� �����, ����������� �����
			if (unsigned_(expr, i_expr)) {
				del_sons();
				return true;
			}
		}
		else
			if (sintax(expr)) {							//���� �� �����, ��������� ��������� ������: ��������, ���������� ��� ������������ ���� 
				del_sons();
				return true;
			}
	}
	return false;
}

void ExpTree::free()
{
	del_sons();
	if (body != NULL)
		delete body;
	body = NULL;
}

float ExpTree::calculate(var_data calc_data)
{
	float tmp;
	if(left_son == NULL)
		tmp = body->calculate(0, 0, calc_data, this);
	else {
		tmp = body->calculate(left_son->calculate(calc_data), right_son->calculate(calc_data), calc_data, this);
		if (left_son->get_err() || right_son->get_err()) {
			set_err();
			left_son->clean_err();
			right_son->clean_err();
			return 0;
		}
	}
	return tmp;
}

bool ExpTree::make_left_son(std::string & expr)
{
	left_son = new ExpTree();
	if(left_son->fill(expr)) return true;
	return false;
}

bool ExpTree::make_right_son(std::string & expr)
{
	right_son = new ExpTree();
	if (right_son->fill(expr)) return true;
	return false;
}

void ExpTree::del_sons()
{
	if (left_son != NULL) left_son->del_sons();
	if (right_son != NULL) right_son->del_sons();

	delete left_son;
	delete right_son;
	left_son = right_son = NULL;
}

int ExpTree::priority(char c)
{
	switch (c) {
	case '+':
		return 0;
	case '-':
		return 0;
	case '*':
		return 1;
	case '/':
		return 1;
	case '|':
		return 2;
	case '^':
		return 2;

	default:
		return 3;
	}
}

bool ExpTree::find_sign(std::string::iterator & a)
{
	std::string::iterator cur = a;

	for (; *cur != '\0'; cur++) {											 //������������� ������ �� �����
		if(priority(*(cur)) <= priority(*a)) a = cur;				 //���� ��������� �������� ������� ������ ���������� ����������� �������, ��������� ������� ������
		if (*cur == '(') {											 //���� ������� ���� - �������� ������, 
			if (skip_brackets(cur)) return true;
		}
	}
	if (priority(*a) == 3) a = cur;									 //���� ���������� ������ �� �������� �����. ������, ��������� \0
	return false;
}

bool ExpTree::skip_brackets(std::string::iterator & a)
{
	//if (*a != '(') return false;
	find_brackets(a);
	int i = 0;
	do {
		switch (*a) {
		case ')':
			a++;
			i--;
			break;

		case '(':
			i++;
			a++;
			break;

		case '\0':
			a++;
			i = 0;
			break;

		default:
			a++;
			break;

		}
	} while (i != 0);
	a--;
	return false;
}

bool ExpTree::find_brackets(std::string::iterator & a)
{
	while ((*a != '(') && (*a != '\0')) a++;
	return false;
}

bool ExpTree::prefix(std::string & expr, std::string::iterator & i_expr)
{
	std::string to_son;

	if (*(i_expr + 1) == '(')
		to_son = std::string(i_expr + 2, expr.end() - 1);
	else
		to_son = std::string(i_expr + 1, expr.end());
	switch (*i_expr) {
	case '-':
		if (make_right_son(to_son)) return true;
		if (make_left_son(std::string("0"))) return true;
		body = new ExpTree::Sign('-');
		break;

	case '|':
		if (make_left_son(to_son)) return true;
		if (make_right_son(std::string("2"))) return true;
		body = new ExpTree::Sign('|');
	}

	return false;
}

bool ExpTree::suffix(std::string & expr, std::string::iterator & i_expr)
{
	std::string left, right;

	std::string::iterator tmp = expr.begin();
	if (*tmp == '(') {
		if (skip_brackets(tmp)) return true;									   //�������� ���������� ������. ���� ������ ���������� �� ������ � ��� ������ ����������� ����� ������� ������, tmp == i_expr - 1
		if (tmp == i_expr - 1)
			left = std::string(expr.begin() + 1, i_expr - 1);				   //���� ����� �� ����� ������ ���� ������, ����� ����� �������� ���� ���������� ������.
		else
			left = std::string(expr.begin(), i_expr);						  //���� ���, �������� ��� ����� ����� ������
	}
	else {
		left = std::string(expr.begin(), i_expr);						   //���� ���, �� ��� ����� ����� ������.
	}

	tmp = i_expr + 1;													   //������� �������� �� ������ ����� ����� �����. ����� ������ ��� � ����� ������.
	if(*tmp == '('){
		if (skip_brackets(tmp)) return true;
		if (tmp == expr.end() - 1 && *tmp == ')') 
			right = std::string(i_expr + 2, expr.end()-1);
		else
			right = std::string(i_expr + 1, expr.end());
	}
	else {
		right = std::string(i_expr + 1, expr.end());
	}

	if (make_left_son(left)) return true;								   //������ ������ ����
	if (make_right_son(right)) return true;								   //������ ������� ����

	body = new ExpTree::Sign(*i_expr);
	

	return false;
}

//!!! ��������� ������ (a)(b)(c)!!!
bool ExpTree::unsigned_(std::string & expr, std::string::iterator & i_expr)
{
	std::string::iterator tmp = expr.begin();
	std::string to_left;
	if (*tmp == '(') {																//����������, �������� �� ������ ��������� ������ ������.
		skip_brackets(tmp);
		if (tmp == i_expr - 1)														//���� ������ ������ ������ - �������� ������ � ���� ������ ������������� ����� i_expr, ����� - ���� ������.
			to_left = std::string(expr.begin() + 1, tmp);							//� ������ ���� ����� �������� ������ ���������� ������
		else
			to_left = std::string(expr.begin(), i_expr);							//� ������ ���� ����� �������� ��� ����� ����� ������
	}
	else
		to_left = std::string(expr.begin(), i_expr);

	if(make_left_son(to_left)) return true;											//�������� ������ ����															    
	if(make_right_son(std::string(i_expr + 1, expr.end() - 1))) return true;		//�������� ������� ���� (�������� ����������� ������).
	body = new ExpTree::Sign('*');
	return false;
}

bool ExpTree::sintax(std::string & expr)							//��������, ��������� �� ������� ����� ��� ����������
{
	bool number = false;
	std::string::iterator i = expr.begin();							

	if (expr.end() == i) return true;								//���� ������� ������ ������, ���������� ������

	if ((*i >= '0') && (*i <= '9'))									//���������� �� ����� ���������� � �����, ������� ������ ������ �������������, ����� ������� ��� ����������
		number = true;

	for (i; i != expr.end(); i++) {									//�������� �� ����� ������
		if (number) {
			if (((*i < '0') || (*i > '9')) && (*i != '.'))			//���� ������������, ��� ������� �����, �� �� ����� ������ �� ������ ����������� ���� ��������, ����� ���� � �����������.
				return true;
		}
		else	
			if(!((*i >= 'A' && *i <= 'z') && (*i <= 'Z' || *i >= 'a')) && (*i != '_') && (((*i < '0') || (*i > '9'))))		  //���� ������������, ��� ������� ����������, �� �� ������ ����������� ������������ ��� �������������� ��������.
				return true;
		
	}

	if (number)
		body = new Value(expr);
	else
		body = new Variable(expr);
	return false;
}

void ExpTree::set_err()
{
	calc_err = true;
}

bool ExpTree::get_err()
{
	return calc_err;
}

void ExpTree::clean_err()
{
	calc_err = false;
}

ExpTree::ExpTree()
{
	calc_err = false;
	left_son = right_son = NULL;
	body = NULL;
}

ExpTree::ExpTree(std::string expr)
{
	calc_err = false;
	left_son = right_son = NULL;
	body = NULL;
	fill(expr);
}

ExpTree::~ExpTree()
{
	if ((left_son != NULL) || (right_son != NULL))
		del_sons();
	delete body;
}

float ExpTree::Variable::calculate(float a, float b, var_data calc_data, ExpTree *container)
{
	if (calc_data.find(name) != calc_data.end())
		return calc_data[name];

	container->set_err();
	return 0;
}

float ExpTree::Value::calculate(float a, float b, var_data calc_data, ExpTree *container)
{
	return val;
}
