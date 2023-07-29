#include <rttr/registration>
#include <rttr/variant.h>
//#include <type_traits>
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

	Base(int val) {
		m_base = val;
		std::cout << "Base(int val)" << std::endl;
	}

	int getVal()
	{
		return m_base;
	}

	Base(const Base& obj)
	{
		m_base = obj.m_base;
		std::cout << "Base(const Base& obj)" << std::endl;
	}

	~Base() {
		std::cout << "~Base()" << std::endl;
	}
private:
	int m_base = 10;
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



using bool_constant3 = std::integral_constant<bool, false>;


template <bool _Val>
using bool_constant2 = std::integral_constant<bool, _Val>;

template <class _Base, class _Derived>
struct if_is_base_of : bool_constant2<__is_base_of(_Base, _Derived)> {
	// determine whether _Base is a base of or the same as _Derived
};

struct alignas(32) defstruct //指定类型对象的对齐要求
{

};

int main()
{
	rttr::registration::class_<Base>("Base");
	//variant var = type::get<float>.create();

	size_t tysize = rttr::type_list<char, bool, uint64_t>::size;
	using var_basic_type = rttr::type_list<char, bool, uint64_t>;
	rttr::detail::max_sizeof_list<var_basic_type> typeIns;
	std::aligned_storage<rttr::detail::max_sizeof_list<var_basic_type>::value,
		rttr::detail::max_alignof_list<var_basic_type>::value>::type  typeIns2;//这个是存储val的具体类型

	//max_sizeof_list_impl 和 max_alignof_list_impl写法都挺炫的(编译期计算好了) 可以抄过来
	std::aligned_storage<rttr::detail::max_sizeof_list_impl<var_basic_type>::value,
		rttr::detail::max_alignof_list_impl<var_basic_type>::value>::type  typeIns3;//这个是存储val的具体类型

	std::aligned_storage<8, 8> typeIns4; 
	static constexpr size_t defstruct_align = std::alignment_of<defstruct>::value;//32
	//int valll = defstruct_align;
	//type为 std::aligned_storage<8, 8>把8 8传递给std::_Aligned<>推导出的std::_Align_type<double, 8>

	rttr::variant var = float(defstruct_align);
	rttr::variant var2 = Base(26);
	Base& bobj = var.get_value<Base>();
	Base& bobj2 = Base(28);

	using newAliType = std::_Align_type<double, 8>;
	newAliType newTypeIns;
	//memcpy(newTypeIns._Pad, reinterpret_cast<double*>(&bobj), 8);//这样子貌似不行  不知道怎么自己写代码在newTypeIns存储bobj地址


	new (&newTypeIns) Base(bobj2);//什么时机delete掉？？
	int bobj_val = reinterpret_cast<Base*>(newTypeIns._Pad)->getVal();

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
	bool ft_flag = type::get<float>() == type::get(ft_obj_ptr);
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

	bool val = std::is_base_of<Base, Derived>::value; //__is_base_of没有源码
	using newType = std::enable_if_t<true, int>;//true如果改成false 编译期会报错（模板没法根据给的参数进行特化）
	newType val2 = 10;
	//using newType2 = std::enable_if_t<!std::is_same<void, void>::value, void>;
	return 0;
}