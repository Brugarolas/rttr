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

class BaseParam
{
    RTTR_ENABLE();
    RTTR_REGISTRATION_FRIEND
public:

    virtual BaseParam* Clone() const = 0;

    virtual ~BaseParam() {} 
};

class FloatParam : public BaseParam
{
    RTTR_ENABLE(BaseParam);
    RTTR_REGISTRATION_FRIEND
public:
    float Value;

    FloatParam() {} 
    FloatParam(float InValue) : Value(InValue) {}

    virtual BaseParam* Clone() const override
    {
        return new FloatParam(Value);
    } 

    virtual ~FloatParam() {}
};

class ObjectParam : public BaseParam
{
    RTTR_ENABLE(BaseParam);
    RTTR_REGISTRATION_FRIEND
public:
    ObjectParam() {}
    ObjectParam(GameObject* InValue) : Value(InValue) {}

    virtual BaseParam* Clone() const override
    {
        return new ObjectParam(Value);
    }

    GameObject* Value = nullptr;
    virtual ~ObjectParam() {}
};

class PolyParam
{
    RTTR_ENABLE();
    RTTR_REGISTRATION_FRIEND

protected:
    BaseParam* _param = nullptr;

public:
    PolyParam() {}

    PolyParam(PolyParam const& InParam)
    {
        if (InParam._param)
        {
            _param = InParam._param->Clone();
        }
    }
    PolyParam& operator=(PolyParam const& InParam)
    {
        if (InParam._param)
        {
            _param = InParam._param->Clone();
        }
        return *this;
    }

    PolyParam(PolyParam&& InParam)
    {

    }

    PolyParam& operator=(PolyParam&& InParam)
    {
        return *this;
    }

    void FreeParam()
    {
        if (_param)
        {
            delete _param;
            _param = nullptr;
        }
    }

    void SetParamPtr(BaseParam* InValue)
    {
        FreeParam();
        _param = InValue;
    }

    void SetParam(GameObject* InValue)
    {
        auto nP = new ObjectParam();
        nP->Value = InValue;
        SetParamPtr(nP);
    }

    void SetParam(float InValue)
    {
        auto nP = new FloatParam();
        nP->Value = InValue;
        SetParamPtr(nP);
    }

    virtual ~PolyParam() 
    {
        FreeParam();
    }
};

class BinarySerializer
{
private:
    int valTest = 44;

public:
    BinarySerializer()
    {

    }

    virtual void DOStuff()
    {

    }
};

class Vector2
{
    RTTR_ENABLE();
    RTTR_REGISTRATION_FRIEND

public:
    float x = 0, y = 0;
public:
    Vector2() {}

    void BinarySerialize(BinarySerializer& InSerializer, bool bIsWrite)
    {

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

    Vector2 _translation;
   

    GameSceneElement(const char* InName) : GameObject(InName) { }

public:
    std::vector< PolyParam > _params;

    auto& GetTranslation() {
        return _translation;            
    }

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



void BinarySerialize(Vector2 &ioVec, BinarySerializer& InSerializer, bool bIsWrite)
{
    printf("did BinarySerialize %f, %f\n", ioVec.x, ioVec.y);

    ioVec.x = 111;
    ioVec.y = 222;
}


RTTR_REGISTRATION
{
    rttr::registration::class_<GameObject>("GameObject");

    rttr::registration::class_<BaseParam>("BaseParam");

    rttr::registration::class_<FloatParam>("FloatParam")
        .property("Value", &FloatParam::Value)(rttr::policy::prop::as_reference_wrapper);

    rttr::registration::class_<ObjectParam>("ObjectParam")
        .property("Value", &ObjectParam::Value)(rttr::policy::prop::as_reference_wrapper);

    rttr::registration::class_<PolyParam>("PolyParam")
        .property("_param", &PolyParam::_param)(rttr::policy::prop::as_reference_wrapper);

    rttr::registration::class_<GameSceneElement>("GameSceneElement")
        .property("_parent", &GameSceneElement::_parent)(rttr::policy::prop::as_reference_wrapper)
        .property("_children", &GameSceneElement::_children)(rttr::policy::prop::as_reference_wrapper)
        .property("_params", &GameSceneElement::_params)(rttr::policy::prop::as_reference_wrapper)

        .property("_translation", &GameSceneElement::_translation)(rttr::policy::prop::as_reference_wrapper)
        ;

    rttr::registration::class_<Vector2>("Vector2")
        .property("x", &Vector2::x)(rttr::policy::prop::as_reference_wrapper)
        .property("y", &Vector2::y)(rttr::policy::prop::as_reference_wrapper)
        .method("BinarySerialize", &Vector2::BinarySerialize);

    rttr::registration::method("BinarySerialize", &BinarySerialize);
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
            else if (propType.is_associative_container())
            {
                auto sub_array_view = org_prop_value.create_associative_view();

                for (auto& item : sub_array_view)
                {
                    WalkObjects(item.first, InFunction);

                    if (sub_array_view.is_key_only_type() == false)
                    {
                        WalkObjects(item.second, InFunction);
                    }
                }
            }
            else if (propType.is_class() || propType.is_pointer())
            {
                WalkObjects(org_prop_value, InFunction);
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

void DumpFunction(rttr::method &InMethod)
{
    printf("method %s\n", InMethod.get_name().to_string().c_str());

    auto curParams = InMethod.get_parameter_infos();

    for (auto& curParam : curParams)
    {
        printf(" - param %s: %s\n",
            curParam.get_name().to_string().c_str(),
            curParam.get_type().get_name().to_string().c_str());
    }
}

int main(int argc, char** argv)
{
    auto typeCheck = rttr::type::get<BinarySerializer&>();

    auto vectypeCheck = rttr::type::get<Vector2&>();

    BinarySerializer testSer;
    //Vector2 curVec;

    GameSceneElement thisGameElement("yoyo");

    auto gameEle = rttr::type::get<GameSceneElement>();
    auto hasProp = gameEle.get_property("_translation");
    
    auto curTrans = hasProp.get_value(thisGameElement);

    auto transType = curTrans.get_type();
    auto baseTransType = transType.get_wrapped_type();

    thisGameElement.GetTranslation().x = 123;
    thisGameElement.GetTranslation().y = 321;

 auto globalSerializer = rttr::type::get_global_method("BinarySerialize",
        {
            baseTransType,
            rttr::type::get<BinarySerializer&>(),
            rttr::type::get<bool>()
        });

    if (globalSerializer)
    {
        DumpFunction(globalSerializer);

        printf("FOUND globalSerializer\n");
        globalSerializer.invoke({}, curTrans, std::ref(testSer), true);
    }

    printf("after BinarySerialize %f, %f\n", thisGameElement.GetTranslation().x, thisGameElement.GetTranslation().y);

    auto methodList = vectypeCheck.get_methods();

    for (auto& curMethod : methodList)
    {
        printf("method %s\n", curMethod.get_name().to_string().c_str());

        auto curParams = curMethod.get_parameter_infos();

        for (auto& curParam : curParams)
        {
           
            printf("param %s\n", curParam.get_type().get_name().to_string().c_str());
        }
    }

    auto topElement = Make_GameObject< GameSceneElement >("Top");
    auto childA = Make_GameObject< GameSceneElement >("ChildA");
    auto childB = Make_GameObject< GameSceneElement >("ChildB");

    auto pObj = Make_GameObject< GameSceneElement >("ParamObj");

    topElement->AddChild(childA);
    topElement->AddChild(childB);

    PolyParam nP;
    nP.SetParam(pObj);

    topElement->_params.push_back(nP);

    
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
                printf("can see %s\n", InOutObj->GetName().c_str());

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

