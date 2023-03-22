
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

int static GetTestHandle()
{
	return 1;
}