#include "pch.h"
#include "TypeInfo.h"

//const TypeInfo* TypeInfo::GetSuper() const
//{
//	return m_super;
//}
//
//bool TypeInfo::IsA(const TypeInfo& other) const
//{
//	if (this == &other) {
//		return true;
//	}
//
//	return m_typeHash == other.m_typeHash;
//}
//
//bool TypeInfo::IsChildOf(const TypeInfo& other) const
//{
//	if (IsA(other)) {
//		return true;
//	}
//
//	for (const TypeInfo* super = m_super; super != nullptr; super = super->GetSuper()) {
//		if (super->IsA(other)) {
//			return true;
//		}
//	}
//
//	return false;
//}
//
//const Method* TypeInfo::GetMethod(const char* name) const
//{
//	auto iter = m_methodMap.find(name);
//	return (iter != std::end(m_methodMap)) ? iter->second : nullptr;
//}
//
//void TypeInfo::AddMethod(const Method* method)
//{
//	m_methods.emplace_back(method);
//	m_methodMap.emplace(method->Name(), method);
//}