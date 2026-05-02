#pragma once
#include<memory>
#include<utility>

namespace Win32
{
	template<class Type>
	class ComPtr
	{
	private:
		Type* ptr;

	public:
		ComPtr()
			:  ptr(nullptr)
		{
		}
		ComPtr(decltype(nullptr) null)
			: ptr(null)
		{
		}
		explicit ComPtr(Type* ptr)
			: ptr(ptr)
		{
			if (ptr) {
				ptr->AddRef();
			}
		}
		ComPtr(const ComPtr& other)
			: ptr(other.ptr)
		{
			if (ptr) {
				ptr->AddRef();
			}
		}
		ComPtr(ComPtr&& other) noexcept
			: ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		ComPtr<Type>& operator=(const ComPtr& other)
		{
			if (this == &other)
			{
				return *this;
			}

			if (other.ptr) {
				other.ptr->AddRef();
			}
			if (ptr) {
				ptr->Release();
			}
			ptr = other.ptr;

			return *this;
		}

		ComPtr<Type>& operator=(ComPtr&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			if (ptr) {
				ptr->Release();
			}
			ptr = other.ptr;
			other.ptr = nullptr;

			return *this;
		}

		Type* operator->() noexcept
		{
			return ptr;
		}
		Type* operator->() const noexcept
		{
			return ptr;
		}

		~ComPtr()
		{
			Reset();
		}

		Type* Get() const noexcept
		{
			return ptr;
		}

		Type** GetAddressOf() noexcept
		{
			return &ptr;
		}

		void Reset()
		{
			if (ptr) {
				ptr->Release();
				ptr = nullptr;
			}
		}
	};
}