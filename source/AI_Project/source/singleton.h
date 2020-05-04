/******************************************************************************/
/*!
\file		resources.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Singleton Implementation, courtesy of Chi-Hao Kuo.
*/
/******************************************************************************/
#pragma once

#include <memory>
#include <string>

// stored deleter for singleton
template <typename T>
class Singleton_Deleter
{
public:
	void operator()(const T * const instance);
};

template <typename T>
class Singleton
{
public:
	static T &GetInstance(void);

private:
	Singleton();
};

/*--------------------------------------------------------------------------*
Name:           operator()

Description:    Delete the object instance.

Arguments:      instance:		object instance to delete.

Returns:        None.
*---------------------------------------------------------------------------*/
template <typename T>
void Singleton_Deleter<T>::operator()(const T * const instance)
{
	delete instance;
}

/*--------------------------------------------------------------------------*
Name:           GetInstance

Description:    Get object instance.

Arguments:      None.

Returns:        T&:		reference of the object instance.
*---------------------------------------------------------------------------*/
template <typename T>
T &Singleton<T>::GetInstance(void)
{
	static std::unique_ptr<T, Singleton_Deleter<T>> instance_ptr(new T());

	return *instance_ptr;
}

/*--------------------------------------------------------------------------*
Name:           Singleton

Description:    Default constructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
template <typename T>
Singleton<T>::Singleton()
{
}