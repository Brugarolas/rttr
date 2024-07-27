/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <iostream>

#include <rttr/registration>

#include "to_json.h"
#include "from_json.h"

using namespace rttr;

enum class color
{
    red,
    green,
    blue
};

struct point2d
{
    point2d() {}
    point2d(int x_, int y_) : x(x_), y(y_) {}
    int x = 0;
    int y = 0;
};

struct shape
{
    shape(std::string n) : name(n) {}

    void set_visible(bool v) { visible = v; }
    bool get_visible() const { return visible; }

    color color_ = color::blue;
    std::string name = "";
    point2d position;
    std::map<color, point2d> dictionary;

public:
    virtual inline::rttr::type get_type() const { return ::rttr::detail::get_type_from_instance(this); }
    virtual inline void* get_ptr() { return reinterpret_cast<void*>(this); }
    virtual inline::rttr::detail::derived_info get_derived_info() { return { reinterpret_cast<void*>(this), ::rttr::detail::get_type_from_instance(this) }; }
    using base_class_list = ::rttr::type_list<>;
private:
    bool visible = false;
};

struct circle : shape
{
    circle(std::string n) : shape(n) {}

    double radius = 5.2;
    std::vector<point2d> points;

    int no_serialize = 100;

    //RTTR_ENABLE(shape)
public: 
    virtual inline::rttr::type get_type() const { return ::rttr::detail::get_type_from_instance(this); }  
    virtual inline void* get_ptr() { return reinterpret_cast<void*>(this); }
    virtual inline::rttr::detail::derived_info get_derived_info() { return { reinterpret_cast<void*>(this), ::rttr::detail::get_type_from_instance(this) }; }
    using base_class_list = ::rttr::type_list<shape>;

};

//RTTR_REGISTRATION

static void rttr_auto_register_reflection_function_();                              
namespace                                                                           
{                                                                                   
    struct rttr__auto__register__                                                   
    {                                                                               
        rttr__auto__register__()                                                    
        {                                                                           
            rttr_auto_register_reflection_function_();                              
        }                                                                           
    };                                                                              
}     

//__LINE__ 表示当前代码所在行号
//a##b表示ab
static const rttr__auto__register__ auto_register__102;

static void rttr_auto_register_reflection_function_()
{
    rttr::registration::class_<shape>("shape")
        .property("visible", &shape::get_visible, &shape::set_visible)
        .property("color", &shape::color_)
        .property("name", &shape::name)
        .property("position", &shape::position)
        .property("dictionary", &shape::dictionary)
    ;

    rttr::registration::class_<circle>("circle")
        .property("radius", &circle::radius)
        .property("points", &circle::points)
        .property("no_serialize", &circle::no_serialize)
        (
            metadata("NO_SERIALIZE", true)
        )
        ;

    rttr::registration::class_<point2d>("point2d")
        .constructor()(rttr::policy::ctor::as_object)
        .property("x", &point2d::x)
        .property("y", &point2d::y)
        ;


    rttr::registration::enumeration<color>("color")
        (
            value("red", color::red),
            value("blue", color::blue),
            value("green", color::green)
        );
}

int _2332main(int argc, char** argv)
{
    std::string json_string;

    {
        circle c_1("Circle #1");
        shape& my_shape = c_1;

        c_1.set_visible(true);
        c_1.points = std::vector<point2d>(2, point2d(1, 1));
        c_1.points[1].x = 23;
        c_1.points[1].y = 42;

        c_1.position.x = 12;
        c_1.position.y = 66;

        c_1.radius = 5.123;
        c_1.color_ = color::red;

        // additional braces are needed for a VS 2013 bug
        c_1.dictionary = { { {color::green, {1, 2} }, {color::blue, {3, 4} }, {color::red, {5, 6} } } };

        c_1.no_serialize = 12345;

        json_string = io::to_json(my_shape); // serialize the circle to 'json_string'
    }

    variant var = "string";
    std::string str_var = var.get_value<std::string>();
    //std::string const * str_val_ref = &str_var;
    //const std::string * str_val_ref = &str_var;
    //str_val_ref = nullptr;

    circle c_2("Circle #2");
    variant var2 = c_2;
    circle c_2_ref = var2.get_value<circle>();


    std::cout << "Circle: c_1:\n" << json_string << std::endl;


    io::from_json(json_string, c_2); // deserialize it with the content of 'c_1'
    std::cout << "\n############################################\n" << std::endl;

    std::cout << "Circle c_2:\n" << io::to_json(c_2) << std::endl;

    return 0;
}

