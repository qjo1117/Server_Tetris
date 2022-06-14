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
//	�� �����ؼ� �����ϱ��
//	�ϴ� TypeInfo���� ó���� ������ ���� ĳ���üӵ��� ���̱� �����̴�.
//	class A�� ����� TypeInfo�� ���ص�
//	�� ��ü�� ���ؼ� �ڽŸ��� hash�ڵ带 ������ �ְ� ���� �θ����
//	hash�񱳸� �ѵ� ĳ������ �������� ���θ� ������ ���̴�.
//
//	�׷��� ù��° �۾��� �ڽ��� �̸�, �ڵ�, �θ� �� ĳ���� ���ɿ��θ� �����°�
//	�̰��� �� �����̴�.
//	https://www.slideshare.net/xtozero/c20-251161090
//
//	������ Template�� concept�� �����ϴ� ������ ���� �����.
//	���� ������ �˰����� ������ �� �� �ֳĴ°� ���̴�.
//	������ �ڵ带 ���鼭 �̷� ������� ������ �����ϴٴ� ������ ���� �� �־���.
//	�׷��Ƿ� �� �����ؾ���...
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