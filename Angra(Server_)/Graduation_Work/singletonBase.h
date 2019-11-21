#pragma once

template <typename T>
class singletonBase
{
protected:
	//single instance declare
	static T* singleton;

	singletonBase(void)	 {};
	~singletonBase(void) {};
public:
	static T* getSingleton(void);
	void releaseSingleton(void);
};

//Init singleton
template <typename T>
T* singletonBase<T>::singleton = 0;

//get singleton
template <typename T>
T* singletonBase<T>::getSingleton(void)
{
	if (!singleton)
	{
		singleton = new T;
	}
	return singleton;
}

//release singleton
template <typename T>
void singletonBase<T>::releaseSingleton(void)
{
	if (singleton)
	{
		delete singleton;

		singleton = 0;
	}
	
}