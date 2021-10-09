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

#include <iostream>

#include <rttr/type>

#include <rttr/registration>
#include <rttr/registration_friend>

using namespace rttr;

static class GameObject* GFirstObject = nullptr;

#define NO_COPY_ALLOWED(ClassName)						\
	ClassName(ClassName const&) = delete;				\
	ClassName& operator=(ClassName const&) = delete;

#define NO_MOVE_ALLOWED(ClassName)						\
	ClassName(ClassName&&) = delete;					\
	ClassName& operator=(ClassName&&) = delete;	

class GameObject
{
    friend void IterateObjects(const std::function<bool(GameObject*)>& InFunction);

    RTTR_ENABLE();
    RTTR_REGISTRATION_FRIEND

    NO_MOVE_ALLOWED(GameObject)
    NO_COPY_ALLOWED(GameObject)

protected:
    GameObject(const char *InName)
    {
        _nextObject = GFirstObject;
        GFirstObject = this;
        _name = InName;
    }

    std::string _name;
    GameObject* _nextObject = nullptr;
  
    uint8_t _flags = 0;
public:
    virtual ~GameObject()
    {
        // NO CLEANUP SILLY EXAMPLE
    }

    std::string GetName() const {
        return _name;
    }
    uint8_t GetFlags() const {
        return _flags;
    }
    void SetFlags(uint8_t InFlags) {
        _flags = InFlags;
    }
    void SetFlag(uint8_t InFlag) {
        _flags |= InFlag;
    }

   
};

class GameSceneElement : public GameObject
{
    friend int main(int argc, char** argv);
    template<typename T>
    friend T* Make_GameObject(const char* InName);

    RTTR_ENABLE(GameObject);
    RTTR_REGISTRATION_FRIEND

protected:
    GameSceneElement* _parent = nullptr;
    std::vector<GameSceneElement*> _children;

    GameSceneElement(const char* InName) : GameObject(InName) { }

public:
    virtual ~GameSceneElement() { }

    void AddChild(GameSceneElement* InChild)
    {
        _children.push_back(InChild);
        InChild->_parent = this;
    }
};

template<typename T>
inline T* Make_GameObject(const char* InName)
{
    static_assert(std::is_base_of_v<GameObject, T>, "Game Object");
    return new T(InName);
}

RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject");

    rttr::registration::class_<GameSceneElement>("GameSceneElement")
        .property("_parent", &GameSceneElement::_parent)(rttr::policy::prop::as_reference_wrapper)
        .property("_children", &GameSceneElement::_children)(rttr::policy::prop::as_reference_wrapper)
        ;
}

bool IsObjectProperty(rttr::type& propType)
{
    auto ObjectType = rttr::type::get<GameObject>();
    return propType.is_pointer() && propType.is_derived_from(ObjectType);
}

void WalkObjects(const rttr::variant& inValue, const std::function<bool(GameObject*&)>& InFunction)
{
    auto originalType = inValue.get_type();
    //original
    rttr::instance orgobj = inValue;
    rttr::instance obj = orgobj.get_type().get_raw_type().is_wrapper() ? orgobj.get_wrapped_instance() : orgobj;

    auto curType = obj.get_derived_type();
    auto ObjectType = rttr::type::get<GameObject>();

    GameObject* objRef = nullptr;
    if (curType.is_derived_from(ObjectType))
    {
        //knowledgeable cast
        std::reference_wrapper<GameObject*> wrappedValue =
            inValue.get_value< std::reference_wrapper<GameObject*> >();

        auto& objRef = wrappedValue.get();
        if (objRef)
        {
            if (InFunction(objRef) == false)
            {
                return;
            }
        }
        else
        {
            return;
        }
    }

    auto prop_list = curType.get_properties();
    for (auto prop : prop_list)
    {
        rttr::variant org_prop_value = prop.get_value(obj);

        if (!org_prop_value)
            continue; // cannot serialize, because we cannot retrieve the value
       
        const auto name = prop.get_name().to_string();
        auto propType = org_prop_value.get_type();
        // it is all wrappers
        if (propType.is_wrapper())
        {
            propType = propType.get_wrapped_type();

            if (IsObjectProperty(propType))
            {
                WalkObjects(org_prop_value, InFunction);
            }
            else if (propType.is_sequential_container())
            {
                auto sub_array_view = org_prop_value.create_sequential_view();
                for (auto& item : sub_array_view)
                {
                    WalkObjects(item, InFunction);
                }
            }
        }
    }
}

void IterateObjects(const std::function<bool(GameObject*)>& InFunction)
{
    auto curObject = GFirstObject;
    while (curObject)
    {
        if (InFunction(curObject) == false)
        {
            return;
        }
        curObject = curObject->_nextObject;
    }
}

static const uint8_t FLAG_Visited = 0x01;
static const uint8_t FLAG_GC = 0x02;

int main(int argc, char** argv)
{
    auto topElement = Make_GameObject< GameSceneElement >("Top");
    auto childA = Make_GameObject< GameSceneElement >("ChildA");
    auto childB = Make_GameObject< GameSceneElement >("ChildB");

    topElement->AddChild(childA);
    topElement->AddChild(childB);
    
    // make sure all flags 0
    IterateObjects([](GameObject* InObj) -> bool
        {
            InObj->SetFlags(0);
            return true;
        });

    // childA being forcefully destroyed
    childA->SetFlags(FLAG_GC);

    // clear out objects that will be garbage collected
    WalkObjects(std::ref(topElement), [](GameObject*& InOutObj) -> bool
        {
            if (InOutObj->GetFlags() & FLAG_GC)
            {
                InOutObj = nullptr;
                return false;
            }

            // prevent cyclic visiting
            if (InOutObj->GetFlags() & FLAG_Visited)
            {
                return false;
            }
            else
            {
                InOutObj->SetFlag(FLAG_Visited);
                return true;
            }
        });

    printf("SHOULD BE NULL (0x%p)\n", topElement->_children[0]);

    IterateObjects([](GameObject* InObj) -> bool
        {
            if (InObj->GetFlags() & FLAG_GC)
            {
                //DELETE InObj
                printf("FAKE DELETE %s\n", InObj->GetName().c_str());
            }

            if (InObj->GetFlags() & FLAG_Visited)
            {
                printf("Could see %s from root\n", InObj->GetName().c_str());
            }

            return true;
        });

    // make sure all flags 0
    IterateObjects([](GameObject* InObj) -> bool
        {
            InObj->SetFlags(0);
            return true;
        });
    
    // set this null to leave ChildB as a dangling object
    topElement->_children[1] = nullptr;

    WalkObjects(std::ref(topElement), [](GameObject*& InOutObj) -> bool
        {
            // prevent cyclic visiting
            if (InOutObj->GetFlags() & FLAG_Visited)
            {
                return false;
            }
            else
            {
                InOutObj->SetFlag(FLAG_Visited);
                return true;
            }
        });

    IterateObjects([](GameObject* InObj) -> bool
        {
            if (InObj->GetFlags() & FLAG_Visited)
            {
                printf("Could only see %s from root\n", InObj->GetName().c_str());
            }

            return true;
        });


    std::cout << "\n";
    return 0;
}

