#include "../../Import/Vlpp.h"

namespace test
{
	using namespace vl;
	using namespace vl::reflection;
	using namespace vl::reflection::description;

	enum class Seasons
	{
		None = 0,
		Spring = 1,
		Summer = 2,
		Autumn = 4,
		Winter = 8,

		Good = Spring | Autumn,
		Bad = Summer | Winter,
	};
	inline Seasons operator&(Seasons a, Seasons b) { return static_cast<Seasons>(static_cast<vuint64_t>(a) & static_cast<vuint64_t>(a)); }
	inline Seasons operator|(Seasons a, Seasons b) { return static_cast<Seasons>(static_cast<vuint64_t>(a) | static_cast<vuint64_t>(a)); }

	struct Point
	{
		vint								x = 0;
		vint								y = 0;
	};
	inline bool operator==(Point a, Point b) { return a.x == b.x && a.y == b.y; }
	inline bool operator!=(Point a, Point b) { return a.x != b.x || a.y != b.y; }

	class PointClass : public Object, public Description<PointClass>
	{
	public:
		vint								x = 0;
		vint								y = 0;
	};

	class ObservableValue : public Object, public AggregatableDescription<ObservableValue>
	{
	protected:
		vint								value;
		WString								name;
	public:
		Event<void(vint, vint)>				ValueChanged;

		ObservableValue();
		ObservableValue(vint _value);
		ObservableValue(vint _value, Nullable<double> x);
		ObservableValue(vint _value, Nullable<bool> x);
		~ObservableValue();

		static Ptr<ObservableValue>			Create(vint value, const WString& name);
		static ObservableValue*				CreatePtr(vint value, const WString& name);

		vint								GetValue();
		void								SetValue(vint newValue);
		WString								GetName();
		void								SetName(const WString& newName);
		WString								GetDisplayName();
	};

	class CustomInterfaceProcessor : public Object, public Description<CustomInterfaceProcessor>
	{
	public:
		static vint							Sum(Ptr<IValueEnumerable> values);
	};
}

namespace vl
{
	namespace reflection
	{
		namespace description
		{
#define UNITTEST_TYPELIST(F)\
			F(test::Seasons)\
			F(test::Point)\
			F(test::PointClass)\
			F(test::ObservableValue)\
			F(test::CustomInterfaceProcessor)\

			UNITTEST_TYPELIST(DECL_TYPE_INFO)

			extern bool						LoadCppTypes();
		}
	}
}

namespace vl
{
	namespace __vwsn
	{
		template<typename T>
		T* This(T* thisValue)
		{
			CHECK_ERROR(thisValue != nullptr, L"The this pointer cannot be null.");
			return thisValue;
		}

		template<typename T>
		WString ToString(const T& value)
		{
			using Type = typename RemoveCVR<T>::Type;
			WString str;
			CHECK_ERROR(reflection::description::TypedValueSerializerProvider<T>::Serialize(value, str), L"Failed to serialize.");
			return str;
		}

		template<typename T>
		T Parse(const WString& str)
		{
			using Type = typename RemoveCVR<T>::Type;
			T value;
			CHECK_ERROR(reflection::description::TypedValueSerializerProvider<T>::Deserialize(str, value), L"Failed to serialize.");
			return value;
		}

		template<typename T>
		reflection::description::Value Box(const T& value)
		{
			using Type = typename RemoveCVR<T>::Type;
			return reflection::description::BoxParameter<Type>(const_cast<T&>(value));
		}

		template<typename T>
		T Unbox(const reflection::description::Value& value)
		{
			using Type = typename RemoveCVR<T>::Type;
			T result;
			reflection::description::UnboxParameter<Type>(value, result);
			return result;
		}

		template<typename T>
		collections::LazyList<T> Range(T begin, T end)
		{
			return collections::Range<T>(begin, end - begin);
		}

		template<typename T>
		bool InSet(const T& value, const collections::LazyList<T>& collection)
		{
			return collection.Any([&](const T& element) {return element == value; });
		}

		template<typename T>
		bool InSet(const T& value, Ptr<reflection::description::IValueReadonlyList> collection)
		{
			return InSet<T>(value, reflection::description::GetLazyList<T>(collection));
		}
	}
}