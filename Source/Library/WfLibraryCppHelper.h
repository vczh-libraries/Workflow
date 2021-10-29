/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

Interfaces:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_CPPLIBRARY
#define VCZH_WORKFLOW_LIBRARY_CPPLIBRARY

#include <VlppReflection.h>

namespace vl
{
	namespace __vwsn
	{
		template<typename T>
		struct RunOnExit
		{
			T* function;

			RunOnExit(T* _function)
				:function(_function)
			{
			}

			~RunOnExit()
			{
				function->operator()();
			}
		};

		template<typename T>
		T* This(T* thisValue)
		{
			CHECK_ERROR(thisValue != nullptr, L"The this pointer cannot be null.");
			return thisValue;
		}

		template<typename T>
		T* Ensure(T* pointer)
		{
			CHECK_ERROR(pointer != nullptr, L"The pointer cannot be null.");
			return pointer;
		}

		template<typename T>
		Ptr<T>& Ensure(Ptr<T>& pointer)
		{
			CHECK_ERROR(pointer != nullptr, L"The pointer cannot be null.");
			return pointer;
		}

		template<typename T>
		Ptr<T> Ensure(Ptr<T>&& pointer)
		{
			CHECK_ERROR(pointer != nullptr, L"The pointer cannot be null.");
			return std::move(pointer);
		}

		template<typename T>
		Nullable<T> Ensure(Nullable<T>&& nullable)
		{
			CHECK_ERROR(nullable, L"The pointer cannot be null.");
			return std::move(nullable);
		}

		template<typename T>
		Nullable<T>& Ensure(Nullable<T>& nullable)
		{
			CHECK_ERROR(nullable, L"The pointer cannot be null.");
			return nullable;
		}

		template<typename T>
		WString ToString(const T& value)
		{
			WString str;
			CHECK_ERROR(reflection::description::TypedValueSerializerProvider<std::remove_cvref_t<T>>::Serialize(value, str), L"Failed to serialize.");
			return str;
		}

		template<typename T>
		T Parse(const WString& str)
		{
			T value;
			CHECK_ERROR(reflection::description::TypedValueSerializerProvider<std::remove_cvref_t<T>>::Deserialize(str, value), L"Failed to serialize.");
			return value;
		}

		template<typename TTo, typename TFrom>
		struct NullableCastHelper
		{
			static Nullable<TTo> Cast(Nullable<TFrom> nullable)
			{
				return Nullable<TTo>(static_cast<TTo>(nullable.Value()));
			}
		};

		template<typename TFrom>
		struct NullableCastHelper<WString, TFrom>
		{
			static Nullable<WString> Cast(Nullable<TFrom> nullable)
			{
				return Nullable<WString>(ToString(nullable.Value()));
			}
		};

		template<typename TTo>
		struct NullableCastHelper<TTo, WString>
		{
			static Nullable<TTo> Cast(Nullable<WString> nullable)
			{
				return Nullable<TTo>(Parse<TTo>(nullable.Value()));
			}
		};

		template<typename TTo, typename TFrom>
		Nullable<TTo> NullableCast(Nullable<TFrom> nullable)
		{
			if (!nullable) return Nullable<TTo>();
			return NullableCastHelper<TTo, TFrom>::Cast(nullable);
		}

		template<typename TTo, typename TFrom>
		TTo* RawPtrCast(TFrom* pointer)
		{
			if (!pointer) return nullptr;
			return pointer->template SafeAggregationCast<TTo>();
		}

		template<typename TTo, typename TFrom>
		Ptr<TTo> SharedPtrCast(TFrom* pointer)
		{
			if (!pointer) return nullptr;
			return pointer->template SafeAggregationCast<TTo>();
		}

		template<typename T>
		reflection::description::Value Box(T&& value)
		{
			return reflection::description::BoxParameter(value);
		}

		template<typename T>
		T Unbox(const reflection::description::Value& value)
		{
			T result;
			reflection::description::UnboxParameter(value, result);
			return result;
		}

		template<typename T>
		struct UnboxWeakHelper
		{
		};

		template<typename T>
		struct UnboxWeakHelper<T*>
		{
			static T* Unbox(const reflection::description::Value& value)
			{
				if (value.IsNull()) return nullptr;
				return value.GetRawPtr()->SafeAggregationCast<T>();
			}
		};

		template<typename T>
		struct UnboxWeakHelper<Ptr<T>>
		{
			static Ptr<T> Unbox(const reflection::description::Value& value)
			{
				if (value.IsNull()) return nullptr;
				return value.GetRawPtr()->SafeAggregationCast<T>();
			}
		};

		template<typename T>
		struct UnboxWeakHelper<Nullable<T>>
		{
			static Nullable<T> Unbox(const reflection::description::Value& value)
			{
				if (value.IsNull()) return Nullable<T>();
				auto boxed = value.GetBoxedValue().Cast<reflection::description::IValueType::TypedBox<T>>();
				if (!boxed) return Nullable<T>();
				return Nullable<T>(boxed->value);
			}
		};

		template<typename T>
		T UnboxWeak(const reflection::description::Value& value)
		{
			return UnboxWeakHelper<std::remove_cvref_t<T>>::Unbox(value);
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

		template<typename T, typename U>
		Ptr<T> UnboxCollection(U&& value)
		{
			auto boxedValue = reflection::description::BoxParameter(value);
			Ptr<T> result;
			reflection::description::UnboxParameter(boxedValue, result);
			return result;
		}

		template<typename T, typename U>
		Ptr<T> UnboxCollection(const collections::LazyList<U>& value)
		{
			auto boxedValue = reflection::description::BoxParameter(const_cast<collections::LazyList<U>&>(value));
			Ptr<T> result;
			reflection::description::UnboxParameter(boxedValue, result);
			return result;
		}

		struct CreateArray
		{
			using IValueArray = reflection::description::IValueArray;

			Ptr<IValueArray>		list;

			CreateArray();
			CreateArray(Ptr<IValueArray> _list);

			CreateArray Resize(vint size)
			{
				list->Resize(size);
				return{ list };
			}

			template<typename T>
			CreateArray Set(vint index, const T& value)
			{
				list->Set(index, Box(value));
				return{ list };
			}
		};

		struct CreateList
		{
			using IValueList = reflection::description::IValueList;

			Ptr<IValueList>			list;

			CreateList();
			CreateList(Ptr<IValueList> _list);

			template<typename T>
			CreateList Add(const T& value)
			{
				list->Add(Box(value));
				return{ list };
			}
		};

		struct CreateObservableList
		{
			using IValueObservableList = reflection::description::IValueObservableList;

			Ptr<IValueObservableList>			list;

			CreateObservableList();
			CreateObservableList(Ptr<IValueObservableList> _list);

			template<typename T>
			CreateObservableList Add(const T& value)
			{
				list->Add(Box(value));
				return{ list };
			}
		};

		struct CreateDictionary
		{
			using IValueDictionary = reflection::description::IValueDictionary;

			Ptr<IValueDictionary>	dictionary;

			CreateDictionary();
			CreateDictionary(Ptr<IValueDictionary> _dictionary);

			template<typename K, typename V>
			CreateDictionary Add(const K& key, const V& value)
			{
				dictionary->Set(Box(key), Box(value));
				return{ dictionary };
			}
		};

		template<typename T>
		struct EventHelper
		{
		};

		template<typename T>
		Ptr<reflection::description::IEventHandler> EventAttach(T& e, typename EventHelper<T>::Handler handler)
		{
			return EventHelper<T>::Attach(e, handler);
		}

		template<typename T>
		bool EventDetach(T& e, Ptr<reflection::description::IEventHandler> handler)
		{
			return EventHelper<T>::Detach(e, handler);
		}

		template<typename T>
		decltype(auto) EventInvoke(T& e)
		{
			return EventHelper<T>::Invoke(e);
		}

		template<typename ...TArgs>
		struct EventHelper<Event<void(TArgs...)>>
		{
			using Handler = const Func<void(TArgs...)>&;

			class EventHandlerImpl : public Object, public reflection::description::IEventHandler
			{
			public:
				Ptr<EventHandler> handler;

				EventHandlerImpl(Ptr<EventHandler> _handler)
					:handler(_handler)
				{
				}

				bool IsAttached()override
				{
					return handler->IsAttached();
				}
			};

			static Ptr<reflection::description::IEventHandler> Attach(Event<void(TArgs...)>& e, Handler handler)
			{
				return MakePtr<EventHandlerImpl>(e.Add(handler));
			}

			static bool Detach(Event<void(TArgs...)>& e, Ptr<reflection::description::IEventHandler> handler)
			{
				auto impl = handler.Cast<EventHandlerImpl>();
				if (!impl) return false;
				return e.Remove(impl->handler);
			}

			static Event<void(TArgs...)>& Invoke(Event<void(TArgs...)>& e)
			{
				return e;
			}
		};
	}
}

#endif