#include "WfLibraryCppHelper.h"

namespace vl
{
	namespace __vwsn
	{

/***********************************************************************
CreateList
***********************************************************************/

		CreateList::CreateList()
			:list(IValueList::Create())
		{
		}

		CreateList::CreateList(Ptr<IValueList> _list)
			:list(_list)
		{
		}

/***********************************************************************
CreateObservableList
***********************************************************************/

		CreateObservableList::CreateObservableList()
			:list(IValueObservableList::Create())
		{
		}

		CreateObservableList::CreateObservableList(Ptr<IValueObservableList> _list)
			:list(_list)
		{
		}

/***********************************************************************
CreateDictionary
***********************************************************************/

		CreateDictionary::CreateDictionary()
			:dictionary(IValueDictionary::Create())
		{
		}

		CreateDictionary::CreateDictionary(Ptr<IValueDictionary> _dictionary)
			:dictionary(_dictionary)
		{
		}
	}
}