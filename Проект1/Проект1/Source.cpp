#include "ExpTree.h"
#include <iostream>

std::map<std::string, ExpTree> exp_map;
std::map<std::string, var_data> var_data_map;

bool is_not_name(std::string a) {															//Проверка на соответствие синтакису имён
	std::string::iterator cur;
	for (cur = a.begin(); *cur != '\0'; cur++) {												//Проверяем, корректно ли введено имя выражения.
		if (cur == a.begin() && ((*cur > '0') && (*cur < '9'))) {								//Первый символ не должен быть цифрой.
			return true;
		}
		if ((!((*cur >= 'A' && *cur <= 'z') && (*cur <= 'Z' || *cur >= 'a')) && (*cur != '_') && (((*cur < '0') || (*cur > '9'))))) {	 //Любой символ должен быть цифрой, буквой либо символом '_'.
			return true;
		}
	}
	return false;
}

void ExpInp() {
	std::cout << "Enter a expression. Format: \"ExpName=expression\"" << std::endl;
	std::string tmp;
	std::cin >> tmp;
	size_t index = tmp.find('=');
	if (index != -1) {
		std::string::iterator cur = tmp.begin() + index;
		const std::string name_expr(tmp.begin(), cur);			
		if (is_not_name(name_expr)) {																			//Проверяем, может ли введённая сирока использоваться как имя.
			std::cout << "The name of expression is incorrect" << std::endl;
			std::cin.clear();
			return;
		}
																												//Проверяем, существует ли уже запись с идентичным именем.
		if (exp_map.find(name_expr) != exp_map.end()) {
			std::cout << "An expression withthe name is already exist." << std::endl;											
			std::cout << "Remove old exception before insernnew with the name." << std::endl;
			std::cin.clear();
			return;
		}
		ExpTree tree;
		exp_map.insert(std::pair<const std::string, ExpTree>(name_expr, tree));									//Если проверка прошла успешно, создаём экземпляр ExpTree и помещаем его в map.
		if (exp_map[name_expr].fill(std::string(tmp.begin() + index + 1, tmp.end()))) {							//Заполняем ExpTree частью строки, находящейся справа от '=' (выражение)
			exp_map.erase(name_expr);
			std::cout << "The expression is incorrect" << std::endl;
			std::cin.clear();											//Если выражение введено неверно, удаляем заись из map.
			return;
		}


		
		std::cout << "Expression saved" << std::endl;
	}
	else
		std::cout << "Format error" << std::endl;
	std::cin.clear();
	return;

}

void ShowExp() {
	std::cout << "Function in not available." << std::endl;
	//std::string tmp;
	/*
	do {
		std::string::iterator cur;
		for (cur = tmp.begin(); *cur != '='; cur++) {												//Проверяем, корректно ли введено имя выражения.
			if (cur == tmp.begin() && ((*cur > '0') && (*cur < '9'))) {								//Первый символ не должен быть цифрой.
				std::cout << "The name of expression is incorrect" << std::endl;
				std::cin.clear();
				return;
			}
			if ((!((*cur >= 'A' && *cur <= 'z') && (*cur <= 'Z' || *cur >= 'a')) && (*cur != '_') && (((*cur < '0') || (*cur > '9'))))) {	 //Любой символ должен быть цифрой, буквой либо символом '_'.
				std::cout << "The name of expression is incorrect" << std::endl;
				std::cin.clear();
				return;
			}	
		}
		const std::string name_var(tmp.begin(), cur);															//Проверяем, существует ли уже запись с идентичным именем.
		///Править. Дохуя править.
		if (exp_map.find(name_expr) != exp_map.end()) {
			std::cout << "An expression withthe name is already exist." << std::endl;											
			std::cout << "Remove old exception before insernnew with the name." << std::endl;
			std::cin.clear();
			return;
		}
		ExpTree tree;
		exp_map.insert(std::pair<const std::string, ExpTree>(name_expr, tree));									//Если проверка прошла успешно, создаём экземпляр ExpTree и помещаем его в map.
		if (exp_map[name_expr].fill(std::string(tmp.begin() + index + 1, tmp.end()))) {							//Заполняем ExpTree частью строки, находящейся справа от '=' (выражение)
			exp_map.erase(name_expr);
			std::cout << "The expression is incorrect" << std::endl;
			std::cin.clear();											//Если выражение введено неверно, удаляем заись из map.
			return;
		}
		else {
			std::cout << "Exception with the name not found." << std::endl;
		}
	} while (*(tmp.end() - 1) != ',');
	*/
	std::cin.clear();
	return;
}

void DelExp() {
	std::cout << "Enter name of an expression." << std::endl;

	std::string tmp;
	do {
		std::cin >> tmp;
		std::string::iterator iter = tmp.begin();
		while (*iter != ',' && *iter != '\0')
			iter++;
		std::string key(tmp.begin(), iter);
		if (exp_map.find(key) != exp_map.end()) {
			exp_map.erase(key);
			std::cout << "Expression deleted." << std::endl;
		}
		else {
			std::cout << "Exception with the name not found." << std::endl;
		}
	} while (*(tmp.end() - 1) != ',');

	std::cin.clear();
	return;
}

void DataInp() {
	std::cout << "Input name of claster of data and data. Format: \"Claster_Name : Var_Name1=value, Var_Name2=value ... Var_Name=value" << std::endl;
	std::string tmp;
	std::cin >> tmp;

	if (is_not_name(tmp)) {																	  //Проверка корректности имени кластера данных
		std::cout << "Claster name is incorrect." << std::endl;
		return;
	}

	if (var_data_map.find(tmp) == var_data_map.end()) {										  //Если введённого имени не существует
		std::pair<std::string, var_data> tmp_pair(tmp, var_data());
		std::cin >> tmp;
		if (tmp[0] == ':' && tmp.length() == 1) {											  //Проверка корректности ввода
			do {
				std::cin >> tmp;															  //Считывание равенства.
				std::string::iterator iter = tmp.begin();
				while (*iter != '=') {														  //Ищем знак равенства. Если дошли до конца строки, произошло нарушение формата.
					if (*iter == '\0') {
						std::cout << "Format error" << std::endl;
						std::cin.clear();
						return;
					}
					iter++;
				}
				std::string var_key(tmp.begin(), iter);											  //Копируем имя переменной в отдельную строку.

				if (is_not_name(var_key)) {														  //Проверка имени переменной на корректность
					std::cout << "Variable name is incorrect." << std::endl;
					std::cin.clear();
					return;
				}

				if (tmp_pair.second.find(var_key) != tmp_pair.second.end()) {					  //Проверка имени переменной на повторение
					std::cout << "Variable name is repited." << std::endl;
					std::cin.clear();
					return;
				}
				std::string var_vals;

				if(*(tmp.end() - 1) == ',')														  //Отрезаем запятую в конце при необходимости
					var_vals = std::string(iter + 1, tmp.end() - 1);
				else
					var_vals = std::string(iter + 1, tmp.end());

				float var_valf;
				try {
					var_valf = std::stof(var_vals);
				}																				 
				catch (std::invalid_argument) {													  //Пытаемся преобразовать значение в float
					std::cout << "Variable value is unallowable." << std::endl;
					std::cin.clear();
					return;
				}

				tmp_pair.second.insert(std::pair<std::string, float>(var_key, var_valf));
			} while (*(tmp.end() - 1) != ',');		

			if (tmp_pair.second.empty()) {															// Проверяем, добавлена ли хотя бы одна пара
				std::cout << "You shood to enter at least one variable." << std::endl;
				return;
			}
			var_data_map.insert(tmp_pair);
			std::cout << "Data saved." << std::endl;													   //Добавлем кластер в массив.
		}
		else {
			std::cout << "Format error" << std::endl;
			std::cin.clear();
			return;
		}
	}
	else {
		std::cout << "Claster with the name already exist." << std::endl;
		std::cin.clear();
		return;
	}

}

void DelData() {
	std::cout << "Enter name of an variable claster." << std::endl;

	std::string tmp;
	do {
		std::cin >> tmp;
		std::string::iterator iter = tmp.begin();
		while (*iter != ',' && *iter != '\0')
			iter++;
		std::string key(tmp.begin(), iter);
		if (var_data_map.find(key) != var_data_map.end()) {
			var_data_map.erase(key);
			std::cout << "Variable claster deleted." << std::endl;
		}
		else {
			std::cout << "Claster with the name not found." << std::endl;
		}
	} while (*(tmp.end() - 1) != ',');

	std::cin.clear();
	return;

}

void Calc() {
	std::cout << "Input name of expression and name of data claster. Format: Exptession_name : Data_name" << std::endl;
	std::string exp_key;
	std::cin >> exp_key;
	if (exp_map.find(exp_key) == exp_map.end()) {
		std::cout << "Expresion with the name not found." << std::endl;
		std::cin.clear();
		return;
	}

	std::string claster_key;
	std::cin >> claster_key;
	if (claster_key != ":"){
		std::cout << "Format error." << std::endl;
		std::cin.clear();
		return;
	}

	std::cin >> claster_key;
	if (var_data_map.find(claster_key) == var_data_map.end()) {
		std::cout << "Expresion with the name not found." << std::endl;
		std::cin.clear();
		return;
	}

	std::cout << exp_map[exp_key].calculate(var_data_map[claster_key]) << std::endl;
}


typedef void(*action)();

static std::pair<const std::string, action> init_pairs[7] = {
	std::pair<const std::string, action>(std::string("ExpInp"), &ExpInp),
	std::pair<const std::string, action>(std::string("ShowExp"), &ShowExp),
	std::pair<const std::string, action>(std::string("DelExp"), &DelExp),
	std::pair<const std::string, action>(std::string("DataInp"), &DataInp),
	std::pair<const std::string, action>(std::string("DelData"), &DelData),
	std::pair<const std::string, action>(std::string("Calc"), &Calc)
};
static std::initializer_list<std::pair<const std::string, action>> init_list(init_pairs, &init_pairs[6]);
static std::map<std::string, action> act_map(init_list);

int main(void) {
	/*ExpTree Tree;
	std::map<std::string, float> calc_data;
	calc_data.insert(std::pair<std::string, float>(std::string("a"), 12));
	calc_data.insert(std::pair<std::string, float>(std::string("b"), 24));

	while (1) {
		std::string a;
		std::getline(std::cin, a);
		if (Tree.fill(a))
			std::cout << "Incorrect input" << std::endl;
		else {
			std::cout << "Correct input" << std::endl;
			float tmp = Tree.calculate(calc_data);
			if (Tree.get_err())
				std::cout << "Calculating error" << std::endl;
			else
				std::cout << tmp << std::endl;
			Tree.clean_err();
		}
		Tree.free();
	}*/

	std::string input;
	while (1) {
		std::cin >> input;
		std::cin.clear();
		if(act_map.find(input) != act_map.end())
			act_map[input]();
		else
			std::cout << "Error. Unknown command." << std::endl;

	}

	return 0;
}


