#include "optionset.h"
#include "errorcode.h"
#include "pubstr.h"

OptionSet::OptionSet()
:m_isComlex(false)
{
}

OptionSet::~OptionSet()
{
}

int OptionSet::GetSubstitute(const std::string& src, size_t pos, const char l_border, const char r_border, std::string& sub, unsigned int& size)
{
	// 格式：{ SUBSTITUTE }
	const size_t BEG_POS = src.find(l_border, pos);
	if ( std::string::npos == BEG_POS )
	{
		return -1;
	}

	const size_t END_POS = src.find(r_border, BEG_POS);
	if ( std::string::npos == END_POS )
	{
		return -1;
	}

	size = END_POS - BEG_POS + 1;
	sub = base::PubStr::TrimB(src.substr(BEG_POS+1, size-2));
	return BEG_POS;
}

bool OptionSet::TryComplexSubstitute(const std::string& subs, std::string& l_sub, int& index)
{
	// 格式：SUBSTITUTE:n
	VEC_STR vec_str;
	base::PubStr::Str2StrVector(subs, ":", vec_str);

	if ( vec_str.size() != 2 )
	{
		return false;
	}

	if ( !base::PubStr::Str2Int(vec_str[1], index) )
	{
		return false;
	}

	l_sub = vec_str[0];
	return true;
}

void OptionSet::Init(const std::string& ops) throw(base::Exception)
{
	// 格式：OP1, OP2, OP3, ...
	VEC_STR vec_str;
	base::PubStr::Str2StrVector(ops, ",", vec_str);

	if ( vec_str.empty() )
	{
		throw base::Exception(ERR_CHKDLY_OPSET_FAIL, "无效的 Option Set: [%s] [FILE:%s, LINE:%d]", ops.c_str(), __FILE__, __LINE__);
	}

	unsigned int size = 0;
	if ( m_isComlex = IsComplex(vec_str[0], size) )
	{
		InitComplexSet(vec_str, size);
	}
	else
	{
		InitSet(vec_str);
	}
}

bool OptionSet::SetOption(const std::string& op)
{
	if ( op.empty() )
	{
		return false;
	}

	m_option = op;
	return true;
}

void OptionSet::Expand(const std::string& file_name, VEC_STR& vec_fn) throw(base::Exception)
{
	int          beg_pos = 0;
	std::string  new_filename;
	std::string  sub;
	unsigned int size  = 0;
	int          index = 0;
	int          off   = 0;
	VEC_STR      v_fn;

	for ( SET_VEC_STR::iterator it = m_setOP.begin(); it != m_setOP.end(); ++it )
	{
		beg_pos      = 0;
		new_filename = file_name;

		while ( (beg_pos = GetSubstitute(new_filename, beg_pos, '{', '}', sub, size)) >= 0 )
		{
			off = 0;

			if ( m_isComlex )	// 复合
			{
				if ( TryComplexSubstitute(sub, sub, index) && sub == m_option )
				{
					if ( index < 1 || index > it->size() )
					{
						throw base::Exception(ERR_CHKDLY_OPSET_FAIL, "OptionSet [%s] index is out of range: [%d] > [%lu] [FILE:%s, LINE:%d]", m_option.c_str(), index, it->size(), __FILE__, __LINE__);
					}

					new_filename.replace(beg_pos, size, (*it)[index-1]);
					off = size - (*it)[index-1].size();
				}
			}
			else	// 一般
			{
				if ( sub == m_option )
				{
					new_filename.replace(beg_pos, size, (*it)[0]);
					off = size - (*it)[0].size();
				}
			}

			beg_pos += (size - off);
		}

		v_fn.push_back(new_filename);
	}

	v_fn.swap(vec_fn);
}

bool OptionSet::IsComplex(const std::string& src, unsigned int& size) const
{
	// 格式：OP_SUB1|OP_SUB2|...
	VEC_STR vec_str;
	base::PubStr::Str2StrVector(src, "|", vec_str);

	if ( vec_str.size() > 1 )
	{
		size = vec_str.size();
		return true;
	}

	return false;
}

void OptionSet::InitSet(const VEC_STR& vs) throw(base::Exception)
{
	m_setOP.clear();
	VEC_STR vec_op;

	const int VEC_SIZE = vs.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		const std::string& ref_s = vs[i];
		if ( ref_s.empty() )
		{
			throw base::Exception(ERR_CHKDLY_OPSET_FAIL, "存在为空的 Option Set! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
		}

		vec_op.assign(1, ref_s);
		m_setOP.insert(vec_op);
	}
}

void OptionSet::InitComplexSet(const VEC_STR& vs, unsigned int size) throw(base::Exception)
{
	m_setOP.clear();
	VEC_STR vec_op;

	const int VEC_SIZE = vs.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		const std::string& ref_s = vs[i];
		base::PubStr::Str2StrVector(ref_s, "|", vec_op);

		if ( vec_op.size() != size )
		{
			throw base::Exception(ERR_CHKDLY_OPSET_FAIL, "不一致的 Option Set: [%s] [FILE:%s, LINE:%d]", ref_s.c_str(), __FILE__, __LINE__);
		}

		m_setOP.insert(vec_op);
	}
}

