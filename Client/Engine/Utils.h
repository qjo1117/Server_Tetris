#pragma once


class Utils
{
public:
	static wstring Str2Wstr(const string& str)
	{
		int32 len;
		int32 slength = static_cast<int32>(str.length()) + 1;
		len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
		wstring ret(buf);
		delete[] buf;
		return ret;
	}

	static string Wstr2Str(const wstring& str)
	{
		int32 len;
		int32 slength = static_cast<int32>(str.length()) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), slength, 0, 0, 0, 0);
		string r(len, '\0');
		::WideCharToMultiByte(CP_ACP, 0, str.c_str(), slength, &r[0], len, 0, 0);
		return r;
	}

	static string Vec3ToStr(const Vec3& vec)
	{
		return to_string(vec.x) + "," + to_string(vec.y) + "," + to_string(vec.z);
	}

	static vector<string> Split(const string& str, char base)
	{
		vector<string> ret;
		int32 size = static_cast<int32>(str.size());
		string strTemp = "";
		for (int32 i = 0; i < size; ++i) {
			if (str[i] == base) {
				ret.push_back(strTemp);
				strTemp = "";
			}
			else {
				strTemp.push_back(str[i]);
			}
		}

		if (strTemp.empty() == false) {
			ret.push_back(strTemp);
		}

		return ret;
	}

	static vector<wstring> Split(const wstring& p_str, char base)
	{
		vector<wstring> ret;
		int32 size = static_cast<int32>(p_str.size());
		wstring strTemp = L"";
		for (int32 i = 0; i < size; ++i) {
			if (p_str[i] == base) {
				ret.push_back(strTemp);
				strTemp = L"";
			}
			else {
				strTemp.push_back(p_str[i]);
			}
		}

		if (strTemp.empty() == false) {
			ret.push_back(strTemp);
		}

		return ret;
	}

	/* ----------- 불편해서 만든것 ----------- */
	static Matrix CreateScaling(const Vec3& scaling)
	{
		Matrix mat;
		::D3DXMatrixScaling(&mat, scaling.x, scaling.y, scaling.z);
		return mat;
	}

	static Matrix CreateRotationX(float angle)
	{
		Matrix mat;
		::D3DXMatrixRotationX(&mat, angle);
		return mat;
	}

	static Matrix CreateRotationY(float angle)
	{
		Matrix mat;
		::D3DXMatrixRotationY(&mat, angle);
		return mat;
	}

	static Matrix CreateRotationZ(float angle)
	{
		Matrix mat;
		::D3DXMatrixRotationZ(&mat, angle);
		return mat;
	}

	static Matrix CreateTranslation(const Vec3& trans)
	{
		Matrix mat;
		::D3DXMatrixTranslation(&mat, trans.x, trans.y, trans.z);
		return mat;
	}
	
	static Vec3 Vec4ToVec3(const Vec4& vec) 
	{
		return Vec3{ vec.x,vec.y,vec.z };
	}

	static Vec3 Vec3ToMatrix(const Vec3& vec, const Matrix& mat)
	{
		Vec4 ret;
		::D3DXVec3Transform(&ret, &vec, &mat);
		return Utils::Vec4ToVec3(ret);
	}

	//Vector3 Up() const noexcept { return Vector3(_21, _22, _23); }
	//void Up(const Vector3& v) noexcept { _21 = v.x; _22 = v.y; _23 = v.z; }

	//Vector3 Down() const  noexcept { return Vector3(-_21, -_22, -_23); }
	//void Down(const Vector3& v) noexcept { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

	//Vector3 Right() const noexcept { return Vector3(_11, _12, _13); }
	//void Right(const Vector3& v) noexcept { _11 = v.x; _12 = v.y; _13 = v.z; }

	//Vector3 Left() const noexcept { return Vector3(-_11, -_12, -_13); }
	//void Left(const Vector3& v) noexcept { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

	//Vector3 Forward() const noexcept { return Vector3(-_31, -_32, -_33); }
	//void Forward(const Vector3& v) noexcept { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

	//Vector3 Backward() const noexcept { return Vector3(_31, _32, _33); }
	//void Backward(const Vector3& v) noexcept { _31 = v.x; _32 = v.y; _33 = v.z; }

	//Vector3 Translation() const  noexcept { return Vector3(_41, _42, _43); }
	//void Translation(const Vector3& v) noexcept { _41 = v.x; _42 = v.y; _43 = v.z; }

	static Vec3 Translation(const Matrix& mat) 
	{
		/*_41, _42, _43*/
		return { mat.m[3][0], mat.m[3][1] ,mat.m[3][2] };
	}
	static Vec3 Up(const Matrix& mat)
	{
		/*_21, _22, _23*/
		return { mat.m[1][0],mat.m[1][1],mat.m[1][2] };
	}
	static Vec3 Right(const Matrix& mat)
	{
		/*_11, _12, _13*/
		return { mat.m[0][0],mat.m[0][1],mat.m[0][2] };
	}
	static Vec3 Forward(const Matrix& mat)
	{
		/*-_31, -_32, -_33*/
		return { mat.m[2][0], mat.m[2][1], mat.m[2][2] };
	}
	static Vec3 Left(const Matrix& mat)
	{
		return -Utils::Right();
	}
	static Vec3 Down(const Matrix& mat)
	{
		return -Utils::Up();
	}
	static Vec3 BackForward(const Matrix& mat)
	{
		return -Utils::Forward();
	}
	static Vec3 Normalize(const Vec3& vec)
	{
		Vec3 ret = vec;
		float dist = (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
		ret = vec / dist;
		return ret;
	}
	inline static const float DX_PI = D3DX_PI;
	inline static const float DX_2PI = 2 * D3DX_PI;


	inline static const Matrix Identity =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
	};


	static Vec3 Forward()
	{
		return Vec3{ 0.0f, 0.0f, 1.0f };
	}
	static Vec3 BackForward()
	{
		return Vec3{ 0.0f, 0.0f, -1.0f };
	}
	static Vec3 Right()
	{
		return Vec3{ 1.0f, 0.0f, 0.0f };
	}
	static Vec3 Left()
	{
		return Vec3{ -1.0f, 0.0f, 0.0f };
	}
	static Vec3 Up()
	{
		return Vec3{ 0.0f, 1.0f, 0.0f };
	}
	static Vec3 Down()
	{
		return Vec3{ 0.0f, -1.0f, 0.0f };
	}
	static Vec3 Zero()
	{
		return Vec3{ 0.0f, 0.0f, 0.0f };
	}
	static Vec3 One()
	{
		return Vec3{ 1.0f, 1.0f, 1.0f };
	}

	/* ------------- Color ------------- */

	static D3DXCOLOR Red()
	{
		return D3DXCOLOR{ 1.0f, 0.0f, 0.0f, 1.0f };
	}
	static D3DXCOLOR Green()
	{
		return D3DXCOLOR{ 0.0f, 1.0f, 0.0f, 1.0f };
	}
	static D3DXCOLOR Blue()
	{
		return D3DXCOLOR{ 0.0f, 0.0f, 1.0f, 1.0f };
	}
	static D3DXCOLOR Cyan()
	{
		return D3DXCOLOR{ 0.0f, 1.0f, 1.0f, 1.0f };
	}
	static D3DXCOLOR Yellow()
	{
		return D3DXCOLOR{ 1.0f, 1.0f, 0.0f, 1.0f };
	}
	static D3DXCOLOR IceBlue()
	{
		return D3DXCOLOR{ 0.0f, 0.5f, 0.8f, 1.0f };
	}
	static D3DXCOLOR Brown()
	{
		return D3DXCOLOR{ 0.3f, 0.0f, 0.01f, 1.0f };
	}
	static D3DXCOLOR Gold()
	{
		return D3DXCOLOR{ 1.0f, 1.0f, 0.0f, 1.0f };
	}
	static D3DXCOLOR Pink()
	{
		return D3DXCOLOR{ 1.0f, 0.0f, 1.0f, 1.0f };
	}
	static D3DXCOLOR Black()
	{
		return D3DXCOLOR{ 0.0f, 0.0f, 0.0f, 1.0f };
	}
	static D3DXCOLOR White()
	{
		return D3DXCOLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	}


	template<typename T>
	int32 EnumToInt32(T type)
	{
		return static_cast<uint32>(type);
	}
};