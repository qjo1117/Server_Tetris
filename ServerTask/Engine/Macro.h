#pragma once

#include <mutex>

#define OUT

/* -------- Simple SingleTon -------- */

#define DECLARE_SINGLE(Type) 							\
private:												\
	Type() {}											\
	~Type() {}											\
public:													\
static Type* GetI()										\
{														\
	static Type instance;								\
	return &instance;									\
}														\


#define GET_SINGLE(Type)		Type::GetI()
#define DEL_SINGLE(Type)		Type::Delete()

/*---------------
	  Crash
---------------*/

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

#define SAFEDELETE(ptr)		\
if (ptr) {					\
		delete ptr;			\
}							\
ptr = nullptr				


/* ------------------------------
			Property
------------------------------ */

#define PROPERTY(_get, _set) _declspec(property(get = _get, put = _set))
#define G_PROPERTY(_get) _declspec(property(get = _get))
#define S_PROPERTY(_set) _declspec(property(set = _set))

#define SETMEMBER(type, member)										\
public:																\
	void Set##member(const type& p_val) { m_##member = p_val; }     

#define GETMEMBER(type, member)										\
public:																\
	type& Get##member() { return m_##member; }				

/* ----- 김치삼 교수님께서 알려주신 PROPERTY (개조 버전) ----- */

#define PUBLIC_PROPERTY(type, member)								\
public:                                                             \
	SETMEMBER(type, member)											\
	GETMEMBER(type, member)											\
	PROPERTY(Get##member, Set##member) type& ##member;				\
public:                                                             \
	type m_##member                                                 

#define PRIVATE_PROPERTY(type, member)								\
public:                                                             \
	SETMEMBER(type, member)											\
	GETMEMBER(type, member)											\
	PROPERTY(Get##member,  Set##member) type& ##member;				\
private:                                                            \
	type m_##member                                                

#define PROTECTED_PROPERTY(type, member)							\
public:                                                             \
	SETMEMBER(type, member)											\
	GETMEMBER(type, member)											\
	PROPERTY(Get##member,  Set##member) type& ##member;				\
protected:                                                          \
	type m_##member                                                   

/* ------------------------------------------------------------
	어쩌다보니 Attribute찾다가 property에 대해서 흥미가 돋아서
	알고 있는 코드와 조합을 시켜서 만들어 보았습니다.
	https://blog.naver.com/kyuniitale/40029470165	
	_declspec(property(get = _get, put = _set)) 참고 자료 ▲
------------------------------------------------------------ */


// Enum Class로 재미있는 게 보여서 가지고옴

// Copyright Epic Games, Inc. All Rights Reserved.

// Defines all bitwise operators for enum classes so it can be (mostly) used as a regular flags enum
#define ENUM_CLASS_FLAGS(Enum) \
	inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }

// Friends all bitwise operators for enum classes so the definition can be kept private / protected.
#define FRIEND_ENUM_CLASS_FLAGS(Enum) \
	friend           Enum& operator|=(Enum& Lhs, Enum Rhs); \
	friend           Enum& operator&=(Enum& Lhs, Enum Rhs); \
	friend           Enum& operator^=(Enum& Lhs, Enum Rhs); \
	friend constexpr Enum  operator| (Enum  Lhs, Enum Rhs); \
	friend constexpr Enum  operator& (Enum  Lhs, Enum Rhs); \
	friend constexpr Enum  operator^ (Enum  Lhs, Enum Rhs); \
	friend constexpr bool  operator! (Enum  E); \
	friend constexpr Enum  operator~ (Enum  E);

template<typename Enum>
constexpr bool EnumHasAllFlags(Enum Flags, Enum Contains)
{
	return (((__underlying_type(Enum))Flags) & (__underlying_type(Enum))Contains) == ((__underlying_type(Enum))Contains);
}

template<typename Enum>
constexpr bool EnumHasAnyFlags(Enum Flags, Enum Contains)
{
	return (((__underlying_type(Enum))Flags) & (__underlying_type(Enum))Contains) != 0;
}

template<typename Enum>
void EnumAddFlags(Enum& Flags, Enum FlagsToAdd)
{
	Flags |= FlagsToAdd;
}

template<typename Enum>
void EnumRemoveFlags(Enum& Flags, Enum FlagsToRemove)
{
	Flags &= ~FlagsToRemove;
}