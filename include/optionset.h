#pragma once

#include <set>
#include <vector>
#include "exception.h"

class OptionSet
{
public:
	typedef std::vector<std::string>		VEC_STR;
	typedef std::set<VEC_STR>				SET_VEC_STR;

public:
	OptionSet();
	~OptionSet();

	static int GetSubstitute(const std::string& src, size_t pos, const char l_border, const char r_border, std::string& sub, unsigned int& size);

	static bool TryComplexSubstitute(const std::string& subs, std::string& l_sub, int& index);

public:
	void Init(const std::string& ops) throw(base::Exception);

	bool SetOption(const std::string& op);

	std::string GetOption() const { return m_option; }
	bool GetComplex() const { return m_isComlex; }

	void Expand(const std::string& file_name, VEC_STR& vec_fn) throw(base::Exception);

private:
	bool IsComplex(const std::string& src, unsigned int& size) const;

	void InitSet(const VEC_STR& vs) throw(base::Exception);

	void InitComplexSet(const VEC_STR& vs, unsigned int size) throw(base::Exception);

private:
	std::string m_option;
	bool        m_isComlex;			// 是否为复合
	SET_VEC_STR m_setOP;
};

