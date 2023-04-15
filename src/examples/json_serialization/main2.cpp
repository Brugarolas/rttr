#include <rttr/registration>
#include<iostream>
#include "ExpTest.h"
//https://en.cppreference.com/w/cpp/language/type_alias

class ClzB
{
private:
	int m_number = 0;
};

template<typename Ty, typename Ty2 = int>
struct StuA
{
	using type = ClzB;
	using type2 = typename int;
};

//using 可以是特定类型的别名 也可以是类模板别名
template<typename Ty>
using StuBTy1 = typename StuA<Ty>::type;//模板中取内容前面貌似都要加typename

template<typename Ty>
using StuBTy2 = typename StuA<Ty>::type2;


int ExpTest::m_int = 10;

template<typename T>
constexpr bool is_lvalue(T&& expression) { return std::is_lvalue_reference<T>::value; }

template<typename T>
constexpr bool is_not_lvalue(T&& expression) { return  !is_lvalue(std::forward<T&&>(expression)); }


class Base {
public:
	Base() {
		std::cout << "Base()" << std::endl;
	}

	~Base() {
		std::cout << "~Base()" << std::endl;
	}
};

class Derived : public Base {
public:
	Derived() {
		std::cout << "Derived()" << std::endl;
	}

	~Derived() {
		std::cout << "~Derived()" << std::endl;
	}
};


int main()
{
	using namespace rttr;
	//const float c_ft_obj = 12.0f;
	const float& c_ft_obj = 12.0f;
	type ft_type_static = type::get<float>();     // statically or
	//type ft_type_dynamic = type::get(12.0f);      // dynamically
	type ft_type_dynamic = type::get(c_ft_obj);      

	void* p_ft_static = &ft_type_static;
	void* p_ft_dynamic = &ft_type_dynamic;
	bool p_ft_flag = ft_type_dynamic == ft_type_static; //对比指针 里面m_type_data才是同一份数据
	//C++ typeid是字符串对比
	//float/ const float/ const float& m_type_data都一样

	float ft_obj = 23.0f;
	float* ft_obj_ptr = &ft_obj;
	const float* c_ft_obj_ptr = ft_obj_ptr;
	bool ft_flag = type::get<float>() == type::get(ft_obj);       
	bool ft_ptr_flag = type::get<float*>() == type::get(ft_obj_ptr);
	bool c_ft_ptr_flag = type::get<const float*>() == type::get(c_ft_obj_ptr);
	type  ft_obj_type = type::get(ft_obj);
	type  ft_obj_ptr_type = type::get(ft_obj_ptr);
	type  c_ft_obj_ptr_type = type::get(c_ft_obj_ptr);
	bool ft_obj_flag = ft_obj_ptr_type == ft_obj_type; //不一样


	ClzB&& clz_ins = ClzB(); //ClzB&&是指类型 不是值类别（左值 右值）
	int l_val = 10;
	int& l_val_ref = l_val;
	bool int_ctgy = std::is_lvalue_reference<int>::value;//false
	bool l_val_ctgy = is_lvalue(int_ctgy);//true
	bool l_val_ref_ctgy = is_lvalue(l_val_ref);//true
	bool clz_ins_ctgy = is_lvalue(clz_ins);//true
	bool str_literal_ctgy = is_lvalue("like");//true
	bool int_literal_ctgy = is_lvalue(7);//false
	Base* base_point = new Derived();
	delete base_point;
	Base& base_ref = std::move(Derived());//会调子类析构
	Base& base_ref2 = Derived();//会调子类析构

	return 0;
}