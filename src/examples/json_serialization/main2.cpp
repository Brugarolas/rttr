
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

int static GetTestHandle()
{
	return 1;
}