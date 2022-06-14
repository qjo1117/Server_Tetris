#pragma once


//
//class CallableBase
//{
//	GENERATE_CLASS_TYPE_INFO(CallableBase)
//public:
//	virtual ~CallableBase() = 0;
//};
//
//template<typename TRet, typename... TArgs>
//class ICallable : public CallableBase
//{
//	GENERATE_CLASS_TYPE_INFO(ICallable)
//public:
//	virtual TRet Invoke(void* caller, TArgs&&... args) const = 0;
//};
//
//template<typename TClass, typename TRet, typename... TArgs>
//class Callable : public ICallable<TRet, TArgs...>
//{
//	GENERATE_CLASS_TYPE_INFO(Callable)
//		using FuncPtr = TRet(TClass::*)(TArgs...);
//
//public:
//	virtual TRet Invoke(void* caller, TArgs&&... args) const override
//	{
//		if constexpr (std::same_as<TRet, void>)
//		{
//			(static_cast<TClass*>(caller)->*m_ptr)(std::forward<TArgs>(args)...);
//		}
//		else {
//			return (static_cast<TClass*>(caller)->*m_ptr)(std::forward<TArgs>(args)...);
//		}
//	}
//
//	Callable(FuncPtr ptr) :
//		m_ptr(ptr) {}
//
//private:
//	FuncPtr m_ptr = nullptr;
//};
//
//template<typename TClass, typename TRet, typename... TArgs>
//class StaticCallable : public ICallable<TRet, TArgs...>
//{
//	GENERATE_CLASS_TYPE_INFO(StaticCallable)
//		using FuncPtr = TRet(*)(TArgs...);
//
//public:
//	virtual TRet Invoke([[maybe_unused]] void* caller, TArgs&&... args) const override
//	{
//		if constexpr (std::same_as<TRet, void>) {
//			(*m_ptr)(std::forward<TArgs>(args)...);
//		}
//		else {
//			return (*m_ptr)(std::forward<TArgs>(args)...);
//		}
//	}
//
//	StaticCallable([[maybe_unused]] TClass* owner, FuncPtr ptr) :
//		m_ptr(ptr) {}
//
//private:
//	FuncPtr m_ptr = nullptr;
//};
//
//class Method
//{
//public:
//	const char* Name() const {
//		return m_name;
//	}
//
//	const TypeInfo& GetReturnInfo() const
//	{
//		return *m_returnType;
//	}
//
//	const TypeInfo& GetParamerType(size_t i) const
//	{
//		return *m_parameterTypes[i];
//	}
//
//	size_t NumParamer() const
//	{
//		return m_parameterTypes.size();
//	}
//
//	template<typename TClass, typename TRet, typename... TArgs>
//	TRet Invoke(void* caller, TArgs&&... args) const
//	{
//		const TypeInfo& typeinfo = m_callable.GetTypeInfo();
//
//		if (typeinfo.IsChildOf<Callable<TClass, TRet, TArgs...>>()) {
//			auto concreateCallable = static_cast<const Callable<TClass, TRet, TArgs...>&>(m_callable);
//			if constexpr (std::same_as<TRet, bool>) {
//				concreateCallable.Invoke(caller, std::forward<TArgs>(args)...);
//			}
//			else {
//				return concreateCallable.Invoke(caller, std::forward<TArgs>(args)...);
//			}
//		}
//		else if (typeinfo.IsChildOf<StaticCallable<TClass, TRet, TArgs...>>()) {
//			auto concreateCallable = static_cast<const StaticCallable<TClass, TRet, TArgs...>&>(m_callable);
//
//			if constexpr (std::same_as<TRet, void>) {
//				concreateCallable.Invoke(caller, std::forward<TArgs>(args)...);
//			}
//			else {
//				return concreateCallable.Invoke(caller, std::forward<TArgs>(args)...);
//			}
//		}
//		else {
//			assert(false && "Method::Invoke<TClass, TRet, TArgs...> - Invalied casting");
//			if constexpr (!std::same_as<TRet, void>) {
//				return {};
//			}
//		}
//	}
//
//	template<typename TRet, typename... TArgs>
//	TRet Invoke(void* owner, TArgs&&... args) const
//	{
//		if (m_callable.GetTypeInfo().IsChildOf<ICallable<TRet, TArgs...>>()) {
//			auto concreateCallable = static_cast<const ICallable<TRet, TArgs...>*>(&m_callable);
//			if constexpr (std::same_as<TRet, void>) {
//				concreateCallable->Invoke(owner, std::forward<TArgs>(args)...);
//			}
//			else {
//				return concreateCallable->Invoke(owner, std::forward<TArgs>(args)...);
//			}
//		}
//		else {
//			assert(false && "Method::Invoke<TRet, TArgs...> - Invalied casting");
//			if constexpr (!std::same_as<TRet, void>) {
//				return {};
//			}
//		}
//	}
//
//	template <typename TRet, typename... TArgs>
//	Method(TypeInfo& owner, [[maybe_unused]] TRet(*ptr)(TArgs...), const char* name, const CallableBase& callable) :
//		m_name(name),
//		m_callable(callable)
//	{
//		CollectFunctionSignature<TRet, TArgs...>();
//		owner.AddMethod(this);
//	}
//
//	template <typename TClass, typename TRet, typename... TArgs>
//	Method(TypeInfo& owner, [[maybe_unused]] TRet(TClass::* ptr)(TArgs...), const char* name, const CallableBase& callable) :
//		m_name(name),
//		m_callable(callable)
//	{
//		CollectFunctionSignature<TRet, TArgs...>();
//		owner.AddMethod(this);
//	}
//
//private:
//	template <typename TRet, typename... Args>
//	void CollectFunctionSignature()
//	{
//		m_returnType = &TypeInfo::GetStaticTypeInfo<TRet>();
//		m_parameterTypes.reserve(sizeof...(Args));
//
//		(m_parameterTypes.emplace_back(&TypeInfo::GetStaticTypeInfo<Args>()), ...);
//	}
//
//	const TypeInfo* m_returnType = nullptr;
//	std::vector<const TypeInfo*> m_parameterTypes;
//	const char* m_name = nullptr;
//	const CallableBase& m_callable;
//};
//
//
//#define METHOD( Name )																	\
//	inline static struct RegistMethodExecutor_##Name									\
//	{																					\
//		RegistMethodExecutor_##Name()													\
//		{																				\
//			static MethodRegister<ThisType, decltype(&ThisType::Name), &ThisType::Name> \
//			method_register_##Name{ #Name, ThisType::StaticTypeInfo() };				\
//		}																				\
//																						\
//	} regist_##Name;