#pragma once
#define _ITERATOR_DEBUG_LEVEL 0
#include <functional>
#include <string>
#include <map>

typedef std::map<std::string, float> var_data;

class ExpTree
{
private:
	//��������� ��� �������-���������
	class IExp{

	public:
		virtual float calculate(float a, float b, var_data calc_data, ExpTree *container) = 0;
	};

	//�����-��������
	class Value : public IExp {
	private:
		float val;
	public:
		Value(std::string s_val) : val(std::stof(s_val)) {};
		float calculate(float a, float b, var_data calc_data, ExpTree *container) override;
	};

	//�����-����������
	class Variable : public IExp {
	private:
		std::string name;
	public:
		Variable(std::string str_name) : name(str_name) {};
		float calculate(float a, float b, var_data calc_data, ExpTree *container) override;
	};

	//�����-����
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
	IExp *body;																//��������� �� �����-��������� ����
	ExpTree *left_son, *right_son;											//��������� �� ������� ����

	bool make_left_son(std::string &expr);
	bool make_right_son(std::string &expr);
	void del_sons();

	int priority(char c);													//������� ��������� �������
	bool find_sign(std::string::iterator &a);								//����� ���� � ����������� �����������
	bool skip_brackets(std::string::iterator &a);							//���������� ������ (�������� ��������������� �� ����������� ������)
	bool find_brackets(std::string::iterator &a);							//����� ������ (�������� ��������������� �� ����������� ������)
	bool prefix(std::string &expr, std::string::iterator &i_expr);			//��������� ������ �� �����, ���� ���� � ���������� �����
	bool suffix(std::string &expr, std::string::iterator &i_expr);			//��������� ������ �� �����, ���� ���� � ���������� �����
	bool unsigned_(std::string &expr, std::string::iterator &i_expr);		//��������� ������ �� ������ ��� ������ ��� �����
	bool sintax(std::string &expr);											//���������� ������������ ��������� ����� ��� ����������
public:
	void set_err();
	bool get_err();
	void clean_err();
	ExpTree();
	ExpTree(std::string expr													//������ ��� ���������� ������
		);
	ExpTree(ExpTree &a) : calc_err(a.calc_err), body(a.body), left_son(a.left_son), right_son(a.right_son) {};
	~ExpTree();
	bool fill																//������� ���������� ������
	(std::string expr														//������ ��� ���������� ������
	);
	void free();															//�������� ������
	float calculate(var_data calc_data);
};

