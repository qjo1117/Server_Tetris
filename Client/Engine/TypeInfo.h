#pragma once

//class Method;
//class TypeInfo;
//
//
///*----------------
//	 TypeInit
//-----------------*/
//
//template <typename T>
//concept HasSuper = requires
//{
//	typename T::Super;
//} && !std::same_as<typename T::Super, void>;
//
//template <typename T>
//concept HasStaticTypeInfo = requires
//{
//	T::StaticTypeInfo();
//};
//
//template <typename T, typename U = void>
//struct SuperClassTypeDeduction
//{
//	using Type = void;
//};
//
//template <typename T>
//struct SuperClassTypeDeduction<T, std::void_t<typename T::ThisType>>
//{
//	using Type = T::ThisType;
//};
//
//template <typename T>
//struct TypeInfoInitializer
//{
//	TypeInfoInitializer(const char* name) :
//		m_name(name)
//	{
//		if constexpr (HasSuper<T>)
//		{
//			m_super = &(typename T::Super::StaticTypeInfo());
//		}
//	}
//
//	const char* m_name = nullptr;
//	const TypeInfo* m_super = nullptr;
//};
//
//
///*----------------
//	 TypeInfo
//-----------------*/
//
//class TypeInfo
//{
//public:
//	template<typename T>
//	TypeInfo(const TypeInfoInitializer<T>& initTypeInfo) :
//		m_typeHash(static_cast<uint32>(typeid(T).hash_code())),
//		m_name(initTypeInfo.m_name),
//		m_typeName(typeid(T).name()),
//		m_super(initTypeInfo.m_super)
//	{
//	}
//
//	const TypeInfo* GetSuper() const;
//
//	template <typename T> requires HasStaticTypeInfo<T>
//	static const TypeInfo& GetStaticTypeInfo()
//	{
//		return T::StaticTypeInfo();
//	}
//
//	template <typename T> requires
//		std::is_pointer_v<T>
//		&& HasStaticTypeInfo<std::remove_pointer_t<T>>
//		static const TypeInfo& GetStaticTypeInfo()
//	{
//		return std::remove_pointer_t<T>::StaticTypeInfo();
//	}
//
//	template<typename T> requires
//		(!HasStaticTypeInfo<T>) &&
//		(!HasStaticTypeInfo<std::remove_pointer_t<T>>)
//		static const TypeInfo& GetStaticTypeInfo()
//	{
//		static TypeInfo typeInfo{ TypeInfoInitializer<T>("unreflected_type_variable") };
//		return typeInfo;
//	}
//
//	bool IsA(const TypeInfo& other) const;
//	bool IsChildOf(const TypeInfo& other) const;
//
//	template<typename T>
//	bool IsA() const
//	{
//		return IsA(GetStaticTypeInfo<T>());
//	}
//
//	template<typename T>
//	bool IsChildOf() const
//	{
//		return IsChildOf(GetStaticTypeInfo<T>());
//	}
//
//	const std::vector<const Method*>& GetMethods() const
//	{
//		return m_methods;
//	}
//
//	const Method* GetMethod(const char* name) const;
//
//
//private:
//	friend Method;
//	using MethodMap = std::map<std::string_view, const Method*>;
//
//	void AddMethod(const Method* method);
//
//	uint32 m_typeHash;
//	const char* m_name = nullptr;
//	std::string m_typeName;
//	const TypeInfo* m_super = nullptr;
//
//	std::vector<const Method*> m_methods;
//	MethodMap m_methodMap;
//};
//
//
///* ---------------------------------------------------------------------------------
//	잘 정리해서 이해하기용
//	일단 TypeInfo지만 처음에 생각한 것은 캐스팅속도를 높이기 위함이다.
//	class A를 만든뒤 TypeInfo를 해준뒤
//	그 객체에 대해서 자신만의 hash코드를 가지고 있고 상위 부모와의
//	hash비교를 한뒤 캐스팅이 가능한지 여부를 따지는 것이다.
//
//	그래서 첫번째 작업은 자신의 이름, 코드, 부모를 얻어서 캐스팅 가능여부를 따지는것
//	이것이 주 목적이다.
//	https://www.slideshare.net/xtozero/c20-251161090
//
//	느낌점 Template와 concept를 이해하는 과정이 역시 힘들다.
//	대충 사용법은 알겠지만 구현을 할 수 있냐는건 버겁다.
//	하지만 코드를 보면서 이런 방식으로 구현이 가능하다는 느낌을 받을 수 있었다.
//	그러므로 더 공부해야지...
//--------------------------------------------------------------------------------- */
//
//
//template <typename To, typename From>
//To* Cast(From* src)
//{
//	return src && src->GetTypeInfo().IsChildOf<To>() ? reinterpret_cast<To*>(src) : nullptr;
//}
//
//
//#define GENERATE_CLASS_TYPE_INFO( TypeName )										\
//private:																			\
//	friend SuperClassTypeDeduction;													\
//	friend TypeInfoInitializer;														\
//																					\
//public:																				\
//	using Super = typename SuperClassTypeDeduction<TypeName>::Type;					\
//	using ThisType = TypeName;														\
//																					\
//	static TypeInfo& StaticTypeInfo()												\
//	{																				\
//		static TypeInfo typeInfo{ TypeInfoInitializer<ThisType>( #TypeName ) };		\
//		return typeInfo;															\
//	}																				\
//																					\
//	virtual const TypeInfo& GetTypeInfo() const										\
//	{																				\
//		return m_typeInfo;															\
//	}																				\
//																					\
//private:																			\
//	inline static TypeInfo& m_typeInfo = StaticTypeInfo();						