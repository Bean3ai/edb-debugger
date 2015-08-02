/*
Copyright (C) 2006 - 2014 Evan Teran
                          eteran@alum.rit.edu

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef REGISTER_20100329_H_
#define REGISTER_20100329_H_

#include "Types.h"
#include "API.h"
#include "Util.h"
#include <cstring>
#include <QString>
#include <type_traits>

class Register;

template<std::size_t bitSize=0, typename ValueType, typename Type>
Register make_Register(const QString &name, ValueType value, Type type);

class EDB_EXPORT Register {
public:
	enum Type{
		// groups, these catagories should remain as portable as possible
		TYPE_INVALID = 0x0000,
		TYPE_GPR     = 0x0001,
		TYPE_IP      = 0x0002,
		TYPE_SEG     = 0x0004,
		TYPE_COND    = 0x0008,
		TYPE_FPU     = 0x0010,
		TYPE_SIMD    = 0x0020
	};

	// Expand when AVX instructions are supported
	typedef edb::value128 StoredType;

public:
	Register();
	Register(const Register &other);
	Register &operator=(const Register &rhs);
	
public:
	bool operator==(const Register &rhs) const;
	bool operator!=(const Register &rhs) const;

public:
	operator void*() const       { return reinterpret_cast<void*>(valid()); }

	Type type() const            { return type_; }
	QString name() const         { return name_; }
	std::size_t bitSize() const  { return bitSize_; }
	
	template <class T>
	T value() const              { return T(value_); }

private:
	bool valid() const { return type_ != TYPE_INVALID; }

private:
	QString    name_;
	StoredType value_;
	Type       type_;
	std::size_t bitSize_;

	template<std::size_t bitSize, typename ValueType, typename Type>
	friend Register make_Register(const QString &name, ValueType value, Type type);
};

template<std::size_t bitSize_, typename ValueType, typename Type>
Register make_Register(const QString &name, ValueType value, Type type)
{
	static_assert(std::is_same<Type,Register::Type>::value,"type must be Register::Type");
	constexpr std::size_t bitSize=(bitSize_ ? bitSize_ : BIT_LENGTH(value));
	static_assert(bitSize_%8==0,"Strange bit size");

	Register reg;
	reg.name_=name;
	reg.type_=type;
	reg.bitSize_=bitSize;

	constexpr std::size_t size=bitSize/8;
	static_assert(size<=sizeof(ValueType),"ValueType appears smaller than size specified");
	util::markMemory(&reg.value_,sizeof reg.value_);
	std::memcpy(&reg.value_,&value,size);

	return reg;
}

#endif
