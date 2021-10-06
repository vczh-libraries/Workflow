#include "CppTypes.h"

namespace test
{
	using namespace vl;
	using namespace vl::reflection;
	using namespace vl::reflection::description;

/***********************************************************************
ObservableValue
***********************************************************************/

	ObservableValue::ObservableValue()
		:value(0)
	{
	}

	ObservableValue::ObservableValue(vint _value)
		:value(_value)
	{
	}

	ObservableValue::ObservableValue(vint _value, Nullable<double> x)
		:value(_value)
	{
	}

	ObservableValue::ObservableValue(vint _value, Nullable<bool> x)
		:value(_value)
	{
	}

	ObservableValue::~ObservableValue()
	{
		FinalizeAggregation();
	}

	Ptr<ObservableValue> ObservableValue::Create(vint value, const WString& name)
	{
		auto ov = MakePtr<ObservableValue>(value);
		ov->name = name;
		return ov;
	}

	ObservableValue* ObservableValue::CreatePtr(vint value, const WString& name)
	{
		auto ov = new ObservableValue(value);
		ov->name = name;
		return ov;
	}

	vint ObservableValue::GetValue()
	{
		return value;
	}

	void ObservableValue::SetValue(vint newValue)
	{
		vint oldValue = value;
		value = newValue;
		ValueChanged(oldValue, newValue);
	}

	WString ObservableValue::GetName()
	{
		return name;
	}

	void ObservableValue::SetName(const WString& newName)
	{
		name = newName;
	}

	WString ObservableValue::GetDisplayName()
	{
		return L"This is " + name;
	}

/***********************************************************************
CustomInterfaceProcessor
***********************************************************************/

	vint CustomInterfaceProcessor::Sum(Ptr<IValueEnumerable> values)
	{
		vint sum = 0;
		auto it = values->CreateEnumerator();
		while (it->Next())
		{
			sum += UnboxValue<vint>(it->GetCurrent());
		}
		return sum;
	}

/***********************************************************************
Hinters
***********************************************************************/

	List<vint>& Hinters::GetList(List<vint>& xs, vint y)
	{
		CopyFrom(list, xs);
		list.Remove(y);
		return list;
	}
	
	MyList& Hinters::GetMyList()
	{
		return myList;
	}

	ObservableList<vint>& Hinters::GetObservableList()
	{
		return observableList;
	}

	const List<vint>& Hinters::GetReadonlyList(const List<vint>& xs)
	{
		CopyFrom(list, xs);
		return list;
	}

	const Array<vint>& Hinters::GetReadonlyArray(const Array<vint>& xs)
	{
		CopyFrom(array, xs);
		return array;
	}

	const SortedList<vint>& Hinters::GetReadonlySL(const SortedList<vint>& xs)
	{
		CopyFrom(sortedList, xs);
		return sortedList;
	}

	List<vint>& Hinters::GetStorage()
	{
		return list;
	}

	void Hinters::SetStorage(List<vint>& xs)
	{
		CopyFrom(list, xs);
	}

	Ptr<Hinters> CreateHinter()
	{
		return new Hinters();
	}

	Hinters* CreateHinter(vint)
	{
		return new Hinters();
	}

	Dictionary<vint, vint>& GetDictionaryFromHinter(Hinters* hinter, Dictionary<vint, vint>& xs)
	{
		CopyFrom(hinter->dictionary, xs);
		return hinter->dictionary;
	}

	const Dictionary<vint, vint>& GetReadonlyDictionaryFromHinter(Hinters* hinter, const Dictionary<vint, vint>& xs)
	{
		CopyFrom(hinter->dictionary, xs);
		return hinter->dictionary;
	}

/***********************************************************************
SyncScheduler
***********************************************************************/

	void SyncScheduler::Run(const Func<void()>& callback)
	{
		auto scheduler = MakePtr<SyncScheduler>();
		IAsyncScheduler::RegisterDefaultScheduler(scheduler);
		callback();
		while (scheduler->tasks.Count() > 0)
		{
			auto firstTask = scheduler->tasks[0];
			scheduler->tasks.RemoveAt(0);
			firstTask();
		}
		IAsyncScheduler::UnregisterDefaultScheduler();
	}

	void SyncScheduler::Execute(const Func<void()>& callback)
	{
		tasks.Add(callback);
	}

	void SyncScheduler::ExecuteInBackground(const Func<void()>& callback)
	{
		tasks.Add(callback);
	}

	void SyncScheduler::DelayExecute(const Func<void()>& callback, vint milliseconds)
	{
		tasks.Add(callback);
	}

/***********************************************************************
Metadata
***********************************************************************/
}

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			using namespace test;

#ifndef VCZH_DEBUG_NO_REFLECTION
			UNITTEST_TYPELIST(IMPL_CPP_TYPE_INFO)
#endif

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
#define _ ,

			BEGIN_ENUM_ITEM_MERGABLE(Seasons)
				ENUM_CLASS_ITEM(None)
				ENUM_CLASS_ITEM(Spring)
				ENUM_CLASS_ITEM(Summer)
				ENUM_CLASS_ITEM(Autumn)
				ENUM_CLASS_ITEM(Winter)
				ENUM_CLASS_ITEM(Good)
				ENUM_CLASS_ITEM(Bad)
			END_ENUM_ITEM(Seaspms)

			BEGIN_STRUCT_MEMBER(Point)
				STRUCT_MEMBER(x)
				STRUCT_MEMBER(y)
			END_STRUCT_MEMBER(Point)

			BEGIN_CLASS_MEMBER(PointClass)
				CLASS_MEMBER_FIELD(x)
				CLASS_MEMBER_FIELD(y)
				CLASS_MEMBER_CONSTRUCTOR(Ptr<PointClass>(), NO_PARAMETER)
			END_CLASS_MEMBER(PointClass)

			BEGIN_CLASS_MEMBER(ObservableValue)
				CLASS_MEMBER_CONSTRUCTOR(Ptr<ObservableValue>(), NO_PARAMETER)
				CLASS_MEMBER_CONSTRUCTOR(Ptr<ObservableValue>(vint), { L"value" })
				CLASS_MEMBER_CONSTRUCTOR(ObservableValue*(vint, Nullable<double>), { L"value" _ L"x" })
				CLASS_MEMBER_CONSTRUCTOR(ObservableValue*(vint, Nullable<bool>), { L"value" _ L"x" })
				CLASS_MEMBER_STATIC_METHOD(Create, { L"value" _ L"name" })
			CLASS_MEMBER_STATIC_METHOD(CreatePtr, { L"value" _ L"name" })

			CLASS_MEMBER_EVENT(ValueChanged)
				CLASS_MEMBER_PROPERTY_EVENT_FAST(Value, ValueChanged)
				CLASS_MEMBER_PROPERTY_FAST(Name)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(DisplayName);
			END_CLASS_MEMBER(ObservableValue)

			BEGIN_CLASS_MEMBER(CustomInterfaceProcessor)
				CLASS_MEMBER_STATIC_METHOD(Sum, { L"values" })
			END_CLASS_MEMBER(CustomInterfaceProcessor)

			BEGIN_CLASS_MEMBER(Hinters)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<Hinters>(), NO_PARAMETER, test::CreateHinter)
				CLASS_MEMBER_STATIC_EXTERNALMETHOD(CreateHinter, { L"x" }, Hinters*(*)(vint), test::CreateHinter)
				CLASS_MEMBER_METHOD(GetList, { L"xs" _ L"y" })
				CLASS_MEMBER_METHOD(GetReadonlyList, { L"xs" })
				CLASS_MEMBER_METHOD(GetReadonlyArray, { L"xs" })
				CLASS_MEMBER_METHOD_RENAME(GetReadonlySortedList, GetReadonlySL, { L"xs" })

				using T1 = Dictionary<vint, vint>&(Hinters::*)(Dictionary<vint, vint>&);
				using T2 = const Dictionary<vint, vint>&(Hinters::*)(const Dictionary<vint, vint>&);
				CLASS_MEMBER_EXTERNALMETHOD(GetDictionary, { L"xs" }, T1, test::GetDictionaryFromHinter)
				CLASS_MEMBER_EXTERNALMETHOD(GetReadonlyDictionary, { L"xs" }, T2, test::GetReadonlyDictionaryFromHinter)

				CLASS_MEMBER_PROPERTY_FAST(Storage)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(MyList)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ObservableList)
			END_CLASS_MEMBER(Hinters)

			BEGIN_CLASS_MEMBER(SyncScheduler)
				CLASS_MEMBER_BASE(IAsyncScheduler)
				CLASS_MEMBER_STATIC_METHOD(Run, { L"callback "})
			END_CLASS_MEMBER(SyncScheduler)

			class UnitTestTypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					UNITTEST_TYPELIST(ADD_TYPE_INFO)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif

			bool LoadCppTypes()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				ITypeManager* manager = GetGlobalTypeManager();
				if (manager)
				{
					Ptr<ITypeLoader> loader = new UnitTestTypeLoader;
					return manager->AddTypeLoader(loader);
				}
#endif
				return false;
			}

#undef _
		}
	}
}
