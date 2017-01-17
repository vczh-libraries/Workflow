#ifndef VCZH_WORKFLOW_UNITTEST_CPPTYPES
#define VCZH_WORKFLOW_UNITTEST_CPPTYPES

#include "../../Import/Vlpp.h"

namespace test
{
	using namespace vl;
	using namespace vl::collections;
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
	inline Seasons operator&(Seasons a, Seasons b) { return static_cast<Seasons>(static_cast<vuint64_t>(a) & static_cast<vuint64_t>(b)); }
	inline Seasons operator|(Seasons a, Seasons b) { return static_cast<Seasons>(static_cast<vuint64_t>(a) | static_cast<vuint64_t>(b)); }

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

	class Hinters : public Object, public Description<Hinters>
	{
	public:
		List<vint>							list;
		Array<vint>							array;
		SortedList<vint>					sortedList;
		Dictionary<vint, vint>				dictionary;

		List<vint>&							GetList(List<vint>& xs);
		const List<vint>&					GetReadonlyList(const List<vint>& xs);
		const Array<vint>&					GetReadonlyArray(const Array<vint>& xs);
		const SortedList<vint>&				GetReadonlySL(const SortedList<vint>& xs);

		List<vint>&							GetStorage();
		void								SetStorage(List<vint>& xs);
	};
	extern Ptr<Hinters>						CreateHinter();
	extern Hinters*							CreateHinter(vint);
	extern Dictionary<vint, vint>&			GetDictionaryFromHinter(Hinters* hinter, Dictionary<vint, vint>& xs);
	extern const Dictionary<vint, vint>&	GetReadonlyDictionaryFromHinter(Hinters* hinter, const Dictionary<vint, vint>& xs);
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
			F(test::Hinters)\

			UNITTEST_TYPELIST(DECL_TYPE_INFO)

			extern bool						LoadCppTypes();
		}
	}
}

#endif
