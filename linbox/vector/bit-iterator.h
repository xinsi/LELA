/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/vector/bit-iterator.h
 * Copyright 2011 Bradford Hovinen <hovinen@gmail.com>
 *
 * Reference- and iterator-clases for bit-vectors
 * -------------------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __BIT_ITERATOR_H
#define __BIT_ITERATOR_H

#include <stdexcept>
#include <vector>

namespace LinBox
{

#undef __LINBOX_SIZEOF_BITVECTOR_WORD_TYPE
#define __LINBOX_BITVECTOR_WORD_TYPE unsigned long long
#define __LINBOX_SIZEOF_BITVECTOR_WORD_TYPE __LINBOX_SIZEOF_LONG

/** Binary constant defined both for 32 and 64 bits
 */
#if (__LINBOX_SIZEOF_BITVECTOR_WORD_TYPE == 1)
#define __LINBOX_BITSOF_LONG 8
#define __LINBOX_BITSOF_LONG_MUN 7
#define __LINBOX_LOGOF_SIZE 3
#define __LINBOX_POS_ALL_ONES 0x07
const __LINBOX_BITVECTOR_WORD_TYPE __LINBOX_ALL_ONES = static_cast<const __LINBOX_BITVECTOR_WORD_TYPE>(-1);
#define __LINBOX_PARITY(s) ParallelParity(s)

    inline bool ParallelParity(unsigned char t) {
	t ^= (t >> 4);
        t &= 0xf;
        return bool( (0x6996 >> t) & 0x1);
    }
#endif
#if (__LINBOX_SIZEOF_BITVECTOR_WORD_TYPE == 4)
#define __LINBOX_BITSOF_LONG 32
#define __LINBOX_BITSOF_LONG_MUN 31
#define __LINBOX_LOGOF_SIZE 5
#define __LINBOX_POS_ALL_ONES 0x1F
const __LINBOX_BITVECTOR_WORD_TYPE __LINBOX_ALL_ONES = static_cast<const __LINBOX_BITVECTOR_WORD_TYPE>(-1);
#define __LINBOX_PARITY(s) ParallelParity(s)

    inline bool ParallelParity(__LINBOX_BITVECTOR_WORD_TYPE t) {
	t ^= (t >> 16);
	t ^= (t >> 8);
	t ^= (t >> 4);
        t &= 0xf;
        return bool( (0x6996 >> t) & 0x1);
    }
#endif
#if (__LINBOX_SIZEOF_BITVECTOR_WORD_TYPE == 8)
#define __LINBOX_BITSOF_LONG 64
#define __LINBOX_BITSOF_LONG_MUN 63
#define __LINBOX_LOGOF_SIZE 6
#define __LINBOX_POS_ALL_ONES 0x3F
const __LINBOX_BITVECTOR_WORD_TYPE __LINBOX_ALL_ONES = static_cast<const __LINBOX_BITVECTOR_WORD_TYPE>(-1);
#define __LINBOX_PARITY(s) ParallelParity(s)

    inline bool ParallelParity(__LINBOX_BITVECTOR_WORD_TYPE t) {
	t ^= (t >> 32);
	t ^= (t >> 16);
	t ^= (t >> 8);
	t ^= (t >> 4);
        t &= 0xf;
        return bool( (0x6996 >> t) & 0x1);
    }
#endif

// Generic routines for big endian word-order

// Big endian version: Position zero is the highest bit on the word
// (so it prints the right way around if you convert the word to
// binary)
template <class _word>
class BigEndian {
public:
	typedef _word word;

	// Constant representing a one in the position zero in the word
	static const word e_0 = 1ULL << __LINBOX_BITSOF_LONG_MUN;

	// Shift the given word pos positions to the right
	static inline word shift_right (word w, uint8 pos) { return w >> pos; }

	// Shift the given word pos positions to the left
	static inline word shift_left (word w, uint8 pos) { return w << pos; }

	// Return a word with all positions from pos onwards set to one and the rest set to zero
	static inline word mask_right (uint8 pos) { return ((e_0 >> pos) - 1); }

	// Return a word with all positions up to and not including pos set to one and the rest set to zero
	static inline word mask_left (uint8 pos) { return ~((e_0 >> pos) - 1); }

	// Return a word with only the bit at the lowest position of the input word set
	static inline word first_position (word w) {
		word v = e_0;

		while (v && !(w & v)) w >>= 1;

		return w;
	}

	// Return e_j
	static inline word e_j (uint8 j) { return shift_right (e_0, j); }
};

// Little endian version: position zero is in the lowest position in
// the word (so e_k is 2^k)
template <class _word>
class LittleEndian {
public:
	typedef _word word;

	// Constant representing a one in the position zero in the word
	static const word e_0 = 1ULL;

	// Shift the given word pos positions to the right
	static inline word shift_right (word w, uint8 pos) { return w << pos; }

	// Shift the given word pos positions to the left
	static inline word shift_left (word w, uint8 pos) { return w >> pos; }

	// Return a word with all positions from pos onwards set to one and the rest set to zero
	static inline word mask_right (uint8 pos) { return ~((e_0 << pos) - 1); }

	// Return a word with all positions up to and not including pos set to one and the rest set to zero
	static inline word mask_left (uint8 pos) { return ((e_0 << pos) - 1); }

	// Return a word with only the bit at the lowest position of the input word set
	static inline word first_position (word w) { return ((w ^ (w - 1)) >> 1) + 1; }

	// Return e_j
	static inline word e_j (uint8 j) { return shift_right (e_0, j); }
};

template <class word_iterator, class const_word_iterator, class Endianness>
class BitVectorIterator;

template <class const_word_iterator, class Endianness>
class BitVectorConstIterator;

template <class word_iterator, class _Endianness = LittleEndian<typename std::iterator_traits<word_iterator>::value_type> >
class BitVectorReference
{
    public:
	typedef _Endianness Endianness;

	BitVectorReference (word_iterator word, uint8 position)
		: _word (word), _pos (position) {}

	~BitVectorReference () {}

	BitVectorReference &operator = (BitVectorReference &a) 
		{ return *this = (bool) a; }

	BitVectorReference &operator = (bool v) 
	{ 
		*_word = v ? (*_word | (Endianness::e_j (_pos))) : (*_word & ~Endianness::e_j (_pos));
		return *this;
	}

	BitVectorReference &operator &= (BitVectorReference &a) 
		{ *_word &= ~Endianness::e_j (_pos) | Endianness::shift_right (a.get_bit (), _pos - a._pos); return *this; }

	BitVectorReference &operator &= (bool v) 
		{ *_word &= ~Endianness::e_j (_pos) | (v & Endianness::e_j (_pos)); return *this; }

	BitVectorReference &operator |= (BitVectorReference &a) 
		{ *_word |= Endianness::shift_right (a.get_bit (), _pos - a._pos); return *this; }

	BitVectorReference &operator |= (bool v) 
		{ *_word |= v & Endianness::e_j (_pos); return *this; }

	BitVectorReference &operator ^= (BitVectorReference &a) 
		{ *_word ^= Endianness::shift_right (a.get_bit (), _pos - a._pos); return *this; }

	BitVectorReference &operator ^= (bool v) 
		{ *_word ^= v & Endianness::e_j (_pos); return *this; }

	operator bool (void) const
		{ return *_word & Endianness::e_j (_pos); return *this; }

    private:
	template <class _word_iterator, class const_word_iterator, class Endianness>
	friend class BitVectorIterator;

	template <class const_word_iterator, class Endianness>
	friend class BitVectorConstIterator;

	template <class const_word_iterator, class Endianness>
	friend class BitVectorConstReference;

	typename std::iterator_traits<word_iterator>::value_type neg_mask_word (void) { return *_word & ~Endianness::e_j (_pos); }
	typename std::iterator_traits<word_iterator>::value_type get_bit ()           { return *_word & Endianness::e_j (_pos); }

	word_iterator _word;
	uint8         _pos;
};

template <class word_iterator>
inline std::istream &operator >> (std::istream &is, BitVectorReference<word_iterator> &a) 
	{ bool v; is >> v; a = v; return is; }

template <class word_iterator>
inline std::ostream &operator << (std::ostream &os, BitVectorReference<word_iterator> &a) 
	{ os << bool (a); return os; }

template <class const_word_iterator, class _Endianness = LittleEndian<typename std::iterator_traits<const_word_iterator>::value_type> >
class BitVectorConstReference
{
    public:
	typedef _Endianness Endianness;

	template <class Iterator>
	BitVectorConstReference (BitVectorReference<Iterator, Endianness> r)
		: _word (r._word), _pos (r._pos) {}

	template <class Iterator>
	BitVectorConstReference (Iterator word, uint8 position)
		: _word (word), _pos (position) {}

	BitVectorConstReference (const_word_iterator word, uint8 position)
		: _word (word), _pos (position) {}

	~BitVectorConstReference () {}

	operator bool (void) const
		{ return *_word & Endianness::e_j (_pos); }

    private:
	friend class BitVectorConstIterator<const_word_iterator, Endianness>;

	const_word_iterator _word;
	uint8               _pos;
};

template<class const_word_iterator>
inline std::ostream &operator << (std::ostream &os, BitVectorConstReference<const_word_iterator> &a) 
	{ os << bool (a); return os; }

// class BitVectorIterator : public std::iterator <std::random_access_iterator_tag, bool>
template <class _word_iterator, class _const_word_iterator, class _Endianness = LittleEndian<typename std::iterator_traits<_word_iterator>::value_type> >
class BitVectorIterator : public std::_Bit_iterator
{
    public:

	typedef _word_iterator word_iterator;
	typedef _const_word_iterator const_word_iterator;
	typedef _Endianness Endianness;

	typedef std::random_access_iterator_tag iterator_category;
	typedef BitVectorReference<word_iterator, Endianness> reference;
	typedef BitVectorConstReference<const_word_iterator, Endianness> const_reference;
	typedef bool *pointer;
	typedef bool value_type;
	typedef long difference_type;
	typedef size_t size_type;

	BitVectorIterator () : _ref (word_iterator (), 0UL) {}
	BitVectorIterator (word_iterator word, uint8 position) : _ref (word, position) {}
	BitVectorIterator (const BitVectorIterator &i) : _ref (i._ref._word, i._ref._pos) {}

	BitVectorIterator &operator = (const BitVectorIterator &i) {
		_ref._word = i._ref._word;
		_ref._pos = i._ref._pos;
		return *this;
	}

	BitVectorIterator &operator ++ () 
	{
		if (++_ref._pos > __LINBOX_BITSOF_LONG_MUN) {
			++_ref._word;
			_ref._pos = 0UL;
		}

		return *this;
	}

	BitVectorIterator operator ++ (int) 
	{
		BitVectorIterator tmp (*this);
		++*this;
		return tmp;
	}

	BitVectorIterator operator + (difference_type i) const
	{
		word_iterator new_word = _ref._word + (i >> __LINBOX_LOGOF_SIZE);
		uint8 new_pos = _ref._pos + (i & __LINBOX_POS_ALL_ONES);

		new_word += new_pos >> __LINBOX_LOGOF_SIZE;
		new_pos &= __LINBOX_POS_ALL_ONES;

		return BitVectorIterator (new_word, new_pos);
	}

	BitVectorIterator &operator += (difference_type i) 
	{
		_ref._word += i >> __LINBOX_LOGOF_SIZE;
		_ref._pos  += i & __LINBOX_POS_ALL_ONES;
		_ref._word += _ref._pos >> __LINBOX_LOGOF_SIZE;
		_ref._pos  &= __LINBOX_POS_ALL_ONES;
		return *this;
	}

	BitVectorIterator &operator -- () 
	{
		if (--_ref._pos > __LINBOX_BITSOF_LONG_MUN) {
			--_ref._word;
			_ref._pos = __LINBOX_BITSOF_LONG_MUN;
		}

		return *this;
	}

	BitVectorIterator operator -- (int) 
	{
		BitVectorIterator tmp (*this);
		--*this;
		return tmp;
	}

	BitVectorIterator operator - (difference_type i) const
		{ return *this + -i; }

	BitVectorIterator &operator -= (difference_type i) 
		{ return *this += -i; }

	difference_type operator - (BitVectorIterator i) const 
		{ return (_ref._word - i._ref._word) * __LINBOX_BITSOF_LONG + (_ref._pos - i._ref._pos); }

	reference operator [] (long i) 
		{ return *(*this + i); }

	reference operator * () 
		{ return _ref; }

	const_reference operator * () const 
		{ return _ref; }

	bool operator == (const BitVectorIterator &c) const 
		{ return (_ref._word == c._ref._word) && (_ref._pos == c._ref._pos); }

	bool operator != (const BitVectorIterator &c) const 
		{ return (_ref._word != c._ref._word) || (_ref._pos != c._ref._pos); }

	word_iterator word () const { return _ref._word; }
	uint8 pos () const { return _ref._pos; }

    private:
	friend class BitVectorConstIterator<const_word_iterator, Endianness>;

	reference _ref;
};

template <class _const_word_iterator, class _Endianness = LittleEndian<typename std::iterator_traits<_const_word_iterator>::value_type> >
class BitVectorConstIterator : public std::iterator <std::random_access_iterator_tag, bool>
{
    public:

	typedef _const_word_iterator word_iterator;
	typedef _const_word_iterator const_word_iterator;
	typedef _Endianness Endianness;

	typedef std::random_access_iterator_tag iterator_category;
	typedef BitVectorConstReference<const_word_iterator, Endianness> reference;
	typedef reference const_reference;
	typedef const bool *pointer;
	typedef bool value_type;
	typedef long difference_type;
	typedef size_t size_type;

	BitVectorConstIterator () : _ref (const_word_iterator (), 0UL) {}
	BitVectorConstIterator (const_word_iterator word, uint8 position) : _ref (word, position) {}
	BitVectorConstIterator (const BitVectorConstIterator &i) : _ref (i._ref._word, i._ref._pos) {}
	template <class Iterator>
	BitVectorConstIterator (const BitVectorIterator<Iterator, const_word_iterator, Endianness> &i) : _ref (i._ref._word, i._ref._pos) {}

	BitVectorConstIterator &operator = (const BitVectorConstIterator &i) {
		_ref._word = i._ref._word;
		_ref._pos = i._ref._pos;
		return *this;
	}

	template <class Iterator>
	BitVectorConstIterator &operator = (const BitVectorIterator<Iterator, const_word_iterator, Endianness> &i) {
		_ref._word = i._ref._word;
		_ref._pos = i._ref._pos;
		return *this;
	}

	BitVectorConstIterator &operator ++ () 
	{
		if (++_ref._pos > __LINBOX_BITSOF_LONG_MUN) {
			++_ref._word;
			_ref._pos = 0UL;
		}

		return *this;
	}

	BitVectorConstIterator operator ++ (int) 
	{
		BitVectorConstIterator tmp (*this);
		++*this;
		return tmp;
	}

	BitVectorConstIterator operator + (long i) const
	{
		const_word_iterator new_word = _ref._word + (i >> __LINBOX_LOGOF_SIZE);
		uint8 new_pos = _ref._pos + (i & __LINBOX_POS_ALL_ONES);

		new_word += new_pos >> __LINBOX_LOGOF_SIZE;
		new_pos &= __LINBOX_POS_ALL_ONES;

		return BitVectorConstIterator (new_word, new_pos);
	}

	BitVectorConstIterator &operator += (long i) 
	{
		_ref._word += i >> __LINBOX_LOGOF_SIZE;
		_ref._pos  += i & __LINBOX_POS_ALL_ONES;
		_ref._word += _ref._pos >> __LINBOX_LOGOF_SIZE;
		_ref._pos  &= __LINBOX_POS_ALL_ONES;
		return *this;
	}

	BitVectorConstIterator &operator -- () 
	{
		if (--_ref._pos > __LINBOX_BITSOF_LONG_MUN) {
			--_ref._word;
			_ref._pos = __LINBOX_BITSOF_LONG_MUN;
		}

		return *this;
	}

	BitVectorConstIterator operator -- (int) 
	{
		BitVectorConstIterator tmp (*this);
		--*this;
		return tmp;
	}

	BitVectorConstIterator operator - (difference_type i) const 
		{ return *this + -i; }

	BitVectorConstIterator &operator -= (difference_type i) 
		{ return *this += -i; }

	difference_type operator - (BitVectorConstIterator i) const 
		{ return (_ref._word - i._ref._word) * __LINBOX_BITSOF_LONG + (_ref._pos - i._ref._pos); }

	reference operator [] (difference_type i) const
		{ return *(*this + i); }

	reference operator * () const
		{ return _ref; }

	bool operator == (const BitVectorConstIterator &c) const 
		{ return (_ref._word == c._ref._word) && (_ref._pos == c._ref._pos); }

	template <class Iterator>
	bool operator == (const BitVectorIterator<Iterator, const_word_iterator, Endianness> &c) const 
		{ return (_ref._word == c._ref._word) && (_ref._pos == c._ref._pos); }

	bool operator != (const BitVectorConstIterator &c) const 
		{ return (_ref._word != c._ref._word) || (_ref._pos != c._ref._pos); }

	template <class Iterator>
	bool operator != (const BitVectorIterator<Iterator, const_word_iterator, Endianness> &c) const 
		{ return (_ref._word != c._ref._word) || (_ref._pos != c._ref._pos); }

	const_word_iterator word () const { return _ref._word; }
	uint8 pos () const { return _ref._pos; }

    private:

	const_reference _ref;
};

} // namespace LinBox

namespace std 
{
	template <class word_iterator, class const_word_iterator, class _Endianness>
	struct iterator_traits<LinBox::BitVectorIterator<word_iterator, const_word_iterator, _Endianness> >
	{
		typedef random_access_iterator_tag iterator_category;
		typedef LinBox::BitVectorReference<word_iterator, _Endianness> reference;
		typedef bool *pointer;
		typedef bool value_type;
		typedef long difference_type;
	};

	template <class const_word_iterator, class _Endianness>
	struct iterator_traits<LinBox::BitVectorConstIterator<const_word_iterator, _Endianness> >
	{
		typedef random_access_iterator_tag iterator_category;
		typedef LinBox::BitVectorConstReference<const_word_iterator, _Endianness> reference;
		typedef const bool *pointer;
		typedef bool value_type;
		typedef long difference_type;
	};
}

#endif // __BIT_ITERATOR_H