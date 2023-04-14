#include <rttr/registration>
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

//using �������ض����͵ı��� Ҳ��������ģ�����
template<typename Ty>
using StuBTy1 = typename StuA<Ty>::type;//ģ����ȡ����ǰ��ò�ƶ�Ҫ��typename

template<typename Ty>
using StuBTy2 = typename StuA<Ty>::type2;


int ExpTest::m_int = 10;

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
	bool p_ft_flag = ft_type_dynamic == ft_type_static; //�Ա�ָ�� ����m_type_data����ͬһ������
	//C++ typeid���ַ����Ա�
	//float/ const float/ const float& m_type_data��һ��

	float ft_obj = 23.0f;
	float* ft_obj_ptr = &ft_obj;
	const float* c_ft_obj_ptr = ft_obj_ptr;
	bool ft_flag = type::get<float>() == type::get(ft_obj);       
	bool ft_ptr_flag = type::get<float*>() == type::get(ft_obj_ptr);
	bool c_ft_ptr_flag = type::get<const float*>() == type::get(c_ft_obj_ptr);
	type  ft_obj_type = type::get(ft_obj);
	type  ft_obj_ptr_type = type::get(ft_obj_ptr);
	type  c_ft_obj_ptr_type = type::get(c_ft_obj_ptr);
	bool ft_obj_flag = ft_obj_ptr_type == ft_obj_type; //��һ��

	return 0;
}